#ifndef NDN_REPO_CLIENT_DELETE_HPP
#define NDN_REPO_CLIENT_DELETE_HPP

#include "tlv.hpp"


#include <ndn-cxx/face.hpp>
#include "ndn-cxx/util/logger.hpp"
#include <ndn-cxx/util/segmenter.hpp>
#include <ndn-cxx/ims/in-memory-storage-fifo.hpp>
#include <ndn-cxx/util/segment-fetcher.hpp>
#include <ndn-cxx/security/validator-null.hpp>

#include <ndn-repo-client/command_checker.hpp>
#include <ndn-repo-client/tlv.hpp>
#include <ndn-repo-client/pubsub.hpp>




using DeleteCallback = std::function<void(bool)>;

class DeleteClient
{
public:

    // A client to delete data in the repo
    DeleteClient(ndn::Face& face, ndn::Name& prefix, ndn::Name& repo_name);


    //Delete from repo packets between "<name_at_repo>/<start_block_id>" and\
            "<name_at_repo>/<end_block_id>" inclusively.
    //:param register_prefix: If repo is configured with ``register_root=False``, it unregisters\
            ``register_prefix`` after receiving the deletion command.
    //   :param check_prefix: NonStrictName. The repo will publish process check messages under\
            ``<check_prefix>/check``. It is necessary to specify this value in the param, instead\
            of using a predefined prefix, to make sure the subscriber can register this prefix\
            under the NDN prefix registration security model. If not specified, default value is\
            the client prefix.
    // return request_no
    void delete_object(ndn::span<const uint8_t>& request_no,ndn::Name& name_at_repo,DeleteCallback _callback,ndn::Name& register_prefix,ndn::Name& check_prefix, uint64_t startBlockId=(uint64_t)0,uint64_t endBlockId=(uint64_t)-1);


private:
    ndn::Face& m_face;
    ndn::Name m_prefix;
    ndn::Name m_repo_name;

    PubSub pubSub;
    CommandChecker commandChecker;
};


#endif