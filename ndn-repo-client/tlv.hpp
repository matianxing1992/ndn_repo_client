#ifndef NDN_REPO_CLIENT_PUTFILE_HPP
#define NDN_REPO_CLIENT_PUTFILE_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/encoding/block.hpp>
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/sha256.hpp"

namespace ndn_repo_client
{
    // https://ndn-python-repo.readthedocs.io/en/latest/src/specification/encoding.html#status-code-definition
    
    enum type: uint32_t {
        NOTIFY_NONCE_TYPE           = 128,
        START_BLOCK_ID_TYPE         = 204,
        END_BLOCK_ID_TYPE           = 205,
        REQUEST_NO_TYPE             = 206,
        STATUS_CODE_TYPE            = 208,
        INSERT_NUM_TYPE             = 209,
        DELETE_NUM_TYPE             = 210,
        FORWARDING_HINT_TYPE        = 211,
        PUBLISHER_FWD_HINT_TYPE     = 211,
        REGISTER_PREFIX_TYPE        = 212,
        OBJECT_PARAM_TYPE           = 301,
        OBJECT_RESULT_TYPE          = 302,
        REPO_COMMAND_PARAM          = 303,
        REPO_COMMAND_RES            = ndn::tlv::Content,
        REPO_STAT_QUERY             = ndn::tlv::ApplicationParameters,
    };

    enum status: uint32_t {
        ROGER                       = 100,
        COMPLETED                   = 200,
        IN_PROGRESS                 = 300,
        FAILED                      = 400,
        MALFORMED                   = 403,
        NOT_FOUND                   = 404,
    };
   
    // ForwardingHint = FORWARDING-HINT-TYPE TLV-LENGTH Name
    class ForwardingHint
    {
    public:
        ForwardingHint()=default;

        ForwardingHint(ndn::Name name)
            :m_name(name)
        {
           wireEncode();
        }

