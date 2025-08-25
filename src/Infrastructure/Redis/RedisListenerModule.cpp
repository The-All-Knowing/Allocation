#include "RedisListenerModule.hpp"

#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "RedisConfig.hpp"
#include "Services/Loggers/ILogger.hpp"
#include "Services/MessageBus/MessageBus.hpp"
#include "Services/UoW/SqlUnitOfWork.hpp"


namespace Allocation::Infrastructure::Redis
{
    RedisListenerModule::RedisListenerModule()
        : _client(RedisConfig::FromConfig()->host, RedisConfig::FromConfig()->port)
    {
    }

    void RedisListenerModule::initialize(Poco::Util::Application& app)
    {
        _reader = std::make_unique<Poco::Redis::AsyncReader>(_client);
        _reader->redisResponse += Poco::delegate(this, &RedisListenerModule::onRedisMessage);
        _reader->start();

        subscribe();

        Services::Loggers::GetLogger()->Information("Redis async listener started");
    }

    void RedisListenerModule::uninitialize()
    {
        if (_reader)
        {
            _reader->redisResponse -= Poco::delegate(this, &RedisListenerModule::onRedisMessage);
            _reader->stop();
            _reader.reset();
        }

        Services::Loggers::GetLogger()->Information("Redis async listener stopped");
    }

    void RedisListenerModule::subscribe()
    {
        Poco::Redis::Command subscribe("SUBSCRIBE");
        subscribe.add("change_batch_quantity");
        _client.execute<void>(subscribe);
    }

    void RedisListenerModule::onRedisMessage(const void* sender, Poco::Redis::RedisEventArgs& args)
    {
        if (const Poco::Exception* exception = args.exception(); exception)
        {
            Services::Loggers::GetLogger()->Error("Redis exception: " + exception->displayText());
        }
        else if (auto message = args.message(); message)
        {
            try
            {
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var parsed = parser.parse(message->toString());
                Poco::JSON::Object::Ptr json = parsed.extract<Poco::JSON::Object::Ptr>();

                if (!json->has("batchref") || !json->has("qty"))
                {
                    Services::Loggers::GetLogger()->Error("JSON missing required fields");
                    return;
                }

                std::string batchRef = json->getValue<std::string>("batchref");
                int qty = json->getValue<int>("qty");

                auto command =
                    std::make_shared<Domain::Commands::ChangeBatchQuantity>(batchRef, qty);
                Services::MessageBus::Instance().Handle(
                    Allocation::Services::UoW::SqlUowFactory, command);
            }
            catch (const Poco::Exception& ex)
            {
                Services::Loggers::GetLogger()->Error(
                    "Failed to parse Redis message JSON: " + ex.displayText());
            }
            catch (const std::exception& ex)
            {
                Services::Loggers::GetLogger()->Error(
                    std::string("Standard exception: ") + ex.what());
            }
            catch (...)
            {
                Services::Loggers::GetLogger()->Error(
                    "Unknown exception while processing Redis message");
            }
        }
    }
}