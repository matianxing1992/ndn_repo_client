#include "delete.hpp"

NDN_LOG_INIT(ndn_repo_client.delete);

DeleteClient::DeleteClient(ndn::Face &face, ndn::Name& prefix, ndn::Name& repo_name)
    :m_face(face),
    m_prefix(prefix),
    m_repo_name(repo_name),
    pubSub(m_face,m_prefix),
    commandChecker(m_face)
{
    NDN_LOG_TRACE("Init DeleteClient");
}

void DeleteClient::delete_object(ndn::span<const uint8_t>& request_no,ndn::Name& name_at_repo, DeleteCallback _callback,ndn::Name& register_prefix,ndn::Name& check_prefix, uint64_t startBlockId, uint64_t endBlockId)
{
    // send command interest
    ndn_repo_client::ObjectParam objectParam(name_at_repo,nullptr,startBlockId,endBlockId,&register_prefix);
    std::shared_ptr<ndn_repo_client::ObjectParam> objectParam_ptr=std::make_shared<ndn_repo_client::ObjectParam>(objectParam);
    NDN_LOG_TRACE("construct delete cmd msg: ObjectParam");

    std::vector<std::shared_ptr<ndn_repo_client::ObjectParam>> objectParams;
    objectParams.push_back(objectParam_ptr);
    
    ndn_repo_client::RepoCommandParam repoCommandParam(objectParams);
    NDN_LOG_TRACE("construct delete cmd msg: RepoCommandParam");

    // The request number of the command is always the SHA256 hash of the command data published in Pub-Sub
    // ndn::span<const uint8_t> repoCommandParamBytes=repoCommandParam.wireEncode().value_bytes();
    ndn::span<const uint8_t> repoCommandParamBytes(objectParam_ptr->wireEncode());
    static auto request_no_buffer = ndn::util::Sha256::computeDigest(repoCommandParamBytes);
    request_no = ndn::span<const uint8_t>(request_no_buffer->begin(),request_no_buffer->end());
    // ndn::span<const uint8_t> request_no = ndn::span<const uint8_t>(request_no_buffer->begin(),request_no_buffer->end());

    ndn::Name deleteName(m_repo_name);
    deleteName.append(ndn::Name("delete"));
    pubSub.publish(deleteName,repoCommandParamBytes,
        [&](auto isSuccess){
            // using PublishCallback = std::function<void(bool)>;
            if(isSuccess){
                NDN_LOG_TRACE("Published an delete msg and was acknowledged by a subscriber");
                // check the insert progress
                // int n_tries = 3;
                // commandChecker.check_delete(m_repo_name,request_no,[&](auto){});
            }else{
                NDN_LOG_TRACE("Published an delete msg but was not acknowledged by a subscriber");
            }
            _callback(isSuccess);
        });
}
