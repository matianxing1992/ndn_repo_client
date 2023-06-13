#ifndef NDN_REPO_CLIENT_UTILS_READ_HANDLE_HPP
#define NDN_REPO_CLIENT_UTILS_READ_HANDLE_HPP

#include <sqlite3.h>
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/ims/in-memory-storage-fifo.hpp"
#include "ndn-cxx/interest.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio.hpp"

#include <unordered_set>
#include <string>

#include <mutex>


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

    static ReadHandle *GetInstance(ndn::Face& face,int cpu_cores=4,int cache_size=8192)
    {
        if(_handle==nullptr){
            _handle = new ReadHandle(face,cpu_cores,cache_size);
        }
        return _handle;

    }
    //return nullprt if not exist
    static ReadHandle *GetExistingEInstance()
    {
        return _handle;
    }
    
    void listen(const ndn::Name& prefix);

    void addToCache(std::string filename,std::vector<std::shared_ptr<ndn::Data>>& data,int freshness_period=60000);

private:
    ReadHandle(ndn::Face& face,int cpu_cores,int cache_size);

    void _onInterest (const ndn::InterestFilter& filter, const ndn::Interest& interest);
    
    bool loadFromStorage(const ndn::Interest& interest);

    bool replyFromCache(const ndn::Interest& interest);


private:
    static ReadHandle* _handle;
    ndn::Face& m_face;
    
    sqlite3 *db;
    boost::asio::thread_pool _threadPool;
    ndn::Scheduler _scheduler;

    int _cache_lifetime_ms;
    std::unordered_set<std::string> _cached_files;
    std::mutex _cached_files_mutex;
    std::vector<ndn::Interest> _pending_interest;
    ndn::InMemoryStorageFifo _cache;
    std::mutex _cache_mutex;

};

#endif // NDN_REPO_CLIENT_UTIL_SEGMENT_FETCHER_HPP