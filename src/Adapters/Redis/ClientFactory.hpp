#pragma once

#include "Precompile.hpp"

#include "RedisConfig.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Фабрика клиентов Redis.
    /// Используется как синглтон для настройки подключения к Redis и
    /// создания объектов клиента Poco::Redis::Client.
    class ClientFactory
    {
    public:
        /// @brief Получает экземпляр фабрики (синглтон).
        /// @return Ссылка на глобальный объект ClientFactory.
        static ClientFactory& Instance() noexcept;

        /// @brief Проверяет, сконфигурирован ли пул подключений к Redis.
        /// @return true, если фабрика уже настроена (адрес установлен), иначе false.
        bool IsConfigured() const noexcept;

        /// @brief Настраивает фабрику, указывая параметры подключения к Redis.
        /// Обычно вызывается один раз при старте сервиса.
        /// @param config Конфигурация Redis (хост, порт и др.).
        void Configure(const RedisConfig& config) noexcept;

        /// @brief Создаёт новый Redis-клиент.
        /// @return Умный указатель на Poco::Redis::Client, связанный с текущей конфигурацией.
        Poco::Redis::Client::Ptr Create();

    private:
        /// @brief Закрытый конструктор для реализации паттерна Singleton.
        ClientFactory() = default;

        /// @brief Копирование запрещено.
        ClientFactory(const ClientFactory&) = delete;
        ClientFactory& operator=(const ClientFactory&) = delete;

        /// @brief Адрес Redis-сервера (IP + порт).
        Poco::Net::SocketAddress _address;
    };
}