#ifndef NDN_REPO_CLIENT_GETDATA_HPP
#define NDN_REPO_CLIENT_GETDATA_HPP


#include <ndn-cxx/face.hpp>
#include "ndn-cxx/util/logger.hpp"
#include <ndn-cxx/util/segmenter.hpp>
#include <ndn-cxx/ims/in-memory-storage-fifo.hpp>
#include "ndn-repo-client/utils/SegmentFetcher.hpp"
#include <ndn-cxx/security/validator-null.hpp>

#include <ndn-repo-client/tlv.hpp>
#include <ndn-repo-client/pubsub.hpp>

#include <ndn-cxx/util/sha256.hpp>


using FetchedDataCallback = std::function<void(ndn::span<const uint8_t>)>;



class GetDataClient
{
public:

    GetDataClient(ndn::Face& face,ndn::Name repo_name);

    void fetch_object(ndn::Name name_at_repo,FetchedDataCallback onFetchedData);



private:
    ndn::Face& m_face;
    ndn::Name m_repo_name;
};


#endif