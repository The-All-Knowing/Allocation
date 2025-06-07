#include "UoWFixture.h"
#include "Adapters/Database/Session/SessionPool.h"
#include "Adapters/Database/DbTables.h"


namespace Allocation::Tests
{
    void UoWFixture::SetUp()
    {
        try
        {
            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            Adapters::Database::InitDatabase(session);
        }
        catch (const Poco::Exception& ex)
        {
            FAIL() << "SetUp failed: " << ex.displayText();
        }
        catch (const std::exception& ex)
        {
            FAIL() << "SetUp failed: " << ex.what();
        }
    }

    void UoWFixture::TearDown()
    {
        try
        {
            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            Adapters::Database::DropDatabase(session);
        }
        catch (const Poco::Exception& ex)
        {
            ADD_FAILURE() << "TearDown failed: " << ex.displayText();
        }
        catch (const std::exception& ex)
        {
            ADD_FAILURE() << "TearDown failed: " << ex.what();
        }
    }

    void UoWFixture::SetUpTestSuite()
    {
        Poco::Path exePath(Poco::Path::current());
        Poco::Path iniPath = exePath;
        iniPath.append("Allocation.ini");

        if (!Poco::File(iniPath).exists())
        {
            FAIL() << "INI file 'Allocation.ini' not found in " << exePath.toString();
        }

        try
        {
            auto config = LoadDatabaseConfig(iniPath.toString());

            auto& pool = Adapters::Database::SessionPool::Instance();
            if (!pool.IsConfigured())
                pool.Configure(config);
            else
                pool.Reconfigure(config);
            Poco::Data::PostgreSQL::Connector::registerConnector();
        }
        catch (const Poco::Exception& ex)
        {
            FAIL() << "SetUpTestSuite failed: " << ex.displayText();
        }
        catch (const std::exception& ex)
        {
            FAIL() << "SetUpTestSuite failed: " << ex.what();
        }
    }

    Adapters::Database::ConnectionConfig UoWFixture::LoadDatabaseConfig(const std::string& path)
    {
        Adapters::Database::ConnectionConfig result;
        Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(
            new Poco::Util::IniFileConfiguration(path));

        std::string dbHost = pConf->getString("database.host", "localhost");
        int dbPort = pConf->getInt("database.port", 5432);
        std::string user = pConf->getString("database.username", "postgres");
        std::string password = pConf->getString("database.password", "1");

        std::ostringstream oss;
        oss << "host=" << dbHost
            << " port=" << dbPort
            << " dbname=" << "AllocationTest"
            << " user=" << user
            << " password=" << password;

        result.connector = Poco::Data::PostgreSQL::Connector::KEY;
        result.connectionString = oss.str();

        return result;
    }
}