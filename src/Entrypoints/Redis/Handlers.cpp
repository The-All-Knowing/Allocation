#include "Handlers.hpp"

#include "Domain/Commands/ChangeBatchQuantity.hpp"
#include "Domain/Parsers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Redis::Handlers
{
    void HandleChangeBatchQuantity(const std::string& payload)
    {
        if (payload.empty())
            return;

        Poco::JSON::Parser parser;
        auto parsed = parser.parse(payload);
        auto json = parsed.extract<Poco::JSON::Object::Ptr>();

        auto command = Domain::FromJson<Domain::Commands::ChangeBatchQuantity>(json);
        ServiceLayer::MessageBus::Instance().Handle(command);
    }
}