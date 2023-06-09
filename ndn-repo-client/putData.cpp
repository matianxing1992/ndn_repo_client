#include <ndn-repo-client/putData.hpp>
#include "putData.hpp"

NDN_LOG_INIT(ndn_repo_client.putData);

PutDataClient::PutDataClient(ndn::Face& face, const ndn::Name& prefix, const ndn::Name& repo_name,ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo, ndn::Name* forwardingHint,size_t imsLimit)
    :m_face(face),
     m_prefix(prefix),
     m_repo_name(repo_name),
     pubSub(m_face,prefix,forwardingHint,imsLimit),
     m_segmenter(keyChain, signingInfo),
     m_ims(imsLimit),
     m_keyChain(keyChain),
     commandChecker(m_face),
     m_scheduler(m_face.getIoService())
{
    NDN_LOG_TRACE("Init PutDataClient");
    m_face.setInterestFilter(ndn::InterestFilter(m_prefix),std::bind(&PutDataClient::_on_interest,this,_1,_2),
    [&](auto,auto){
        NDN_LOG_DEBUG("failed to register prefix : " << m_prefix.toUri());
    });
}


void PutDataClient::_on_interest(const ndn::InterestFilter &filter, const ndn::Interest &interest)
{
    auto data = m_ims.find(interest.getName());
    if (data != nullptr)
    {
        // if(interest.getName()[-1].isSegment())
        m_face.put(*data);
        // m_ims.erase(interest.getName());
        NDN_LOG_TRACE("Reply from PutDataClient IMS : " << interest.getName());
    }
    else
    {
        NDN_LOG_TRACE("Failed to reply from PutDataClient IMS : " << interest.getName());
    }
}

void PutDataClient::_check_progress(ndn::Name& m_repo_name, ndn::span<const uint8_t>& request_no, PublishCallback onResult)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    commandChecker.check_insert(m_repo_name, request_no, [&, m_repo_name, request_no, onResult](uint64_t res) mutable{
        switch (res)
        {
            case ndn_repo_client::NOT_FOUND:
                onResult(false);
                break;
            case ndn_repo_client::IN_PROGRESS:
                _check_progress(m_repo_name, request_no, onResult);
                break;
            case ndn_repo_client::COMPLETED:
                onResult(true);
                break;
            case ndn_repo_client::FAILED:
                // insertion failed, or command interest nack
                onResult(false);
                break;
            default:
                onResult(false);
        } });
}

void PutDataClient::insert_object(std::shared_ptr<ndn::span<const uint8_t>> object, ndn::Name& name_at_repo, int segment_size, ndn::time::milliseconds freshness_period, int cpu_count, ResultCallback onResult, ndn::Name* forwarding_hint, ndn::Name& register_prefix, ndn::Name& check_prefix,ndn::span<const uint8_t>& request_no)
{
    auto segments = m_segmenter.segment(*object,name_at_repo,segment_size, freshness_period);
    NDN_LOG_TRACE("create segments of the object : " << name_at_repo.toUri());
    for (const auto& segment:segments){
        m_ims.insert(*segment,ndn::time::milliseconds(freshness_period));
        m_scheduler.schedule(ndn::time::milliseconds(freshness_period), [this, name = segment->getName()] { m_ims.erase(name); });
    }
        
    NDN_LOG_TRACE("insert into local in-memory-storage " << segments.at(0)->getName().toUri());
    int num_packets=segments.size();


    // If the uploaded file has the client's name as prefix, set an interest filter
    // for handling corresponding Interests from the repo
    if (!m_prefix.isPrefixOf(name_at_repo))
    {
        // Otherwise, register the file name as prefix for responding interests from the repo
        NDN_LOG_DEBUG("set interest filter and register prefix : " << name_at_repo.toUri());
        m_face.setInterestFilter(ndn::InterestFilter(name_at_repo),std::bind(&PutDataClient::_on_interest,this,_1,_2),
            [&](auto,auto){
                NDN_LOG_DEBUG("failed to register prefix : " << name_at_repo.toUri());
            });
    }

    // construct insert cmd msg
    ndn_repo_client::ObjectParam objectParam(name_at_repo,forwarding_hint,(uint64_t)0,(uint64_t)(num_packets-1),&register_prefix);
    std::shared_ptr<ndn_repo_client::ObjectParam> objectParam_ptr=std::make_shared<ndn_repo_client::ObjectParam>(objectParam);
    NDN_LOG_TRACE("construct insert cmd msg: ObjectParam");

    std::vector<std::shared_ptr<ndn_repo_client::ObjectParam>> objectParams;
    objectParams.push_back(objectParam_ptr);

    ndn_repo_client::RepoCommandParam repoCommandParam(objectParams);
    NDN_LOG_TRACE("construct insert cmd msg: RepoCommandParam");

    // The request number of the command is always the SHA256 hash of the command data published in Pub-Sub
    // ndn::span<const uint8_t> repoCommandParamBytes=repoCommandParam.wireEncode().value_bytes();
    ndn::span<const uint8_t> repoCommandParamBytes(objectParam_ptr->wireEncode());
    static std::shared_ptr<const std::vector<uint8_t>> request_no_buffer = ndn::util::Sha256::computeDigest(repoCommandParamBytes);
    request_no = ndn::span<const uint8_t>(request_no_buffer->begin(),request_no_buffer->end());
    // static auto ret = std::make_shared<ndn::span<const uint8_t>>(request_no);


    ndn::Name insertName(m_repo_name);
    insertName.append(ndn::Name("insert"));
    static auto handler = [&,onResult,request_no](bool isSuccess) mutable {
            // using PublishCallback = std::jjfunction<void(bool)>;
            if(isSuccess){
                NDN_LOG_TRACE("Published an insert msg and was acknowledged by a subscriber");
                // check the insert progress
                // int n_tries = 3;
                _check_progress(m_repo_name, request_no, onResult);

            }else{
                NDN_LOG_TRACE("Published an insert msg but was not acknowledged by a subscriber");
                onResult(isSuccess);
            }
            
        };
    pubSub.publish(insertName,repoCommandParamBytes,handler);

}



