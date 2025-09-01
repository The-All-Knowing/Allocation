#pragma once

#include "Precompile.hpp"

#include "Domain/Events/Allocated.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Публикация событий в Redis.
    class RedisEventPublisher
    {
    public:
        /// @brief Конструктор.
        /// @param host Адрес хоста Redis.
        /// @param port Порт Redis.
        RedisEventPublisher(const std::string& host, int port);

        /// @brief Публикует событие в указанный канал.
        /// @param channel Канал для публикации.
        /// @param event Событие для публикации.
        void Publish(const std::string& channel, std::shared_ptr<Domain::Events::Allocated> event);

    private:
        Poco::Redis::Client _client;
    };
}