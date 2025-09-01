#pragma once

#include "Precompile.hpp"

#include "ConnectionConfig.hpp"


namespace Allocation::Adapters::Database
{
    /// @brief Пул сессий для работы с базой данных.
    class SessionPool
    {
    public:
        /// @brief Получает экземпляр пула сессий.
        /// @return Ссылка на экземпляр пула сессий.
        static SessionPool& Instance();

        /// @brief Проверяет, сконфигурирован ли пул сессий.
        /// @return true, если пул сессий сконфигурирован; в противном случае - false.
        bool IsConfigured();

        /// @brief Конфигурирует пул сессий.
        /// @param config Конфигурация подключения к базе данных.
        void Configure(const ConnectionConfig& config);

        /// @brief Переконфигурирует пул сессий.
        /// @param config Конфигурация подключения к базе данных.
        void Reconfigure(const ConnectionConfig& config);

        /// @brief Получает сессию из пула.
        /// @return Сессию БД.
        Poco::Data::Session GetSession();

    private:
        SessionPool() = default;
        ~SessionPool() = default;
        SessionPool(const SessionPool&) = delete;
        SessionPool& operator=(const SessionPool&) = delete;

        std::unique_ptr<Poco::Data::SessionPool> _pool;
        std::shared_mutex _mutex;
    };
}
