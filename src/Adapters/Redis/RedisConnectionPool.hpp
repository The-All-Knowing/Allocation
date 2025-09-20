#pragma once

#include "RedisConfig.hpp"


namespace Allocation::Adapters::Redis
{
    /// @brief Потокобезопасный пул сессий подключений к Redis.
    class RedisConnectionPool
    {
    public:
        /// @brief Возвращает экземпляр.
        /// @return Экземпляр пула.
        static RedisConnectionPool& Instance() noexcept;

        /// @brief Проверяет, сконфигурирован ли пул подключений к Redis.
        /// @return true - сконфигурирован, false - не сконфигурирован.
        bool IsConfigured() const noexcept;

        /// @brief Конфигурирует или рекогфигурирует пул подключения к Redis.
        /// @param config Конфигурация Redis.
        /// @throw std::runtime_error Если пул уже сконфигурирован.
        void Configure(const RedisConfig& config);

        /// @brief Возвращает подключение к Redis-клиенту.
        /// @return Новый Redis-клиент с текущей конфигурацией.
        /// @throw std::runtime_error Если пул подключений не сконфигурирован.
        /// @throw Poco::InvalidArgumentException Если превышен таймаут ожидания подключения.
        Poco::Redis::PooledConnection GetConnection();

    private:
        /// @brief Конструктор.
        RedisConnectionPool() = default;

        /// @brief Деструктор.
        ~RedisConnectionPool() = default;

        /// @brief Удалённый конструктор копирования.
        RedisConnectionPool(const RedisConnectionPool&) = delete;

        /// @brief Удалённый конструктор перемещения.
        RedisConnectionPool(RedisConnectionPool&&) = delete;

        /// @brief Удалённый оператор присваивания копированием.
        RedisConnectionPool& operator=(const RedisConnectionPool&) = delete;

        /// @brief Удалённый оператор присваивания перемещением.
        RedisConnectionPool& operator=(RedisConnectionPool&&) = delete;

        Poco::Net::SocketAddress _address;
        long _timeoutMilliseconds{0};
        Poco::PoolableObjectFactory<Poco::Redis::Client, Poco::Redis::Client::Ptr> _factory{
            _address};
        std::unique_ptr<Poco::ObjectPool<Poco::Redis::Client, Poco::Redis::Client::Ptr>> _pocoPool{
            nullptr};
        mutable std::shared_mutex _mutex;
    };
}