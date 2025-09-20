#include "RedisConnectionPool.hpp"


namespace Allocation::Adapters::Redis
{
    RedisConnectionPool& RedisConnectionPool::Instance() noexcept
    {
        static RedisConnectionPool instance;
        return instance;
    }

    bool RedisConnectionPool::IsConfigured() const noexcept
    {
        std::shared_lock lock(_mutex);
        return static_cast<bool>(_pocoPool);
    }

    void RedisConnectionPool::Configure(const RedisConfig& config)
    {
        std::unique_lock lock(_mutex);
        if (_pocoPool)
            throw std::runtime_error("RedisConnectionPool is already configured");
        _address = Poco::Net::SocketAddress(config.hostname, config.port);
        _timeoutMilliseconds = config.timeoutMilliseconds;
        _factory =
            Poco::PoolableObjectFactory<Poco::Redis::Client, Poco::Redis::Client::Ptr>{_address};
        _pocoPool =
            std::make_unique<Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>>(
                _factory, config.minConnections, config.maxConnections);
    }

    Poco::Redis::PooledConnection RedisConnectionPool::GetConnection()
    {
        std::shared_lock lock(_mutex);
        if (!_pocoPool)
            throw std::runtime_error("RedisConnectionPool is not configured");

        return Poco::Redis::PooledConnection(*_pocoPool, _timeoutMilliseconds);
    }
}