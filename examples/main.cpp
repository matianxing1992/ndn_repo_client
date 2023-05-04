#include "ndn-repo-client/pubsub.hpp"
#include "ndn-repo-client/putData.hpp"
#include "ndn-repo-client/getData.hpp"
#include "ndn-repo-client/delete.hpp"
#include "ndn-repo-client/command_checker.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/random.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <iostream>
#include <chrono>
#include <thread>

NDN_LOG_INIT(examples.main);


int
main(int argc, char* argv[])
{
  try {
    ndn::Block data = ndn::makeStringBlock(ndn::tlv::Data,"Put data object into the repo");

    ndn::Face m_face;
    ndn::Scheduler m_scheduler{m_face.getIoService()};
    ndn::Name m_client_name("client_name");
    ndn::Name m_repo_name("testrepo");
    ndn::KeyChain m_keyChain;
    PutDataClient m_putDataClient(m_face,m_client_name,m_repo_name,m_keyChain);

    ndn::Name name_at_repo=m_repo_name.append(ndn::Name("Object_0"));

    NDN_LOG_TRACE("PutDataClient: Put data object into the repo");
    auto request_no = m_putDataClient.insert_object(data.value_bytes(),name_at_repo,ndn::MAX_NDN_PACKET_SIZE>>1,ndn::time::milliseconds(600000),1,
      [&](auto){},nullptr,nullptr,nullptr);

    NDN_LOG_TRACE("Wait for 5s");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    CommandChecker _checker(m_face);
    _checker.check_insert(m_repo_name,request_no,[&](ndn_repo_client::RepoCommandRes res){
      NDN_LOG_TRACE("Check insert status code: "<< res.m_statusCode.m_statusCode);
    });

    NDN_LOG_TRACE("Wait for 1s");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    GetDataClient m_getDataClient(m_face,m_repo_name);
    m_getDataClient.fetch_object(name_at_repo,[&](auto data){
      ndn::Block _b=ndn::makeBinaryBlock(ndn::tlv::Data,data);
      NDN_LOG_TRACE("Get data from repo: "<< ndn::readString(_b));
    });

    NDN_LOG_TRACE("Wait for 1s");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    DeleteClient m_deleteClient(m_face,m_client_name,m_repo_name);
    auto request_no_1 = m_deleteClient.delete_object(name_at_repo,[&](bool){});

    NDN_LOG_TRACE("Wait for 5s");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    _checker.check_delete(m_repo_name,request_no_1,[&](ndn_repo_client::RepoCommandRes res){
      NDN_LOG_TRACE("Check Delete status code: "<< res.m_statusCode.m_statusCode);
    });


  }
  catch (const std::exception& e) {
    NDN_LOG_ERROR(e.what());
  }
}