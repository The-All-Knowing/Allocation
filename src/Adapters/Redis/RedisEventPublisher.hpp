#pragma once

#include "Domain/Events/AbstractEvent.hpp"
#include "RedisConnectionPool.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Публикует события в Redis.
    /// @tparam T Тип события.
    template <typename T>
        requires std::derived_from<T, Domain::Events::AbstractEvent>
    class RedisEventPublisher
    {
    public:
        /// @brief Конструктор.
        RedisEventPublisher() : _connection(RedisConnectionPool::Instance().GetConnection()) {}

        /// @brief Публикует событие в указанный канал.
        /// @param channel Канал для публикации.
        /// @param event Событие для публикации.
        void operator()(const std::string& channel, std::shared_ptr<T> event) const
        {
            Poco::JSON::Object json;
            for (const auto& [name, value] : GetAttributes<T>(event))
                json.set(name, value);

            std::stringstream ss;
            json.stringify(ss);
            Poco::Redis::Command publish("PUBLISH");
            publish << channel << ss.str();

            try
            {
                static_cast<Poco::Redis::Client::Ptr>(_connection)->execute<Poco::Int64>(publish);
            }
            catch (const Poco::Exception& e)
            {
                Allocation::Loggers::GetLogger()->Error(
                    std::format("Redis publish failed: {}", e.displayText()));
            }
            catch (...)
            {
                Allocation::Loggers::GetLogger()->Error("Redis publish failed: unknown error");
            }
        }

    private:
        mutable Poco::Redis::PooledConnection _connection;
    };
}