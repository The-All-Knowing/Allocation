#include "Session/SessionPool.h"


namespace Allocation::Adapters::Database
{
    SessionPool& SessionPool::Instance()
    {
        static SessionPool instance;
        return instance;
    }

    void SessionPool::Configure(const std::string& connector, const std::string& connectionString)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_pool)
            throw std::runtime_error("SessionPool is already configured");

        _pool = std::make_unique<Poco::Data::SessionPool>(connector, connectionString, 1, 16, 60);
    }

    Poco::Data::Session SessionPool::GetSession()
    {
        if (!_pool)
            throw std::runtime_error("SessionPool is not configured");

        return _pool->get();
    }
}