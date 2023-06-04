#ifndef NDN_REPO_CLIENT_UTILS_READ_HANDLE_HPP
#define NDN_REPO_CLIENT_UTILS_READ_HANDLE_HPP

#include <sqlite3.h>
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/ims/in-memory-storage-fifo.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio.hpp"


class ReadHandle
{
public: 
    /**
     * Singletons should not be cloneable.
     */
    ReadHandle(ReadHandle &other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const ReadHandle &) = delete;

    static ReadHandle *GetInstance(ndn::Face& face)
    {
        if(_handle==nullptr){
            _handle = new ReadHandle(face);
        }
        return _handle;

    }

    static ReadHandle *GetInstance()
    {
        return _handle;
    }
    
    void listen(const ndn::Name& prefix);

    void addToCache(const ndn::Data& data,int freshness_period=60000);

private:
    ReadHandle(ndn::Face& face);

    void _onInterest (const ndn::InterestFilter& filter, const ndn::Interest& interest);
    
    bool loadFromStorage(const ndn::Interest& interest);

    bool replyFromCache(const ndn::Interest& interest);


private:
    static ReadHandle* _handle;
    ndn::Face& m_face;
    ndn::InMemoryStorageFifo _cache;
    sqlite3 *db;
    boost::asio::thread_pool _threadPool;
    ndn::Scheduler _scheduler;

};

#endif // NDN_REPO_CLIENT_UTIL_SEGMENT_FETCHER_HPP