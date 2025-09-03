#include "ClientPool.hpp"


namespace Allocation::Adapters::Redis
{
    ClientPool& ClientPool::Instance()
    {
        static ClientPool instance;
        return instance;
    }

    bool ClientPool::IsConfigured()
    {
        std::shared_lock lock(_mutex);
        return static_cast<bool>(_pool);
    }

    void ClientPool::Configure(Poco::Net::SocketAddress& address)
    {
        std::unique_lock lock(_mutex);
        if (_pool)
            throw std::runtime_error("ClientPool is already configured");

        _factory = std::make_unique<
            Poco::PoolableObjectFactory<Poco::Redis::Client, Poco::Redis::Client::Ptr>>(address);
        _pool = std::make_unique<Poco::Redis::PooledConnection>(*_factory);
    }

    void ClientPool::Reconfigure(Poco::Net::SocketAddress& address)
    {
        std::unique_lock lock(_mutex);
        if (_pool)
            _pool->shutdown();

        _pool = std::make_unique<Poco::Data::SessionPool>(config.connector, config.connectionString,
            config.minSessions, config.maxSessions, config.idleTime, config.connTimeout);
    }

    Poco::Data::Session ClientPool::GetSession()
    {
        std::shared_lock lock(_mutex);
        if (!_pool)
            throw std::runtime_error("SessionPool is not configured");

        return _pool->get();
    }
}