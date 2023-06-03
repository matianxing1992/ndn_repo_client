#include "ndn-repo-client/directInsertSqlite3.hpp"
#include "ndn-repo-client/getData.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include "ndn-cxx/security/signing-helpers.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

#include <iostream>
#include <chrono>
#include <thread>

NDN_LOG_INIT(examples.directInsert);


int
main(int argc, char* argv[])
{
  try {
    std::string content="";
    for(int i=0;i<1;i++)
    {
      content+="Put data object into the repo. ";
    }
    auto data_ptr=std::make_shared<ndn::Block>(ndn::makeStringBlock(ndn::tlv::Data,content));

    ndn::Face m_face("127.0.0.1");

    auto run=[&](){
      m_face.processEvents(ndn::time::milliseconds(30000),true);
    };

    std::thread thread_run(run);

    ndn::Scheduler m_scheduler{m_face.getIoService()};
    ndn::Name m_client_name("client_name");
    ndn::Name m_repo_name("testrepo");
    ndn::KeyChain m_keyChain;
    auto identity = m_keyChain.createIdentity(ndn::Name("ndn_repo_client"));
    m_keyChain.setDefaultIdentity(identity);
    NDN_LOG_TRACE("Default identity: "<< m_keyChain.getPib().getDefaultIdentity());

    DirectInsertSqlite3Client* m_directInsertClient=DirectInsertSqlite3Client::GetInstance(m_keyChain,ndn::security::SigningInfo());



    
    
    
    ndn::Name name_at_repo=ndn::Name("client_name").append(ndn::Name("Object_"+std::to_string(std::chrono::system_clock::now().time_since_epoch().count())));

    NDN_LOG_TRACE("DirectInsert: Put data object into the database directly");

    auto data_bytes = std::make_shared<ndn::span<const uint8_t>>(data_ptr->value_bytes());
    m_directInsertClient->putData(name_at_repo,data_bytes);
    NDN_LOG_TRACE(name_at_repo);

    GetDataClient m_getDataClient(m_face,m_repo_name);
    m_getDataClient.fetch_object(name_at_repo,[&](auto data){
      ndn::Block _b=ndn::makeBinaryBlock(ndn::tlv::Data,data);
      NDN_LOG_TRACE("Get data from repo: "<< ndn::readString(_b));
    });

    

    // m_face.expressInterest(ndn::Interest(name_at_repo).setCanBePrefix(true),[&](auto,ndn::Data data){
    //   NDN_LOG_TRACE(data);
    //   NDN_LOG_TRACE(ndn::readString(data.getContent()));
    // },NULL,NULL);

    


    NDN_LOG_TRACE("Wait for 10s");
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    
    thread_run.join();

    

    return 0;

  }
  catch (const std::exception& e) {
    NDN_LOG_ERROR(e.what());
    return 1;
  }

}