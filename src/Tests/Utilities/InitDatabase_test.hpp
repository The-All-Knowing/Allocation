#pragma once

#include <gtest/gtest.h>

#include "Adapters/Database/Session/SessionPool.hpp"
#include "Utilities/ConfigReaders.hpp"


namespace Allocation::Tests
{

    class InitDatabase : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            auto config = ReadSystemDatabaseConfigs();
            Adapters::Database::SessionPool::Instance().Configure(config);
            Poco::Data::PostgreSQL::Connector::registerConnector();
        }

    protected:
        void SetUp() override
        {
            _session = Adapters::Database::SessionPool::Instance().GetSession();
            _session.begin();
        }

        void TearDown() override
        {
            try
            {
                _session.rollback();
            }
            catch (...)
            {
            }
        }

        Poco::Data::Session _session{Adapters::Database::SessionPool::Instance().GetSession()};
    };
}