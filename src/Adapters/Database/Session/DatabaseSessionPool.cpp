#include "DatabaseSessionPool.hpp"


namespace Allocation::Adapters::Database
{
    DatabaseSessionPool& DatabaseSessionPool::Instance() noexcept
    {
        static DatabaseSessionPool instance;
        return instance;
    }

    bool DatabaseSessionPool::IsConfigured() const noexcept
    {
        std::shared_lock lock(_mutex);
        return static_cast<bool>(_pocoPool);
    }

    void DatabaseSessionPool::Configure(const DatabaseConfig& config)
    {
        std::unique_lock lock(_mutex);
        if (_pocoPool)
        {
            _pocoPool->shutdown();
            UnregisterConnector(_currentConnector);
        }
        _pocoPool =
            std::make_unique<Poco::Data::SessionPool>(config.connector, config.connectionString,
                config.minSessions, config.maxSessions, config.idleTime, config.connTimeout);
        RegisterConnector(config.connector);
        _currentConnector = config.connector;
    }

    void DatabaseSessionPool::Shutdown()
    {
        std::unique_lock lock(_mutex);
        if (_pocoPool)
        {
            _pocoPool->shutdown();
            _pocoPool.reset();
        }
    }

    Poco::Data::Session DatabaseSessionPool::GetSession()
    {
        std::shared_lock lock(_mutex);
        if (!_pocoPool)
            throw std::runtime_error("DatabaseSessionPool is not configured");

        return _pocoPool->get();
    }

    DatabaseSessionPool::~DatabaseSessionPool() { Shutdown(); }

    void DatabaseSessionPool::RegisterConnector(const std::string& connector)
    {
        if (connector == Poco::Data::PostgreSQL::Connector::KEY)
            Poco::Data::PostgreSQL::Connector::registerConnector();
    }

    void DatabaseSessionPool::UnregisterConnector(const std::string& connector)
    {
        if (connector == Poco::Data::PostgreSQL::Connector::KEY)
            Poco::Data::PostgreSQL::Connector::unregisterConnector();
    }
}