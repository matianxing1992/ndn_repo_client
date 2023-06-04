#include "ndn-repo-client/utils/directInsertSqlite3.hpp"
#include <chrono>
#include <stdlib.h>


NDN_LOG_INIT(ndn_repo_client.directInsert);

DirectInsertSqlite3Client* DirectInsertSqlite3Client::_client=nullptr;

DirectInsertSqlite3Client::DirectInsertSqlite3Client(ndn::KeyChain& keyChain, const ndn::security::SigningInfo& signingInfo)
    :m_segmenter(keyChain, signingInfo)
{
    int rc;

    // sqlite db of ndn-python-repo
    std::string databaseFile="/.ndn/ndn-python-repo/sqlite3.db";
    std::string homePath=std::getenv("HOME")+databaseFile;
    rc = sqlite3_open(homePath.c_str(), &db);
    if (rc !=SQLITE_OK)
    {
        // failed
        NDN_LOG_INFO(sqlite3_errmsg(db));
    }

}



void DirectInsertSqlite3Client::putData(ndn::Name &name_at_repo, std::shared_ptr<ndn::span<const uint8_t>> object, int segment_size, ndn::time::milliseconds freshness_period)
{
    auto current = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
    uint64_t expireTime = (uint64_t)current.count()+(uint64_t)freshness_period.count();
    auto segments = m_segmenter.segment(*object,name_at_repo,segment_size, freshness_period);
    _putMany(segments,expireTime);
    auto _readHandle = ReadHandle::GetInstance();
    if(_readHandle!=nullptr)
    {
        for(auto data:segments)
            _readHandle->addToCache(*data);
    }

    // for(auto data:segments)
    // {
    //     // remove name's TL as key to support efficient prefix search
    //     ndn::span<const uint8_t> keyBytes=data->getName().wireEncode().value_bytes();
    //     ndn::span<const uint8_t> dataBytes(data->wireEncode().data(),data->wireEncode().size());
    //     _put(keyBytes,dataBytes,expireTime);
        
    // }
}

void DirectInsertSqlite3Client::_putMany(std::vector<std::shared_ptr<ndn::Data>>& data, uint64_t expire_time_ms)
{
    std::string sql="INSERT OR REPLACE INTO data (key, value, expire_time_ms) VALUES ";//(?, ?, ?)
    for(int i=0;i<data.size();i++){
        sql.append("(?, ?, ?)");
        if(i!=data.size()-1)sql.append(",");
    }
    char *zErrMsg = 0;
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db,sql.c_str(),-1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        NDN_LOG_ERROR("prepare failed: " << sqlite3_errmsg(db));
    } else {
        // SQLITE_STATIC because the statement is finalized
        // before the buffer is freed:
        int i=0;
        for(auto d:data){
            // remove name's TL as key to support efficient prefix search
            ndn::span<const uint8_t> keyBytes=d->getName().wireEncode().value_bytes();
            ndn::span<const uint8_t> dataBytes(d->wireEncode().data(),d->wireEncode().size());

            int rc1 = sqlite3_bind_text(stmt, 3*i+1, (const char*)keyBytes.begin(), keyBytes.size(), SQLITE_TRANSIENT);
            int rc2 = sqlite3_bind_blob(stmt, 3*i+2, dataBytes.begin(), dataBytes.size(), SQLITE_TRANSIENT);
            int rc3 = sqlite3_bind_int64(stmt, 3*i+3, expire_time_ms);

            if (rc1 != SQLITE_OK or rc2 != SQLITE_OK or rc3 != SQLITE_OK) {
                NDN_LOG_ERROR( "bind failed: " << sqlite3_errmsg(db) );
                sqlite3_finalize(stmt);
                return;
            } 
            i++;
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE){
            NDN_LOG_ERROR("execution failed: " << sqlite3_errmsg(db));
        }else{
            NDN_LOG_INFO("execution success");
        }
    }
    sqlite3_finalize(stmt);
}

void DirectInsertSqlite3Client::_put(ndn::span<const uint8_t>& key, ndn::span<const uint8_t>& value, uint64_t expire_time_ms)
{
    char *zErrMsg = 0;

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db,
        "INSERT OR REPLACE INTO data (key, value, expire_time_ms) VALUES (?, ?, ?)",
        -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
            NDN_LOG_ERROR("prepare failed: " << sqlite3_errmsg(db));
        } else {
            // SQLITE_STATIC because the statement is finalized
            // before the buffer is freed:
            int rc1 = sqlite3_bind_text(stmt, 1, (const char*)key.begin(), key.size(), SQLITE_TRANSIENT);
            int rc2 = sqlite3_bind_blob(stmt, 2, value.begin(), value.size(), SQLITE_TRANSIENT);
            int rc3 = sqlite3_bind_int64(stmt, 3, expire_time_ms);

            if (rc1 != SQLITE_OK or rc2 != SQLITE_OK or rc3 != SQLITE_OK) {
                NDN_LOG_ERROR( "bind failed: " << sqlite3_errmsg(db) );
            } else {
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE){
                    NDN_LOG_ERROR("execution failed: " << sqlite3_errmsg(db));
                }else{
                    NDN_LOG_INFO("execution success");
                }
                    
            }
    }
    sqlite3_finalize(stmt);


}
