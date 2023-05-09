/*
    A c++ implementation of the ndn-python-repo/putfile
    https://github.com/UCLA-IRL/ndn-python-repo/blob/master/ndn_python_repo/clients/putfile.py

*/

#ifndef NDN_REPO_CLIENT_PUTDATA_HPP
#define NDN_REPO_CLIENT_PUTDATA_HPP

#include <ndn-cxx/face.hpp>
#include "tlv.hpp"
#include "ndn-cxx/util/logger.hpp"
#include <ndn-cxx/util/segmenter.hpp>
#include <ndn-cxx/ims/in-memory-storage-fifo.hpp>
#include <pubsub.hpp>
#include <command_checker.hpp>





using ResultCallback = std::function<void(bool)>;


class PutDataClient
{
public:
    // A client to insert files into the repo
    // face: NDN Face; prefix: The name of this client; repo_name: Routable name to remote repo.
    PutDataClient(ndn::Face& face,const ndn::Name& prefix,const ndn::Name& repo_name, ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo = ndn::security::SigningInfo(),ndn::Name* forwardingHint=nullptr, size_t imsLimit = 6000);

    // return request_no
    void insert_object(std::shared_ptr<ndn::span<const uint8_t>> object,ndn::Name& name_at_repo,int segment_size,ndn::time::milliseconds freshness_period, int cpu_count , ResultCallback onResult,
                        ndn::Name* forwarding_hint, ndn::Name& register_prefix, ndn::Name& check_prefix,ndn::span<const uint8_t>& request_no);

    // return request_no
    ndn::span<const uint8_t> insert_objects(std::vector<ndn::span<const uint8_t>> objects,ndn::Name name_at_repo,int segment_size,ndn::time::milliseconds freshness_period, int cpu_count , ResultCallback onResult,
                        ndn::Name* forwarding_hint, ndn::Name register_prefix, ndn::Name check_prefix);

private:
    void _on_interest(const ndn::InterestFilter& filter, const ndn::Interest& interest);


private:
    ndn::Face& m_face;
    ndn::Name m_prefix;
    ndn::Name m_repo_name;

    PubSub pubSub;

    ndn::util::Segmenter m_segmenter;

    ndn::InMemoryStorageFifo m_ims;

    ndn::KeyChain& m_keyChain;

    CommandChecker commandChecker;
};


#endif