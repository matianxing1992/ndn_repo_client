#include "pubsub.hpp"

NDN_LOG_INIT(ndn_repo_client.pubsub);

template <std::size_t N>
void generateInitVector(uint8_t (&IV_buff)[N])
{
    using bytes_randomizer = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint8_t>;
    std::default_random_engine rd;
    bytes_randomizer bytes(rd);

    std::generate(std::begin(IV_buff), std::end(IV_buff), std::ref(bytes));
}

PubSub::PubSub(ndn::Face &face, ndn::Name prefix, ndn::Name* forwarding_hint, int ims_limit)
    : m_face(face),
    m_publisher_prefix(prefix),
    m_published_data(ims_limit)
{
    NDN_LOG_TRACE("Init pubsub");
    m_forwarding_hint = forwarding_hint;
    m_face.setInterestFilter(m_publisher_prefix,
        [&](const ndn::InterestFilter&, const ndn::Interest& interest){
            auto data=m_published_data.find(interest.getName());
            if(data!=nullptr)
            {
                m_face.put(*data);
                NDN_LOG_TRACE("reply from IMS : " << interest.getName() );
            }
            else
            {
                NDN_LOG_TRACE("Failed to reply from IMS : " << interest.getName() );
            }
        },[&](const ndn::Name& prefix, const std::string& e){
            NDN_LOG_DEBUG("Failed to register the prefix : " << prefix.toUri() << " - " << e);
        });
}

void PubSub::expressNotifyInterest(int n_retries, ndn::Interest notifyInterest, PublishCallback publishCallback)
{
    n_retries-=1;
    if(n_retries>=0)
    {
            m_face.expressInterest(notifyInterest,
        [&](const ndn::Interest&, const ndn::Data&)
        {
            publishCallback(true);
            NDN_LOG_TRACE("publish succeeded : " << notifyInterest.toUri());
        },
        [&](const ndn::Interest&, const ndn::lp::Nack&)
        {
            expressNotifyInterest(n_retries,notifyInterest,publishCallback);
        },
        [&](const ndn::Interest&)
        {
            expressNotifyInterest(n_retries,notifyInterest,publishCallback);
        });

    }
    else
    {
        publishCallback(false);
        NDN_LOG_TRACE("publish failed : " << notifyInterest.toUri());
    }

}

void PubSub::publish(ndn::Name topic, ndn::span<const uint8_t>& msg, PublishCallback publishCallback)
{
    NDN_LOG_TRACE("publishing a message to topic: " << topic.toUri());
    // generate a nonce for each message. Nonce is a random sequence of bytes
    uint8_t nonce[4];
    generateInitVector(nonce);
    // wrap msg in a data packet named /<publisher_prefix>/msg/<topic>/nonce
    ndn::Name dataName(m_publisher_prefix);

    dataName.append("msg").append(topic).append(ndn::name::Component(nonce));
    NDN_LOG_TRACE("Packet Name: " << dataName.toUri());

    auto data = std::make_shared<ndn::Data>(ndn::Data(dataName));
    data->setContentType(ndn::tlv::ContentType_Blob);
    data->setContent(msg);
    m_keyChain.sign(*data);
    data->wireEncode();
    m_published_data.insert(*data,ndn::time::milliseconds(10000));

    // prepare notify interest /<topic>/notify
    ndn::Name notifyInterestName(topic);
    notifyInterestName.append("notify");
    ndn::Interest notifyInterest(notifyInterestName);

    //applicationParameters
    //publisher_prefix type ndn::tlv::Name; notify_nonce type 128
    ndn::Block applicationParameters(ndn::tlv::ApplicationParameters);

    ndn::Block b1(m_publisher_prefix.wireEncode());
    ndn::Block b2 = ndn::encoding::makeBinaryBlock(ndn_repo_client::NOTIFY_NONCE_TYPE,nonce);
    applicationParameters.push_back(b1);
    applicationParameters.push_back(b2);

    if(m_forwarding_hint!=nullptr)
    {
        ndn::Block b3 = ndn::encoding::makeBinaryBlock(ndn_repo_client::PUBLISHER_FWD_HINT_TYPE,m_forwarding_hint->wireEncode());
        applicationParameters.push_back(b3);
    }

    notifyInterest.setApplicationParameters(applicationParameters);
    notifyInterest.setInterestLifetime(ndn::time::milliseconds(3000));

    NDN_LOG_TRACE("NotifyInterest : " << notifyInterest.toUri());
    
    // express notify interest, will retry for n_retries
    int n_retries = 3;
    expressNotifyInterest(n_retries,notifyInterest,publishCallback);
}
