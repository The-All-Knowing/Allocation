#pragma once

namespace Allocation::Adapters::Redis
{
    /// @brief Конфигурация подключения к Redis.
    struct RedisConfig
    {
        /// @brief Имя хоста Redis-сервера.
        std::string hostname;
        /// @brief Порт Redis-сервера.
        Poco::Int16 port;
        /// @brief Минимальное количество подключений в пуле.
        std::size_t minConnections{10};
        /// @brief Максимальное количество подключений в пуле.
        std::size_t maxConnections{50};
        /// @brief Таймаут подключения в миллисекундах.
        long timeoutMilliseconds{0};
    };
}