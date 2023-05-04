#include "command_checker.hpp"

NDN_LOG_INIT(ndn_repo_client.command_checker);

CommandChecker::CommandChecker(ndn::Face &face)
    :m_face(face)
{
    
}

void CommandChecker::check_insert(ndn::Name repo_name, ndn::span<const uint8_t> request_no, CheckCallback _callback)
{
    _check("insert",repo_name,request_no,_callback);
}

void CommandChecker::check_delete(ndn::Name repo_name, ndn::span<const uint8_t> request_no, CheckCallback _callback)
{
    _check("delete",repo_name,request_no,_callback);
}

void CommandChecker::_check(std::string method, ndn::Name repo_name, ndn::span<const uint8_t> request_no, CheckCallback _callback)
{

    ndn_repo_client::RequestNo requestNo(request_no);
    ndn_repo_client::RepoStatQuery repoStatQuery(requestNo); 
    ndn::Name checkName = ndn::Name(repo_name).append(ndn::Name(method+" check"));

    ndn::Interest checkInterest(checkName);
    checkInterest.setApplicationParameters(repoStatQuery.wireEncode());
    checkInterest.setMustBeFresh(true);
    checkInterest.setCanBePrefix(false);
    checkInterest.setInterestLifetime(ndn::time::milliseconds(1000));

    NDN_LOG_TRACE("check : " << checkInterest.toUri());

    m_face.expressInterest(checkInterest,
            [&](const ndn::Interest&, const ndn::Data& data)
        {
            // afterSatisfied
            NDN_LOG_TRACE("check interest satisfied: " << checkInterest.toUri());
            ndn::Block _block_repoCommandRes(ndn_repo_client::REPO_COMMAND_RES,data.getContent());
            _callback(ndn_repo_client::RepoCommandRes(_block_repoCommandRes));

        },
        [&](const ndn::Interest&, const ndn::lp::Nack&)
        {
            // afterNacked
            NDN_LOG_DEBUG("check interest nacked: " << checkInterest.toUri());

        },
        [&](const ndn::Interest&)
        {
            // afterTimeout
            NDN_LOG_DEBUG("check interest timeout:" << checkInterest.toUri());

    });
}
