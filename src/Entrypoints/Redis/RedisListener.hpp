#pragma once

#include "Precompile.hpp"

#include "Adapters/Redis/ClientFactory.hpp"
#include "Domain/Commands/AbstractCommand.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Redis
{
    class RedisListener
    {
    public:
        RedisListener()
            : _client(Adapters::Redis::ClientFactory::Instance().Create()), _reader(*_client)
        {
        }

        ~RedisListener()
        {
            if (!_reader.isStopped())
                Stop();
        }

        void Start() { _reader.start(); }

        void Stop() { _reader.stop(); };

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
}