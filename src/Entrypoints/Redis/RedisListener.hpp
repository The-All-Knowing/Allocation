#pragma once

#include "Precompile.hpp"

#include "Adapters/Redis/ClientFactory.hpp"
#include "Domain/Commands/AbstractCommand.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Redis
{
    /// @brief Слушает сообщения из Redis и перенаправляет их в обработчики.
    class RedisListener
    {
    public:
        /// @brief Создаёт клиента Redis и настраивает асинхронного читателя.
        RedisListener()
            : _client(Adapters::Redis::ClientFactory::Instance().Create()), _reader(*_client)
        {
        }

        /// @brief Освобождает ресурсы, останавливая чтение при необходимости.
        ~RedisListener()
        {
            if (!_reader.isStopped())
                Stop();
        }

        /// @brief Запускает асинхронное чтение сообщений из Redis.
        void Start() { _reader.start(); }

        /// @brief Останавливает асинхронное чтение сообщений из Redis.
        void Stop() { _reader.stop(); };

        /// @brief Подписывается на канал Redis и регистрирует обработчик сообщений.
        /// @tparam Handler Тип функции-обработчика.
        /// @param channel Имя канала Redis.
        /// @param handler Функция-обработчик, вызываемая при получении сообщения.
        template <typename Handler>
        void Subscribe(const std::string& channel, Handler&& handler)
        {
            Poco::Redis::Command subscribe("SUBSCRIBE");
            subscribe.add(channel);
            _client->execute<void>(subscribe);

            _handlers.try_emplace(channel, std::forward<Handler>(handler));
            _reader.redisResponse += Poco::delegate(this, &RedisListener::OnRedisMessage);
        }

    private:
        /// @brief Обрабатывает входящие сообщения от Redis и вызывает обработчики.
        /// @param sender Источник события.
        /// @param args Аргументы события Redis.
        void OnRedisMessage(const void* sender, Poco::Redis::RedisEventArgs& args)
        {
            if (const Poco::Exception* exception = args.exception(); exception)
            {
                Allocation::Loggers::GetLogger()->Error(
                    "Redis exception: " + exception->displayText());
                return;
            }

            try
            {
                if (auto msg = args.message(); msg && msg->isArray())
                {
                    auto arr = dynamic_cast<Poco::Redis::Array*>(msg.get());
                    if (arr && arr->size() == 3 && arr->get<std::string>(0) == "message")
                    {
                        std::string channel = arr->get<std::string>(1);
                        std::string payload = arr->get<std::string>(2);

                        if (auto it = _handlers.find(channel); it != _handlers.end())
                            it->second(payload);
                    }
                }
            }
            catch(const Poco::Exception& e)
            {
                Allocation::Loggers::GetLogger()->Error(
                    "RedisListener exception: " + std::string(e.displayText()));
            }
            catch (const std::exception& e)
            {
                Allocation::Loggers::GetLogger()->Error(
                    "RedisListener exception: " + std::string(e.what()));
            }
        }

        Poco::Redis::Client::Ptr _client;
        Poco::Redis::AsyncReader _reader;

        std::unordered_map<std::string, std::function<void(const std::string&)>> _handlers;
    };

    using RedisListenerPtr = std::shared_ptr<RedisListener>;
}
