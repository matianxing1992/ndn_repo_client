#include "delete.hpp"

DeleteClient::DeleteClient(ndn::Face &face, ndn::Name prefix, ndn::Name repo_name)
    :m_face(face),
    m_prefix(prefix),
    m_repo_name(repo_name),
    pubSub(m_face,prefix),
    commandChecker(m_face)
{
}

ndn::span<const uint8_t> DeleteClient::delete_object(ndn::Name name_at_repo, DeleteCallback _callback, uint64_t startBlockId, uint64_t endBlockId, ndn::Name register_prefix, ndn::Name check_prefix)
{
    // send command interest
    ndn_repo_client::ObjectParam objectParam(name_at_repo,nullptr,startBlockId,endBlockId,register_prefix);
    std::vector<ndn_repo_client::ObjectParam> objectParams;
    objectParams.push_back(objectParam);
    ndn_repo_client::RepoCommandParam repoCommandParam(objectParams);

    // The request number of the command is always the SHA256 hash of the command data published in Pub-Sub
    ndn::span<const uint8_t> repoCommandParamBytes=repoCommandParam.wireEncode().value_bytes();
    auto request_no_buffer = ndn::util::Sha256::computeDigest(repoCommandParamBytes);
    ndn::span<const uint8_t> request_no = ndn::span<const uint8_t>(request_no_buffer->data(),request_no_buffer->size());


    pubSub.publish(m_repo_name.append(ndn::Name("delete")),repoCommandParamBytes,
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

    return request_no;
}
