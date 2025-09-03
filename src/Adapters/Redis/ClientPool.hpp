#pragma once

#include "Precompile.hpp"


namespace Allocation::Adapters::Redis
{
    class ClientPool
    {
    public:
        /// @brief
        /// @return
        static ClientPool& Instance();

        /// @brief Проверяет, сконфигурирован ли пул сессий.
        /// @return true, если пул сессий сконфигурирован; в противном случае - false.
        bool IsConfigured();

        void Configure(Poco::Net::SocketAddress& address);

        void Reconfigure(Poco::Net::SocketAddress& address);

        Poco::Redis::Client GetClient();

    private:
        ClientPool() = default;
        ~ClientPool() = default;
        ClientPool(const ClientPool&) = delete;
        ClientPool& operator=(const ClientPool&) = delete;

        std::unique_ptr<Poco::PoolableObjectFactory<Poco::Redis::Client, Poco::Redis::Client::Ptr>>
            _factory;
        std::unique_ptr<Poco::Redis::PooledConnection> _pool;
        std::shared_mutex _mutex;
    };
}