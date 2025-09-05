#pragma once

#include "Precompile.hpp"

#include "Adapters/Database/Session/ConnectionConfig.hpp"


namespace Allocation
{
    Adapters::Database::ConnectionConfig GetSystemConfigs()
    {
        Adapters::Database::ConnectionConfig result;
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
        oss << "host=" << POSTGRES_HOST << " port=" << POSTGRES_HOST << " dbname=" << POSTGRES_DB
            << " user=" << POSTGRES_USER << " password=" << POSTGRES_PASSWORD;

        result.connector = Poco::Data::PostgreSQL::Connector::KEY;
        result.connectionString = oss.str();
        return result;
    }
}