#include "ndn-repo-client/utils/ReadHandle.hpp"
#include "ReadHandle.hpp"

NDN_LOG_INIT(ndn_repo_client.ReadHandle);
ReadHandle *ReadHandle::_handle = nullptr;

ReadHandle::ReadHandle(ndn::Face &face)
    : m_face(face),
      _cache(8192),
      _threadPool(4),
      _scheduler(m_face.getIoService())
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

void ReadHandle::listen(const ndn::Name& prefix)
{
    m_face.setInterestFilter(prefix,
        std::bind(&ReadHandle::_onInterest,this,_1,_2),
        [&](const ndn::Name& prefix, const std::string& e){
            NDN_LOG_DEBUG("Failed to register the prefix : " << prefix.toUri() << " - " << e);
        });
}

void ReadHandle::addToCache(const ndn::Data &data,int freshness_period)
{
    _cache.insert(data);
    _scheduler.schedule(ndn::time::milliseconds(freshness_period), [this, name = data.getName()] { _cache.erase(name); });
}

void ReadHandle::_onInterest(const ndn::InterestFilter &filter, const ndn::Interest &interest) 
{
    boost::asio::post(_threadPool,[this, interest](){
        if(!replyFromCache(interest)){
            loadFromStorage(interest);
            replyFromCache(interest);
        }
    });

} 

bool ReadHandle::loadFromStorage(const ndn::Interest &interest)
{

    return false;
}

bool ReadHandle::replyFromCache(const ndn::Interest &interest)
{
    auto data=_cache.find(interest.getName());
    if(data!=nullptr){
        m_face.put(*data);
    }
    return false;
}
