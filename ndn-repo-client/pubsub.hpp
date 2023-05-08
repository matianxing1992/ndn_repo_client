/*
    https://ndn-python-repo.readthedocs.io/en/latest/src/misc_pkgs/pub_sub.html
*/
#ifndef NDN_REPO_CLIENT_PUBSUB_HPP
#define NDN_REPO_CLIENT_PUBSUB_HPP

#include "tlv.hpp"
#include <random>
#include <ndn-cxx/face.hpp>
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/ims/in-memory-storage-fifo.hpp"
#include "ndn-cxx/util/segmenter.hpp"




using SubscribeCallback = std::function<void()>;

using PublishCallback = std::function<void(bool)>;

// Only publisher now
class PubSub
{
public:
    PubSub(ndn::Face& face,ndn::Name prefix, ndn::Name* forwarding_hint = nullptr,  int ims_limit=6000);

private:
    void expressNotifyInterest(int n_retries, ndn::Interest notifyInterest, PublishCallback publishCallback);

public:  
    void publish(ndn::Name topic, ndn::span<const uint8_t>& msg, PublishCallback publishCallback);

    void subscribe(ndn::Name topic, SubscribeCallback cb);

    void unSubscribe(ndn::Name topic);

private:
    ndn::Face& m_face;
    ndn::Name m_publisher_prefix;
    ndn::Name* m_forwarding_hint;

    ndn::KeyChain m_keyChain;
    ndn::InMemoryStorageFifo m_published_data;

};



#endif