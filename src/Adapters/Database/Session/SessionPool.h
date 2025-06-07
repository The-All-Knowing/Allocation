#pragma once

#include "Precompile.h"
#include "ConnectionConfig.h"


namespace Allocation::Adapters::Database
{

    class SessionPool
    {
    public:
        static SessionPool& Instance();

        bool IsConfigured();
        void Configure(const ConnectionConfig& config);
        void Reconfigure(const ConnectionConfig& config);

        Poco::Data::Session GetSession();

    private:
        SessionPool() = default;
        ~SessionPool() = default;

        SessionPool(const SessionPool&) = delete;
        SessionPool& operator=(const SessionPool&) = delete;

        std::unique_ptr<Poco::Data::SessionPool> _pool;
        std::shared_mutex _mutex;
    };
}
