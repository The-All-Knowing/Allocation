#pragma once

#include "Precompile.hpp"

#include "ClientFactory.hpp"
#include "Domain/Events/AbstractEvent.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Redis
{
    template <typename T>
        requires std::derived_from<T, Domain::Events::AbstractEvent>
    class RedisEventPublisher
    {
    public:
        /// @brief Конструктор.
        RedisEventPublisher() : _client(ClientFactory::Instance().Create()) {}

        /// @brief Публикует событие в указанный канал.
        /// @param channel Канал для публикации.
        /// @param event Событие для публикации.
        void operator()(std::string channel, std::shared_ptr<T> event) const
        {
            Poco::JSON::Object json;
            for (auto& [name, value] : GetAttributes<T>(event))
                json.set(name, value);

            std::stringstream ss;
            json.stringify(ss);
            Poco::Redis::Command publish("PUBLISH");
            publish << channel << ss.str();

            try
            {
                _client->execute<Poco::Int64>(publish);
            }
            catch (const Poco::Exception& e)
            {
                Allocation::Loggers::GetLogger()->Error(
                    std::format("Redis publish failed: {}", e.displayText()));
            }
        }

    private:
        mutable Poco::Redis::Client::Ptr _client;
    };
}