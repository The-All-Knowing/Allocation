#pragma once

#include "Precompile.hpp"

#include "RedisConfig.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Фабрика клиентов Redis.
    class ClientFactory
    {
    public:
        /// @brief Получает экземпляр фабрики.
        /// @return Ссылка на глобальный объект ClientFactory.
        static ClientFactory& Instance() noexcept;

        /// @brief Проверяет, сконфигурирован ли фабрика подключений к Redis.
        /// @return true, если фабрика уже настроена, иначе false.
        bool IsConfigured() const noexcept;

        /// @brief Настраивает фабрику, указывая параметры подключения к Redis.
        /// @param config Конфигурация Redis.
        void Configure(const RedisConfig& config) noexcept;

        /// @brief Создаёт новый Redis-клиент.
        /// @return Новый Redis-клиент с текущей конфигурацией.
        Poco::Redis::Client::Ptr Create();

    private:
        /// @brief Конструктор.
        ClientFactory() = default;

        /// @brief Копирующий конструктор.
        ClientFactory(const ClientFactory&) = delete;

        /// @brief Перемещающий конструктор.
        ClientFactory(ClientFactory&&) = delete;

        /// @brief Оператор присваивания.
        ClientFactory& operator=(const ClientFactory&) = delete;

        /// @brief Оператор перемещающего присваивания.
        ClientFactory& operator=(ClientFactory&&) = delete;

        Poco::Net::SocketAddress _address;
    };
}