#ifndef NDN_REPO_CLIENT_COMMAND_CHECKER_HPP
#define NDN_REPO_CLIENT_COMMAND_CHECKER_HPP

#include "ndn-cxx/face.hpp"
#include "tlv.hpp"
#include "ndn-cxx/util/logger.hpp"

NDN_LOG_INIT(ndn_repo_client.command_checker);

using CheckCallback = std::function<void(ndn_repo_client::RepoCommandRes)>;


// The request number of the command is always the SHA256 hash of the command data published in Pub-Sub
class CommandChecker
{
public:

    CommandChecker(ndn::Face& face);

    // Check the status of an insert process.
    //    :param repo_name: Name. The name of the remote repo.
    //    :param request_no: bytes. The request id of the process to check.
    //    :return: The response from the repo.
    void check_insert(ndn::Name repo_name, ndn::span<const uint8_t> request_no,CheckCallback _callback);
    void check_delete(ndn::Name repo_name, ndn::span<const uint8_t> request_no,CheckCallback _callback);

private:
    void _check(std::string method, ndn::Name repo_name, ndn::span<const uint8_t> request_no,CheckCallback _callback);

private:
    ndn::Face& m_face;
};


#endif