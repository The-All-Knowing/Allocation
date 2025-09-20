#pragma once

#include "Adapters/Database/DatabaseConfig.hpp"
#include "Adapters/Redis/RedisConfig.hpp"


namespace Allocation
{
    /// @brief Читает настройки подключения к СУБД из переменных окружения или устанавливает
    /// значения по умолчанию.
    /// @return Настройки подключения к СУБД.
    Adapters::Database::DatabaseConfig ReadDatabaseConfigurations();

    /// @brief Читает настройки подключения к Redis из переменных окружения или устанавливает
    /// значения по умолчанию.
    /// @return Настройки подключения к Redis.
    Adapters::Redis::RedisConfig ReadRedisConfigurations();

    /// @brief Читает настройки сервера из переменных окружения или устанавливает значения по
    /// умолчанию.
    /// @return Настройки сервера и порт.
    std::pair<Poco::Net::HTTPServerParams*, Poco::UInt16> ReadServerConfigurations();

    /// @brief Загружает параметры подключения к СУБД из файла конфигурации.
    /// @param cfg Инициализированный ридер конфигурации.
    /// @return Конфигурация подключения к СУБД.
    Adapters::Database::DatabaseConfig LoadDatabaseConfigFromFile(Poco::Util::LayeredConfiguration& cfg);

    /// @brief Загружает параметры подключения к Redis из файла конфигурации.
    /// @param cfg Инициализированный ридер конфигурации.
    /// @return Конфигурация подключения к Redis.
    Adapters::Redis::RedisConfig LoadRedisConfigFromFile(Poco::Util::LayeredConfiguration& cfg);

    /// @brief Загружает конфигурацию сервера из файла.
    /// @param cfg Инициализированный ридер конфигурации.
    /// @return Параметры сервера, порт.
    std::pair<Poco::Net::HTTPServerParams*, Poco::UInt16> LoadServerConfigFromFile(
        Poco::Util::LayeredConfiguration& cfg);
}