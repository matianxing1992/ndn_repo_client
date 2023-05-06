#include "getData.hpp"

NDN_LOG_INIT(ndn_repo_client.getData);

GetDataClient::GetDataClient(ndn::Face &face, ndn::Name repo_name)
    :m_face(face),
    m_repo_name(repo_name)
{
    NDN_LOG_TRACE("Init GetDataClient");
}

void GetDataClient::fetch_object(ndn::Name name_at_repo, FetchedDataCallback onFetchedData)
{
    ndn::util::SegmentFetcher::Options options;

    auto fetcher = ndn::util::SegmentFetcher::start(m_face, ndn::Interest(name_at_repo), 
        ndn::security::v2::getAcceptAllValidator(), options);

    fetcher->onComplete.connect([&](ndn::ConstBufferPtr data){
        onFetchedData(ndn::span<const uint8_t>(data->data(),data->size()));
        NDN_LOG_TRACE("Data Fetched : " << name_at_repo.toUri());
    });

    fetcher->onError.connect([&](auto code,auto msg){
        NDN_LOG_TRACE("Failed to fetch Data : " << name_at_repo.toUri() << " | " << msg);
    });
}
