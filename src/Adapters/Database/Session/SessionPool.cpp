#include "SessionPool.hpp"


namespace Allocation::Adapters::Database
{
    SessionPool& SessionPool::Instance() noexcept
    {
        static SessionPool instance;
        return instance;
    }

    bool SessionPool::IsConfigured() const noexcept
    {
        std::shared_lock lock(_mutex);
        return static_cast<bool>(_pool);
    }

    void SessionPool::Configure(const DatabaseConfig& config)
    {
        std::unique_lock lock(_mutex);
        if (_pool)
            throw std::runtime_error("SessionPool is already configured");

        _pool = std::make_unique<Poco::Data::SessionPool>(config.connector, config.connectionString,
            config.minSessions, config.maxSessions, config.idleTime);
    }

    void SessionPool::Reconfigure(const DatabaseConfig& config)
    {
        std::unique_lock lock(_mutex);
        if (_pool)
            _pool->shutdown();

        _pool = std::make_unique<Poco::Data::SessionPool>(config.connector, config.connectionString,
            config.minSessions, config.maxSessions, config.idleTime);
    }

    void SessionPool::Shutdown()
    {
        _pool->shutdown();
    }

    Poco::Data::Session SessionPool::GetSession()
    {
        std::shared_lock lock(_mutex);
        if (!_pool)
            throw std::runtime_error("SessionPool is not configured");

        return _pool->get();
    }
}