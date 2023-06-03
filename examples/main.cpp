#include "ndn-repo-client/pubsub.hpp"
#include "ndn-repo-client/putData.hpp"
#include "ndn-repo-client/getData.hpp"
#include "ndn-repo-client/delete.hpp"
#include "ndn-repo-client/command_checker.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include "ndn-cxx/security/signing-helpers.hpp"

#include <iostream>
#include <chrono>
#include <thread>

NDN_LOG_INIT(examples.main);


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


    ndn::Scheduler m_scheduler{m_face.getIoService()};
    ndn::Name m_client_name("client_name");
    ndn::Name m_repo_name("testrepo");
    ndn::KeyChain m_keyChain;
    auto identity = m_keyChain.createIdentity(ndn::Name("ndn_repo_client"));
    m_keyChain.setDefaultIdentity(identity);
    NDN_LOG_TRACE("Default identity: "<< m_keyChain.getPib().getDefaultIdentity());

    PutDataClient m_putDataClient(m_face,m_client_name,m_repo_name,m_keyChain,ndn::security::SigningInfo());

    auto run=[&](){
      // m_face.registerPrefix(m_client_name,[&](auto){},[&](auto,auto){});
      // m_face.processEvents(boost::chrono::duration<int_least64_t, boost::milli>::zero(),true);
      m_face.processEvents();
    };

    
    
    ndn::Name name_at_repo=ndn::Name("client_name").append(ndn::Name("Object_"+std::to_string(std::chrono::system_clock::now().time_since_epoch().count())));

    NDN_LOG_TRACE("PutDataClient: Put data object into the repo");
    auto data_bytes = std::make_shared<ndn::span<const uint8_t>>(data_ptr->value_bytes());

    ndn::span<const uint8_t> request_no;
    m_putDataClient.insert_object(data_bytes,name_at_repo,8000,ndn::time::milliseconds(600000),1,
      [&](auto isComplete){
        if(isComplete){
          NDN_LOG_TRACE("Insertion Done");
        }else{
          NDN_LOG_TRACE("Insertion Failed or Check Interest Nacked/Timeout");
        }
      },nullptr,m_repo_name,m_client_name,request_no);

    std::thread thread_run(run);

    NDN_LOG_TRACE("Wait for 5s");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    GetDataClient m_getDataClient(m_face,m_repo_name);
    m_getDataClient.fetch_object(name_at_repo,[&](auto data){
      ndn::Block _b=ndn::makeBinaryBlock(ndn::tlv::Data,data);
      NDN_LOG_TRACE("Get data from repo: "<< ndn::readString(_b));
    });

    // NDN_LOG_TRACE("Wait for 1s");
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // DeleteClient m_deleteClient(m_face,m_client_name,m_repo_name);
    // ndn::span<const uint8_t> request_no_1;
    // m_deleteClient.delete_object(request_no_1,name_at_repo,[&](bool){},m_repo_name,m_client_name,(uint64_t)0,(uint64_t)-1);

    
    // NDN_LOG_TRACE("Wait for 1s");
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // CommandChecker _checker(m_face);
    // _checker.check_insert(m_repo_name,request_no,[&](uint64_t res){
    //   NDN_LOG_TRACE("Check insert status code: "<< res);
    // });
    
    // NDN_LOG_TRACE("Wait for 1s");
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // _checker.check_delete(m_repo_name,request_no_1,[&](uint64_t res){
    //   NDN_LOG_TRACE("Check Delete status code: "<< res);
    // });

    NDN_LOG_TRACE("Wait for 5s");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    thread_run.join();

    

    return 0;

  }
  catch (const std::exception& e) {
    NDN_LOG_ERROR(e.what());
    return 1;
  }

}