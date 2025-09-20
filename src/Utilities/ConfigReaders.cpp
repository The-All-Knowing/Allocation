#include "ConfigReaders.hpp"


namespace Allocation
{
    Adapters::Database::DatabaseConfig ReadDatabaseConfigurations()
    {
        std::string POSTGRES_HOST = "localhost";
        if (Poco::Environment::has("POSTGRES_HOST"))
            POSTGRES_HOST = Poco::Environment::get("POSTGRES_HOST");
        int POSTGRES_PORT = 5432;
        if (Poco::Environment::has("POSTGRES_PORT"))
            POSTGRES_PORT = std::stoi(Poco::Environment::get("POSTGRES_PORT"));
        std::string POSTGRES_DB = "allocation";
        if (Poco::Environment::has("POSTGRES_DB"))
            POSTGRES_DB = Poco::Environment::get("POSTGRES_DB");
        std::string POSTGRES_USER = "user";
        if (Poco::Environment::has("POSTGRES_USER"))
            POSTGRES_USER = Poco::Environment::get("POSTGRES_USER");
        std::string POSTGRES_PASSWORD = "password";
        if (Poco::Environment::has("POSTGRES_PASSWORD"))
            POSTGRES_PASSWORD = Poco::Environment::get("POSTGRES_PASSWORD");

        std::ostringstream oss;
        oss << "host=" << POSTGRES_HOST << " port=" << POSTGRES_PORT << " dbname=" << POSTGRES_DB
            << " user=" << POSTGRES_USER << " password=" << POSTGRES_PASSWORD;

        Adapters::Database::DatabaseConfig result;
        result.connector = Poco::Data::PostgreSQL::Connector::KEY;
        result.connectionString = oss.str();
        return result;
    }

    Adapters::Redis::RedisConfig ReadRedisConfigurations()
    {
        std::string REDIS_HOST = "localhost";
        if (Poco::Environment::has("REDIS_HOST"))
            REDIS_HOST = Poco::Environment::get("REDIS_HOST");
        int REDIS_PORT = 6379;
        if (Poco::Environment::has("REDIS_PORT"))
            REDIS_PORT = std::stoi(Poco::Environment::get("REDIS_PORT"));

        Adapters::Redis::RedisConfig result;
        result.hostname = REDIS_HOST;
        result.port = REDIS_PORT;
        return result;
    }

    std::pair<Poco::Net::HTTPServerParams*, Poco::UInt16> ReadServerConfigurations()
    {
        Poco::UInt16 ALLOCATION_PORT = 8080;
        if (Poco::Environment::has("ALLOCATION_PORT"))
            ALLOCATION_PORT = std::stoi(Poco::Environment::get("ALLOCATION_PORT"));

        int ALLOCATION_MAX_CONNECTIONS = 100;
        if (Poco::Environment::has("ALLOCATION_MAX_CONNECTIONS"))
            ALLOCATION_MAX_CONNECTIONS =
                std::stoi(Poco::Environment::get("ALLOCATION_MAX_CONNECTIONS"));

        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(16);
        pParams->setMaxKeepAliveRequests(ALLOCATION_MAX_CONNECTIONS);
        return {pParams, ALLOCATION_PORT};
    }

    Adapters::Database::DatabaseConfig LoadDatabaseConfigFromFile(
        Poco::Util::LayeredConfiguration& cfg)
    {
        auto dbHost = cfg.getString("database.host", "localhost");
        auto dbPort = cfg.getInt("database.port", 5432);
        auto dbname = cfg.getString("database.name", "allocation");
        auto user = cfg.getString("database.username", "user");
        auto password = cfg.getString("database.password", "password");

        std::ostringstream oss;
        oss << "host=" << dbHost << " port=" << dbPort << " dbname=" << dbname << " user=" << user
            << " password=" << password;

        Adapters::Database::DatabaseConfig config;
        config.connTimeout = cfg.getInt("database.connection_timeout", 60);
        config.connector = Poco::Data::PostgreSQL::Connector::KEY;
        config.connectionString = oss.str();
        return config;
    }

    Adapters::Redis::RedisConfig LoadRedisConfigFromFile(Poco::Util::LayeredConfiguration& cfg)
    {
        Adapters::Redis::RedisConfig result;
        result.hostname = cfg.getString("redis.host", "localhost");
        result.port = cfg.getInt("redis.port", 6379);
        return result;
    }

    std::pair<Poco::Net::HTTPServerParams*, Poco::UInt16> LoadServerConfigFromFile(
        Poco::Util::LayeredConfiguration& cfg)
    {
        Poco::UInt16 port = cfg.getInt("server.port", 8080);
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(cfg.getInt("server.max_queued", 100));
        pParams->setMaxThreads(cfg.getInt("server.max_threads", 16));
        pParams->setMaxKeepAliveRequests(cfg.getInt("server.max_connections", 100));
        return {pParams, port};
    }
}