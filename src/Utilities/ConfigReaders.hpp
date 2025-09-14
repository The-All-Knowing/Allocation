#pragma once

#include "Precompile.hpp"

#include "Adapters/Database/DatabaseConfig.hpp"
#include "Adapters/Redis/RedisConfig.hpp"


namespace Allocation
{
    /// @brief Читает настройки подключения к БД из переменных окружения или устанавливает значения
    /// по умолчанию.
    /// @return Настройки подключения к БД.
    Adapters::Database::DatabaseConfig ReadDatabaseConfigurations();

    /// @brief Читает настройки подключения к Redis из переменных окружения или устанавливает
    /// значения по умолчанию.
    /// @return Настройки подключения к Redis.
    Adapters::Redis::RedisConfig ReadRedisConfigurations();

    /// @brief Читает настройки сервера из переменных окружения или устанавливает значения по
    /// умолчанию.
    /// @return Настройки сервера и порт.
    std::pair<std::unique_ptr<Poco::Net::HTTPServerParams>, Poco::UInt16>
    ReadServerConfigurations();
}