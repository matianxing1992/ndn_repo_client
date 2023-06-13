#include <ndn-repo-client/getData.hpp>

NDN_LOG_INIT(ndn_repo_client.getData);

GetDataClient::GetDataClient(ndn::Face &face, ndn::Name repo_name)
    :m_face(face),
    m_repo_name(repo_name)
{
    NDN_LOG_TRACE("Init GetDataClient");
}

void GetDataClient::fetch_object(ndn::Name name_at_repo, FetchedDataCallback onFetchedData)
{
    
    ndn_repo_client::SegmentFetcher::Options options;
    options.initCwnd=50;

    auto fetcher = ndn_repo_client::SegmentFetcher::start(m_face, ndn::Interest(name_at_repo), 
        ndn::security::v2::getAcceptAllValidator(), options);

    fetcher->onComplete.connect([&,onFetchedData,name_at_repo](ndn::ConstBufferPtr data){
        ndn::util::Sha256 digest;
        ndn::span<const uint8_t> dataSpan(data->data(),data->size());
        digest.update(dataSpan);
        NDN_LOG_TRACE("Data Fetched : " << name_at_repo.toUri()<<" | bytes: "<<data->size()<<" | sha256: "<<digest.toString());
        onFetchedData(dataSpan); 
    });

    fetcher->onError.connect([&,name_at_repo](auto code,auto msg){
        NDN_LOG_TRACE("Failed to fetch Data : " << name_at_repo.toUri() << " | " << msg);
    });
}
