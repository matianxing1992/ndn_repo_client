#include <ndn-repo-client/command_checker.hpp>

NDN_LOG_INIT(ndn_repo_client.command_checker);

CommandChecker::CommandChecker(ndn::Face &face)
    :m_face(face)
{
    NDN_LOG_TRACE("Init CommandChecker");
}

void CommandChecker::check_insert(ndn::Name repo_name, ndn::span<const uint8_t>& request_no, CheckCallback _callback)
{
    _check("insert",repo_name,request_no,_callback);
}

void CommandChecker::check_delete(ndn::Name repo_name, ndn::span<const uint8_t>& request_no, CheckCallback _callback)
{
    _check("delete",repo_name,request_no,_callback);
}

void CommandChecker::_check(std::string method, ndn::Name repo_name, ndn::span<const uint8_t>& request_no, CheckCallback _callback)
{

    // ndn_repo_client::RequestNo requestNo(*request_no);
    // ndn_repo_client::RepoStatQuery repoStatQuery(requestNo); 
    ndn::Name checkName = ndn::Name(repo_name).append(ndn::Name(method+" check"));

    ndn::Interest checkInterest(checkName);
    
    checkInterest.setApplicationParameters(ndn::makeBinaryBlock(ndn_repo_client::REQUEST_NO_TYPE,request_no));
    checkInterest.setMustBeFresh(true);
    checkInterest.setCanBePrefix(false);
    checkInterest.setInterestLifetime(ndn::time::milliseconds(10000));

    NDN_LOG_TRACE("check : " << checkInterest.toUri());

    m_face.expressInterest(checkInterest,
            [&,_callback](const ndn::Interest& interest, const ndn::Data& data)
        {
            // afterSatisfied
            NDN_LOG_TRACE("check interest satisfied: " << interest.toUri());
            // ndn::Block _block_repoCommandRes(ndn_repo_client::REPO_COMMAND_RES,data.getContent());
            auto content = data.getContent();
            content.parse();
            _callback(ndn::readNonNegativeInteger(content.elements().at(0)));

        },
        [&,_callback](const ndn::Interest& interest, const ndn::lp::Nack&)
        {
            // afterNacked
            _callback(ndn_repo_client::FAILED);
            NDN_LOG_DEBUG("check interest nacked: " << interest.toUri());

        },
        [&,_callback](const ndn::Interest& interest)
        {
            // afterTimeout
            _callback(ndn_repo_client::IN_PROGRESS);
            NDN_LOG_DEBUG("check interest timeout:" << interest.toUri());

    });
}
