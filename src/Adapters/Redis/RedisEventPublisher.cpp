#include "RedisEventPublisher.hpp"

#include "Services/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Redis
{
    RedisEventPublisher::RedisEventPublisher(const std::string& host, int port)
        : _client(host, port)
    {
    }

    void RedisEventPublisher::Publish(
        const std::string& channel, std::shared_ptr<Domain::Events::Allocated> event)
    {
        Services::Loggers::GetLogger()->Debug(
            std::format("publishing: channel={}, event={}", channel, event->Name()));

        Poco::JSON::Object json;
        json.set("sku", event->SKU);
        json.set("qty", event->qty);
        json.set("batchref", event->batchref);

        std::stringstream ss;
        json.stringify(ss);

        Poco::Redis::Command publish("PUBLISH");
        publish.add(channel);
        publish.add(ss.str());

        try
        {
            _client.execute<void>(publish);
        }
        catch (const Poco::Exception& e)
        {
            Services::Loggers::GetLogger()->Error(
                std::format("Redis publish failed: {}", e.displayText()));
        }
    }
}