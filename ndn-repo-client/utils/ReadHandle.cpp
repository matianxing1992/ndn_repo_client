#include "ndn-repo-client/utils/ReadHandle.hpp"

NDN_LOG_INIT(ndn_repo_client.ReadHandle);
ReadHandle *ReadHandle::_handle = nullptr;

ReadHandle::ReadHandle(ndn::Face &face,int cpu_cores,int cache_size)
    : m_face(face),
      _threadPool(cpu_cores),
      _scheduler(m_face.getIoService()),
      _cache(cache_size)
{
    int rc;

    // sqlite db of ndn-python-repo
    std::string databaseFile="/.ndn/ndn-python-repo/sqlite3.db";
    std::string homePath=std::getenv("HOME")+databaseFile;
    rc = sqlite3_open(homePath.c_str(), &db);
    if (rc !=SQLITE_OK)
    {
        // failed
        NDN_LOG_ERROR(sqlite3_errmsg(db));
    }
}

void ReadHandle::listen(const ndn::Name& prefix)
{
    m_face.setInterestFilter(prefix,
        std::bind(&ReadHandle::_onInterest,this,_1,_2),
        [&](const ndn::Name& prefix, const std::string& e){
            NDN_LOG_DEBUG("Failed to register the prefix : " << prefix.toUri() << " - " << e);
        });
}

void ReadHandle::addToCache(std::string filename,std::vector<std::shared_ptr<ndn::Data>>& data,int freshness_period)
{   
    // return if alread there

    NDN_LOG_INFO(filename << " added to cache : "<<data.size()<<" segments");
    
    std::unordered_set<ndn::Name> _segments_names;
    {
            std::lock_guard<std::mutex> lock(_cached_files_mutex);
            _cached_files.insert(filename);
    }
    
    for(auto segment:data)
    {
        {
            std::lock_guard<std::mutex> lock(_cache_mutex);
            _segments_names.insert(segment->getName());
            _cache.insert(*segment);
        }

    }
    _scheduler.schedule(ndn::time::milliseconds(freshness_period), 
        [this, filename, _segments_names] { 
            for(auto name:_segments_names)
            {
                {
                    std::lock_guard<std::mutex> lock(_cache_mutex);
                    _cache.erase(name); 
                }
            }
            {
                std::lock_guard<std::mutex> lock(_cached_files_mutex);
                _cached_files.erase(filename);
            }
        });

   
}

void ReadHandle::_onInterest(const ndn::InterestFilter &filter, const ndn::Interest &interest) 
{
    boost::asio::post(_threadPool,[this, interest](){
        if(!replyFromCache(interest)){
            
            loadFromStorage(interest);
            _scheduler.schedule(ndn::time::milliseconds(100), 
                [this, interest] { 
                    if(!replyFromCache(interest)){
                        _scheduler.schedule(ndn::time::milliseconds(1000), 
                            [this, interest] { 
                                replyFromCache(interest);
                            });
                    }
                });

        }
    });

} 

bool ReadHandle::loadFromStorage(const ndn::Interest &interest)
{

    // remove segment from name
    ndn::Name name;
    if(interest.getName().get(-1).isSegment())
    {
        name=interest.getName().getPrefix(-1);
    }else{
        name=interest.getName();
    }
    // return if alread there
    {
        std::lock_guard<std::mutex> lock(_cached_files_mutex);
        if(_cached_files.find(name.toUri())!=_cached_files.end())
        {
            return false;
        }else{
            _cached_files.insert(name.toUri());
        }
    }


    // std::string sql="SELECT value FROM data WHERE hex(key) like '?%';";

    std::string sql="SELECT value FROM data WHERE key like '%' || ? || '%';";

    //remove Type-Length parts and segment component from name
    ndn::span<const uint8_t> keyBytes;
    keyBytes=name.wireEncode().value_bytes();

    sqlite3_stmt* stmt;
	if(sqlite3_prepare(db, sql.c_str(),-1,&stmt,nullptr) == SQLITE_OK){
        int rc = sqlite3_bind_blob(stmt, 1, keyBytes.begin(), keyBytes.size(), SQLITE_TRANSIENT);
        // int rc = sqlite3_bind_text(stmt, 1, "%", -1, 0);

        // NDN_LOG_INFO(sqlite3_expanded_sql(stmt));

        if (rc != SQLITE_OK) {
            NDN_LOG_ERROR( "bind failed: " << sqlite3_errmsg(db) );
            sqlite3_finalize(stmt);
            return false;
        }else{
            		
            int rv;
            std::vector<std::shared_ptr<ndn::Data>> segments;
            while((rv = sqlite3_step(stmt)) == SQLITE_ROW){
                auto data = (const uint8_t*)sqlite3_column_blob(stmt, 0);
                int size = sqlite3_column_bytes(stmt, 0);
                ndn::Data segment;
                ndn::Block _b(ndn::span<const uint8_t>(data,size));
                segment.wireDecode(_b);
                segments.push_back(std::make_shared<ndn::Data>(segment)); 
            }
            NDN_LOG_INFO("Loaded rows: "<<segments.size());
            if(segments.size()>0)
            {
                addToCache(name.toUri(),segments);
            }

            if(rv != SQLITE_DONE){
                NDN_LOG_ERROR("Failed to get data from the database");
                sqlite3_finalize(stmt);
                return false;
            }

        } 


		sqlite3_finalize(stmt);
		return true;
	}
	else{
		NDN_LOG_ERROR("SQLITE3 ERROR" << sqlite3_errmsg(db) );
	}
    sqlite3_finalize(stmt);
	return false;
}

bool ReadHandle::replyFromCache(const ndn::Interest &interest)
{
    
    std::lock_guard<std::mutex> lock(_cache_mutex);
    auto data=_cache.find(interest.getName());
    if(data!=nullptr){
        m_face.put(*data);
    }
    return false;
}
