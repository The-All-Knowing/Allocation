#include "Handlers/AllocateHandler.h"

#include "Services/Loggers/ILogger.h"
#include "Services/UoW/SqlUnitOfWork.h"
#include "Services/MessageBus/MessageBus.h"
#include "Domain/Commands/Allocate.h"


namespace Allocation::Infrastructure::Server::Handlers
{
    void AllocateHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        std::istream& bodyStream = request.stream();
        std::ostringstream body;
        body << bodyStream.rdbuf();

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(body.str());
        Poco::JSON::Object::Ptr json = result.extract<Poco::JSON::Object::Ptr>();

        std::string orderid = json->getValue<std::string>("orderid");
        std::string sku = json->getValue<std::string>("sku");
        int qty = json->getValue<int>("qty");

        try
        {
            auto event = std::make_shared<Domain::Commands::Allocate>(orderid, sku, qty);
            auto result = Services::MessageBus::Instance().Handle(Allocation::Services::UoW::SqlUowFactory, event);
            std::string batchRef = result.back();

            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            ostr << "{\"batchref\": \"" << batchRef << "\"}";
            return;
        }
        catch (const Poco::Exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::string msg = ex.displayText();
            response.send() << "{\"error\":\"" << msg << "\"}";

            Services::Loggers::GetLogger()->Error(msg);
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            std::string msg = ex.what();
            ostr << "{\"message\": \"" << msg << "\"}";

            Services::Loggers::GetLogger()->Error(msg);
        }
    }
}