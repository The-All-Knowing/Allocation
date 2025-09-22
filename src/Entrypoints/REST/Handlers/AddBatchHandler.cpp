#include "AddBatchHandler.hpp"

#include "Domain/Commands/CreateBatch.hpp"
#include "Domain/Parsers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Utilities/Common.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    void AddBatchHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        response.set("Access-Control-Allow-Origin", "*");

        std::istream& bodyStream = request.stream();
        std::ostringstream body;
        body << bodyStream.rdbuf();
        Poco::JSON::Parser parser;
        auto result = parser.parse(body.str());
        auto json = result.extract<Poco::JSON::Object::Ptr>();

        try
        {
            auto command = Domain::FromJson<Domain::Commands::CreateBatch>(json);
            ServiceLayer::MessageBus::Instance().Handle(command);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
            response.setContentType("application/json");

            std::ostream& out = response.send();
            out << "{\"message\":\"Batch added\"}";
        }
        catch (const Poco::Exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");

            std::ostream& out = response.send();
            out << "{\"message\":\"" << ex.displayText() << "\"}";

            Allocation::Loggers::GetLogger()->Error(ex.displayText());
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");

            std::ostream& out = response.send();
            out << "{\"message\":\"" << ex.what() << "\"}";

            Allocation::Loggers::GetLogger()->Error(ex.what());
        }
        catch (...)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");

            std::ostream& out = response.send();
            out << "{\"message\":\"AddBatchHandler unknown exception\"}";

            Allocation::Loggers::GetLogger()->Error("AddBatchHandler unknown exception");
        }
    }
}