        ForwardingHint(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::Block(ndn_repo_client::FORWARDING_HINT_TYPE,m_name.wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::FORWARDING_HINT_TYPE) {
                NDN_THROW(ndn::tlv::Error("FORWARDING_HINT_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_name = ndn::Name(m_wire.blockFromValue());
        }

    public:
        ndn::Name m_name;

    private:
        mutable ndn::Block m_wire;
        

    };

    // RegisterPrefix = REGISTER-PREFIX-TYPE TLV-LENGTH Name
    class RegisterPrefix
    {
    public:
        RegisterPrefix()=default;

        RegisterPrefix(ndn::Name name)
            :
             m_name(name)
        {
           wireEncode();
        }

        RegisterPrefix(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::Block(ndn_repo_client::REGISTER_PREFIX_TYPE,m_name.wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REGISTER_PREFIX_TYPE) {
                NDN_THROW(ndn::tlv::Error("REGISTER_PREFIX_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_name = ndn::Name(m_wire.blockFromValue());
        }

    public:
        ndn::Name m_name;

    private:
        mutable ndn::Block m_wire;
        

    };

    class StartBlockId
    {
    public:
        StartBlockId()=default;

        StartBlockId(uint64_t startBlockId)
            :m_startBlockId(startBlockId)
        {
           wireEncode();
        }

        StartBlockId(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeNonNegativeIntegerBlock(ndn_repo_client::START_BLOCK_ID_TYPE,m_startBlockId);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::START_BLOCK_ID_TYPE) {
                NDN_THROW(ndn::tlv::Error("START_BLOCK_ID_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_startBlockId = ndn::readNonNegativeInteger(m_wire);
        }

    public:
        uint64_t m_startBlockId;

    private:
        mutable ndn::Block m_wire;
        

    };

    class EndBlockId
    {
    public:
        EndBlockId()=default;

        EndBlockId(uint64_t endBlockId)
            :m_endBlockId(endBlockId)
        {
           wireEncode();
        }

        EndBlockId(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeNonNegativeIntegerBlock(ndn_repo_client::END_BLOCK_ID_TYPE,m_endBlockId);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::END_BLOCK_ID_TYPE) {
                NDN_THROW(ndn::tlv::Error("END_BLOCK_ID_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_endBlockId = ndn::readNonNegativeInteger(m_wire);
        }

    public:
        uint64_t m_endBlockId;

    private:
        mutable ndn::Block m_wire;
        
    };



    class StatusCode
    {
    public:
        StatusCode()=default;

        StatusCode(uint64_t statusCode)
            :m_statusCode(statusCode)
        {
           wireEncode();
        }

        StatusCode(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeNonNegativeIntegerBlock(ndn_repo_client::STATUS_CODE_TYPE,m_statusCode);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::STATUS_CODE_TYPE) {
                NDN_THROW(ndn::tlv::Error("STATUS_CODE_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_statusCode = ndn::readNonNegativeInteger(m_wire);
        }

    public:
        uint64_t m_statusCode;

    private:
        mutable ndn::Block m_wire;
        
    };

    class InsertNum
    {
    public:
        InsertNum()=default;

        InsertNum(uint64_t insertNum)
            :m_insertNum(insertNum)
        {
           wireEncode();
        }

        InsertNum(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeNonNegativeIntegerBlock(ndn_repo_client::INSERT_NUM_TYPE,m_insertNum);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::INSERT_NUM_TYPE) {
                NDN_THROW(ndn::tlv::Error("INSERT_NUM_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_insertNum = ndn::readNonNegativeInteger(m_wire);
        }

    public:
        uint64_t m_insertNum;

    private:
        mutable ndn::Block m_wire;
        
    };

    class DeleteNum
    {
    public:
        DeleteNum()=default;

        DeleteNum(uint64_t deleteNum)
            :m_deleteNum(deleteNum)
        {
           wireEncode();
        }

        DeleteNum(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeNonNegativeIntegerBlock(ndn_repo_client::DELETE_NUM_TYPE,m_deleteNum);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::DELETE_NUM_TYPE) {
                NDN_THROW(ndn::tlv::Error("DELETE_NUM_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_deleteNum = ndn::readNonNegativeInteger(m_wire);
        }

    public:
        uint64_t m_deleteNum;

    private:
        mutable ndn::Block m_wire;
        
    };

    class RequestNo
    {
    public:
        RequestNo()=default;

        RequestNo(ndn::span<const uint8_t> requestNo)
            :m_requestNo(requestNo)
        {
           wireEncode();
        }

        RequestNo(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire = ndn::makeBinaryBlock(ndn_repo_client::REQUEST_NO_TYPE,m_requestNo);
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REQUEST_NO_TYPE) {
                NDN_THROW(ndn::tlv::Error("REQUEST_NO_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_requestNo = wire.value_bytes();
        }

    public:
        ndn::span<const uint8_t> m_requestNo;

    private:
        mutable ndn::Block m_wire;
        
    };

    class ObjectParam
    {
    public:
        ObjectParam(ndn::Name name,ndn::Name* forwardingHint=nullptr,uint64_t startBlockId=(uint64_t)-1,uint64_t endBlockId=(uint64_t)-1,ndn::Name* registerPrefix=nullptr)
            :m_name(name)
            
            // m_forwardingHint(forwardingHint)
        {
            if(forwardingHint!=nullptr){
                m_forwardingHint = std::make_shared<ForwardingHint>(ForwardingHint(*forwardingHint));
            }else{
                m_forwardingHint=nullptr;
            }
            if(startBlockId!=(uint64_t)-1){
                m_startBlockId = std::make_shared<StartBlockId>(StartBlockId(startBlockId));
            }
            else{
                m_startBlockId=nullptr;
            }
            if(endBlockId!=(uint64_t)-1){
                m_endBlockId = std::make_shared<EndBlockId>(EndBlockId(endBlockId));
            }
            else{
                m_endBlockId=nullptr;
            }
            if(registerPrefix!=nullptr){
                m_registerPrefix = std::make_shared<RegisterPrefix>(RegisterPrefix(*registerPrefix));
            }
            else{
                m_registerPrefix=nullptr;
            }

            wireEncode();
        }

        ObjectParam(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            // m_wire = ndn::makeBinaryBlock(ndn_repo_client::REQUEST_NO_TYPE,m_requestNo);
            m_wire=ndn::Block(ndn_repo_client::OBJECT_PARAM_TYPE);
            m_wire.push_back(m_name.wireEncode());
            if(m_forwardingHint.get()!=nullptr)m_wire.push_back(m_forwardingHint->wireEncode());
            if(m_startBlockId.get()!=nullptr)m_wire.push_back(m_startBlockId->wireEncode());
            if(m_endBlockId.get()!=nullptr)m_wire.push_back(m_endBlockId->wireEncode());
            if(m_registerPrefix.get()!=nullptr)m_wire.push_back(m_registerPrefix->wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REQUEST_NO_TYPE) {
                NDN_THROW(ndn::tlv::Error("REQUEST_NO_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
                if (it->type() == ndn::tlv::Name) {
                    m_name = ndn::Name(*it);
                }
                if (it->type() == ndn_repo_client::FORWARDING_HINT_TYPE) {
                    m_forwardingHint = std::make_shared<ForwardingHint>(ForwardingHint(*it));
                }
                if (it->type() == ndn_repo_client::START_BLOCK_ID_TYPE) {
                    m_startBlockId = std::make_shared<StartBlockId>(StartBlockId(*it));
                }
                if (it->type() == ndn_repo_client::END_BLOCK_ID_TYPE) {
                    m_endBlockId = std::make_shared<EndBlockId>(EndBlockId(*it));
                }
                if (it->type() == ndn_repo_client::REGISTER_PREFIX_TYPE) {
                    m_registerPrefix = std::make_shared<RegisterPrefix>(RegisterPrefix(*it));
                }
            }
            // m_requestNo = wire.value_bytes();
        }

    public:
        ndn::Name m_name;
        std::shared_ptr<ForwardingHint> m_forwardingHint;
        std::shared_ptr<StartBlockId> m_startBlockId;
        std::shared_ptr<EndBlockId> m_endBlockId;
        std::shared_ptr<RegisterPrefix> m_registerPrefix;

    private:
        mutable ndn::Block m_wire;
        
    };
    


    class ObjectResult
    {
    public:
        ObjectResult()=default;

        ObjectResult(ndn::Name name,uint64_t statusCode,uint64_t insertNum=(uint64_t)-1,uint64_t deleteNum=(uint64_t)-1)
            :m_name(name),
            m_statusCode(statusCode)
        {

            if(insertNum!=(uint64_t)-1){
                m_insertNum = std::make_shared<InsertNum>(InsertNum(insertNum));
            }
            else{
                m_insertNum=nullptr;
            }
            if(deleteNum!=(uint64_t)-1){
                m_deleteNum = std::make_shared<DeleteNum>(DeleteNum(deleteNum));
            }
            else{
                m_deleteNum=nullptr;
            }
            wireEncode();
        }

        ObjectResult(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire=ndn::Block(ndn_repo_client::OBJECT_RESULT_TYPE);
            m_wire.push_back(m_name.wireEncode());
            m_wire.push_back(m_statusCode.wireEncode());

            if(m_insertNum.get()!=nullptr)m_wire.push_back(m_insertNum->wireEncode());
            if(m_deleteNum.get()!=nullptr)m_wire.push_back(m_deleteNum->wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::OBJECT_RESULT_TYPE) {
                NDN_THROW(ndn::tlv::Error("REQUEST_NO_TYPE", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
                if (it->type() == ndn::tlv::Name) {
                    m_name = ndn::Name(*it);
                }
                if (it->type() == ndn_repo_client::STATUS_CODE_TYPE) {
                    m_statusCode = StatusCode(*it);
                }
                if (it->type() == ndn_repo_client::INSERT_NUM_TYPE) {
                    m_insertNum = std::make_shared<InsertNum>(InsertNum(*it));
                }
                if (it->type() == ndn_repo_client::DELETE_NUM_TYPE) {
                    m_deleteNum = std::make_shared<DeleteNum>(DeleteNum(*it));
                }
            }
        }

    public:
        ndn::Name m_name;
        StatusCode m_statusCode;
        std::shared_ptr<InsertNum> m_insertNum;
        std::shared_ptr<DeleteNum> m_deleteNum;

    private:
        mutable ndn::Block m_wire;
        
    };
   

    class RepoCommandParam
    {
    public:
        RepoCommandParam()=default;

        RepoCommandParam(std::vector<std::shared_ptr<ndn_repo_client::ObjectParam>>& objectParams)
        {
            m_objectParams = objectParams;
            wireEncode();
        }

        RepoCommandParam(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire=ndn::Block(ndn_repo_client::REPO_COMMAND_PARAM);
            for(auto objectParam : m_objectParams)
                m_wire.push_back(objectParam->wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REPO_COMMAND_PARAM) {
                NDN_THROW(ndn::tlv::Error("REPO_COMMAND_PARAM", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            m_objectParams.clear();
            for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
                if (it->type() == ndn_repo_client::OBJECT_PARAM_TYPE) {
                    m_objectParams.push_back(std::make_shared<ndn_repo_client::ObjectParam>(ndn_repo_client::ObjectParam(*it)));
                }
            }
        }

    public:
        std::vector<std::shared_ptr<ndn_repo_client::ObjectParam>> m_objectParams;

    private:
        mutable ndn::Block m_wire;
        
    };
    
    class RepoCommandRes
    {
    public:
        RepoCommandRes()=default;

        RepoCommandRes(uint64_t statusCode, std::vector<ObjectResult> objectResults)
            :m_statusCode(statusCode)
            
        {
            m_objectResults= std::make_shared<std::vector<ObjectResult>>(objectResults);
            wireEncode();
        }

        RepoCommandRes(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire=ndn::Block(ndn_repo_client::REPO_COMMAND_RES);
            m_wire.push_back(m_statusCode.wireEncode());
            for(auto objectResult:*m_objectResults)
                m_wire.push_back(objectResult.wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REPO_COMMAND_RES) {
                NDN_THROW(ndn::tlv::Error("REPO_COMMAND_RES", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            std::vector<ObjectResult> tmp;
            m_objectResults=std::make_shared<std::vector<ObjectResult>>(tmp);
            for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
                if (it->type() == ndn_repo_client::OBJECT_RESULT_TYPE) {
                    m_objectResults->push_back(ObjectResult(*it));
                }
                if (it->type() == ndn_repo_client::STATUS_CODE_TYPE) {
                    m_statusCode = StatusCode(*it);
                }
            }
        }

    public:
        StatusCode m_statusCode;
        std::shared_ptr<std::vector<ObjectResult>> m_objectResults;

    private:
        mutable ndn::Block m_wire;
        
    };

    class RepoStatQuery
    {
    public:
        RepoStatQuery()=default;

        RepoStatQuery(RequestNo requestNo)
            :m_requestNo(requestNo)
        {
            wireEncode();
        }

        RepoStatQuery(const ndn::Block& block)
        {  
            wireDecode(block);
        }

        const ndn::Block& wireEncode()
        {
            if (m_wire.hasWire()) {
                return m_wire;
            }
            m_wire=ndn::Block(ndn_repo_client::REPO_STAT_QUERY);
            m_wire.push_back(m_requestNo.wireEncode());
            m_wire.encode();
            return m_wire;
        }

        void wireDecode(const ndn::Block& wire)
        {
            if (wire.type() != ndn_repo_client::REPO_STAT_QUERY) {
                NDN_THROW(ndn::tlv::Error("REPO_STAT_QUERY", wire.type()));
            }
            wire.parse();
            m_wire = wire;
            for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
                if (it->type() == ndn_repo_client::REQUEST_NO_TYPE) {
                    m_requestNo = RequestNo(*it);
                }
            }
        }

    public:
        RequestNo m_requestNo;

    private:
        mutable ndn::Block m_wire;
        
    };


}

#endif