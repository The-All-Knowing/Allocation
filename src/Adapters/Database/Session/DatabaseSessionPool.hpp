#pragma once

#include "Adapters/Database/DatabaseConfig.hpp"


namespace Allocation::Adapters::Database
{
    /// @brief Потокобезопасный пул сессий подключений к базе данных.
    class DatabaseSessionPool
    {
    public:
        /// @brief Возвращает экземпляр.
        /// @return Экземпляр пула сессий.
        static DatabaseSessionPool& Instance() noexcept;

        /// @brief Проверяет, сконфигурирован ли пул сессий.
        /// @return true - сконфигурирован, false - не сконфигурирован.
        bool IsConfigured() const noexcept;

        /// @brief Конфигурирует или рекогфигурирует пул сессий.
        /// @param config Конфигурация подключения к базе данных.
        void Configure(const DatabaseConfig& config);

        /// @brief Завершает работу сессий подключений к БД.
        void Shutdown();

        /// @brief Возвращает сессию подключения к БД из пула.
        /// @throw Poco::Data::SessionPoolExhaustedException Если пул сессий исчерпан.
        /// @throw std::runtime_error Если пул сессий не сконфигурирован.
        /// @return Сессия подключения к базе данных.
        Poco::Data::Session GetSession();

    private:
        /// @brief Конструктор по умолчанию.
        DatabaseSessionPool() = default;

        /// @brief Деструктор.
        ~DatabaseSessionPool();

        /// @brief Удалённый конструктор копирования.
        DatabaseSessionPool(const DatabaseSessionPool&) = delete;

        /// @brief Удалённый конструктор перемещения.
        DatabaseSessionPool(DatabaseSessionPool&&) = delete;

        /// @brief Удалённый оператор присваивания копированием.
        DatabaseSessionPool& operator=(const DatabaseSessionPool&) = delete;

        /// @brief Удалённый оператор присваивания перемещением.
        DatabaseSessionPool& operator=(DatabaseSessionPool&&) = delete;

        /// @brief Регистрирует коннектор базы данных.
        /// @param connector Тип коннектора (например, "SQLite", "PostgreSQL", "MySQL" и т.д.).
        void RegisterConnector(const std::string& connector);

        /// @brief Удаляет регистрацию коннектора базы данных.
        /// @param connector Тип коннектора (например, "SQLite", "PostgreSQL", "MySQL" и т.д.).
        void UnregisterConnector(const std::string& connector);

        std::unique_ptr<Poco::Data::SessionPool> _pocoPool{nullptr};
        std::string _currentConnector;
        mutable std::shared_mutex _mutex;
    };
}
