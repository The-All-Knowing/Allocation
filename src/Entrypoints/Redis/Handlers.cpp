#include "Handlers.hpp"

#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Services/Loggers/ILogger.hpp"
#include "Services/MessageBus/MessageBus.hpp"


namespace Allocation::Infrastructure::Redis::Handlers
{
    void HandleChangeBatchQuantity(const void* sender, Poco::Redis::RedisEventArgs& args)
    {
        auto message = args.message();
        if (message.isNull())
            return;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed = parser.parse(message->toString());
        Poco::JSON::Object::Ptr json = parsed.extract<Poco::JSON::Object::Ptr>();

        /// @todo добавить валидатор
        if (!json->has("batchref") || !json->has("qty"))
            throw Poco::Exception("JSON missing required fields");

        std::string batchRef = json->getValue<std::string>("batchref");
        int qty = json->getValue<int>("qty");

        Services::MessageBus::Instance().Handle(
            std::make_shared<Domain::Commands::ChangeBatchQuantity>(batchRef, qty));
    }
}