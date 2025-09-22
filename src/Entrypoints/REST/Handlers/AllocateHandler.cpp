#include "AllocateHandler.hpp"

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Parsers.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Utilities/Loggers/ILogger.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    void AllocateHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        std::istream& bodyStream = request.stream();
        std::ostringstream body;
        body << bodyStream.rdbuf();
        Poco::JSON::Parser parser;
        auto result = parser.parse(body.str());
        auto json = result.extract<Poco::JSON::Object::Ptr>();

        try
        {
            auto command = Domain::FromJson<Domain::Commands::Allocate>(json);
            ServiceLayer::MessageBus::Instance().Handle(command);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_ACCEPTED);
            response.setContentType("application/json");
            response.send();
            return;
        }
        catch (const Poco::Exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::string msg = ex.displayText();
            response.send() << "{\"message\":\"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
        catch (const std::runtime_error& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_CONFLICT);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            std::string msg = ex.what();
            ostr << "{\"message\": \"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            std::string msg = ex.what();
            ostr << "{\"message\": \"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
        catch (...)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            response.send() << "{\"message\":\" AllocateHandler unknown exception \"}";
            Allocation::Loggers::GetLogger()->Error("AllocateHandler unknown exception");
        }
    }
}