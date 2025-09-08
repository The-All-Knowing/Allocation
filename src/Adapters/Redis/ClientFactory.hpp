#pragma once

#include "Precompile.hpp"

#include "RedisConfig.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief 
    class ClientFactory
    {
    public:
        /// @brief
        /// @return
        static ClientFactory& Instance() noexcept;

        /// @brief Проверяет, сконфигурирован ли пул сессий.
        /// @return true, если пул сессий сконфигурирован; в противном случае - false.
        bool IsConfigured() const noexcept;

        /// @brief 
        /// @param address
        void Configure(const RedisConfig& config) noexcept;

        /// @brief 
        /// @return
        Poco::Redis::Client::Ptr Create();

    private:
        ClientFactory() = default;
        ClientFactory(const ClientFactory&) = delete;
        ClientFactory& operator=(const ClientFactory&) = delete;

        Poco::Net::SocketAddress _address;
    };
}