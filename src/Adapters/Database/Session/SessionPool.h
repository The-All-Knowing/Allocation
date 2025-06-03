#pragma once

#include "Precompile.h"


namespace Allocation::Adapters::Database
{

    class SessionPool
    {
    public:
        static SessionPool& Instance();

        void Configure(const std::string& connector, const std::string& connectionString);

        Poco::Data::Session GetSession();

    private:
        SessionPool() = default;
        ~SessionPool() = default;

        SessionPool(const SessionPool&) = delete;
        SessionPool& operator=(const SessionPool&) = delete;

        std::unique_ptr<Poco::Data::SessionPool> _pool;
        std::mutex _mutex;
    };

}
