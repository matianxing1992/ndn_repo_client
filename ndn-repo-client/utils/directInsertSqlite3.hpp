/*
    Directly insert data into local sqlite3 database
*/

#ifndef NDN_REPO_CLIENT_DIRECTINSERTSQLITE3_HPP
#define NDN_REPO_CLIENT_DIRECTINSERTSQLITE3_HPP

#include <sqlite3.h>
#include <stdexcept>
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/name.hpp"
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/segmenter.hpp>
#include "ndn-repo-client/utils/ReadHandle.hpp"

#include "boost/asio/thread_pool.hpp"
#include "boost/asio.hpp"

#include "ndn-cxx/util/sha256.hpp"

class DirectInsertSqlite3Client
{
public: 
    /**
     * Singletons should not be cloneable.
     */
    DirectInsertSqlite3Client(DirectInsertSqlite3Client &other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const DirectInsertSqlite3Client &) = delete;

    static DirectInsertSqlite3Client *GetInstance(ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo)
    {
        if(_client==nullptr){
            _client = new DirectInsertSqlite3Client(keyChain, signingInfo);
        }
        return _client;
    }

    // return nullptr if not exist
    static DirectInsertSqlite3Client *GetExistingInstance()
    {
        return _client;
    }

    void putData(ndn::Name& name_at_repo,std::shared_ptr<ndn::span<const uint8_t>> object, int segment_size=8000, 
        ndn::time::milliseconds freshness_period=ndn::time::milliseconds(60000));

private:
    DirectInsertSqlite3Client(ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo, int cpu_cores=8);

    ~DirectInsertSqlite3Client()
    {
        sqlite3_close(db);
        delete _client;
    }

    void _put(ndn::span<const uint8_t>& key, ndn::span<const uint8_t>& value, uint64_t expire_time_ms);

    void _putMany(std::vector<std::shared_ptr<ndn::Data>> data, uint64_t expire_time_ms);

    static DirectInsertSqlite3Client* _client;

    sqlite3 *db;

    ndn::util::Segmenter m_segmenter;

    boost::asio::thread_pool _threadPool;

    
};


#endif