#pragma once

#include "Precompile.hpp"

#include "Domain/Events/AbstractEvent.hpp"
#include "Services/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Redis
{
    template <typename T>
        requires std::derived_from<T, Domain::Events::AbstractEvent>
    class RedisEventPublisher
    {
    public:
        /// @brief Конструктор.
        RedisEventPublisher() {}
        RedisEventPublisher(RedisEventPublisher&&) = default;
        RedisEventPublisher(const RedisEventPublisher&) = default;

        /// @brief Публикует событие в указанный канал.
        /// @param channel Канал для публикации.
        /// @param event Событие для публикации.
        void operator()(std::string channel, std::shared_ptr<T> event) const
        {
            Services::Loggers::GetLogger()->Debug(
                std::format("publishing: channel={}, event={}", channel, event->Name()));

            Poco::JSON::Object json;
            for (auto& [name, value] : GetAttributes<T>(event))
                json.set(name, value);

            std::stringstream ss;
            json.stringify(ss);
            Poco::Redis::Command publish("PUBLISH");
            publish.add(channel);
            publish.add(ss.str());

            try
            {
                _client->execute<void>(publish);
            }
            catch (const Poco::Exception& e)
            {
                Services::Loggers::GetLogger()->Error(
                    std::format("Redis publish failed: {}", e.displayText()));
            }
        }

    private:
        mutable Poco::Redis::Client::Ptr _client;
    };
}