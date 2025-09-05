#pragma once

#include <gtest/gtest.h>

#include "Adapters/Database/Session/ConnectionConfig.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation::Tests
{

    class InitDatabase : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            auto config = GetSystemConfigs();
            Adapters::Database::SessionPool::Instance().Configure(config);
            Poco::Data::PostgreSQL::Connector::registerConnector();
        }
    };
}