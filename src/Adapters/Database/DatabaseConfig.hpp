#pragma once

namespace Allocation::Adapters::Database
{
    /// @brief Конфигурация подключения к базе данных.
    struct DatabaseConfig
    {
        /// @brief Тип коннектора (например, "SQLite", "PostgreSQL", "MySQL" и т.д.).
        std::string connector{"PostgreSQL"};
        /// @brief Строка подключения к базе данных.
        std::string connectionString;
        /// @brief Минимальное количество сессий в пуле.
        int minSessions{1};
        /// @brief Максимальное количество сессий в пуле.
        int maxSessions{32};
        /// @brief Время простоя сессии в секундах, после которого она может быть закрыта.
        int idleTime{60};
        /// @brief Таймаут подключения в секундах.
        int connTimeout{60};
    };
}