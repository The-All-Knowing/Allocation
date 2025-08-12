#include <gtest/gtest.h>

#include "Adapters/Database/Session/ConnectionConfig.hpp"
#include "Precompile.hpp"


namespace Allocation::Tests
{
    class SqlFixture : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        static void SetUpTestSuite();

    private:
        static Adapters::Database::ConnectionConfig LoadDatabaseConfig(const std::string& path);
    };
}