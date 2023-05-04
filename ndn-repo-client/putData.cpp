#include "putData.hpp"

NDN_LOG_INIT(ndn_repo_client.putData);

PutDataClient::PutDataClient(ndn::Face& face, const ndn::Name& prefix, const ndn::Name& repo_name,ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo, ndn::Name forwardingHint,size_t imsLimit)
    :m_face(face),
     m_keyChain(keyChain),
     m_segmenter(m_keyChain, signingInfo),
     m_ims(imsLimit),
     pubSub(m_face,prefix,forwardingHint,imsLimit),
     commandChecker(m_face)
{
    this->m_prefix=prefix;
    this->m_repo_name=repo_name;
}


void PutDataClient::_on_interest(const ndn::InterestFilter &filter, const ndn::Interest &interest)
{
    auto data = m_ims.find(interest.getName());
    if (data != nullptr)
    {
        m_face.put(*data);
        NDN_LOG_TRACE("Reply from IMS : " << interest.getName());
    }
    else
    {
        NDN_LOG_TRACE("Failed to reply from IMS : " << interest.getName());
    }
}

ndn::span<const uint8_t> PutDataClient::insert_object(ndn::span<const uint8_t> object, ndn::Name name_at_repo, int segment_size, ndn::time::milliseconds freshness_period, int cpu_count, ResultCallback onResult, ndn::Name forwarding_hint, ndn::Name register_prefix, ndn::Name check_prefix)
{
    auto segments = m_segmenter.segment(object,ndn::Name(name_at_repo).appendVersion(),segment_size, freshness_period);
    
    for (auto segment:segments)
        m_ims.insert(*segment,ndn::time::milliseconds(600000));

    int num_packets=segments.size();

    // If the uploaded file has the client's name as prefix, set an interest filter
    // for handling corresponding Interests from the repo
    if (m_repo_name.isPrefixOf(name_at_repo))
    {
        NDN_LOG_DEBUG("set interest filter : " << name_at_repo.toUri());
        m_face.setInterestFilter(ndn::InterestFilter(name_at_repo),std::bind(&PutDataClient::_on_interest,this,_1,_2));
    }
    else
    {
        // Otherwise, register the file name as prefix for responding interests from the repo
        NDN_LOG_DEBUG("set interest filter and register prefix : " << name_at_repo.toUri());
        m_face.setInterestFilter(ndn::InterestFilter(name_at_repo),std::bind(&PutDataClient::_on_interest,this,_1,_2),
            [&](auto,auto){
                NDN_LOG_DEBUG("failed to register prefix : " << name_at_repo.toUri());
            });
    }

    // construct insert cmd msg
    ndn_repo_client::ObjectParam objectParam(name_at_repo,forwarding_hint,0,num_packets-1,register_prefix);
    std::vector<ndn_repo_client::ObjectParam> objectParams;
    objectParams.push_back(objectParam);
    ndn_repo_client::RepoCommandParam repoCommandParam(objectParams);

    // The request number of the command is always the SHA256 hash of the command data published in Pub-Sub
    ndn::span<const uint8_t> repoCommandParamBytes=repoCommandParam.wireEncode().value_bytes();
    auto request_no_buffer = ndn::util::Sha256::computeDigest(repoCommandParamBytes);
    ndn::span<const uint8_t> request_no = ndn::span<const uint8_t>(request_no_buffer->data(),request_no_buffer->size());

    pubSub.publish(m_repo_name.append(ndn::Name("insert")),repoCommandParamBytes,
        [&](auto isSuccess){
            // using PublishCallback = std::function<void(bool)>;
            if(isSuccess){
                NDN_LOG_TRACE("Published an insert msg and was acknowledged by a subscriber");
                // check the insert progress
                // int n_tries = 3;
                // commandChecker.check_insert(m_repo_name,request_no,[&](auto){});
            }else{
                NDN_LOG_TRACE("Published an insert msg but was not acknowledged by a subscriber");
            }
        });

    return request_no;
}

