#include "AddBatchHandler.hpp"

#include "Domain/Commands/CreateBatch.hpp"
#include "Utilities/Loggers/ILogger.hpp"
#include "ServiceLayer/MessageBus/MessageBus.hpp"
#include "ServiceLayer/UoW/SqlUnitOfWork.hpp"
#include "Utilities/Common.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    void AddBatchHandler::handleRequest(
        Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        std::istream& bodyStream = request.stream();
        std::ostringstream body;
        body << bodyStream.rdbuf();
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(body.str());
        auto json = result.extract<Poco::JSON::Object::Ptr>();

        try
        {
            /// @todo: Добавить валидатор.
            std::string ref = json->getValue<std::string>("ref");
            std::string sku = json->getValue<std::string>("sku");
            int qty = json->getValue<int>("qty");
            std::optional<std::chrono::year_month_day> eta;
            if (json->has("eta") && !json->isNull("eta"))
                eta = Convert(json->getValue<Poco::DateTime>("eta"));

            ServiceLayer::MessageBus::Instance().Handle(
                std::make_shared<Domain::Commands::CreateBatch>(ref, sku, qty, eta));

            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
            response.setContentType("application/json");
            response.send() << "{\"message\":\"Batch added\"}";
        }
        catch (const Poco::Exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::string msg = ex.displayText();
            response.send() << "{\"error\":\"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            std::string msg = ex.what();
            response.send() << "{\"error\":\"" << msg << "\"}";

            Allocation::Loggers::GetLogger()->Error(msg);
        }
    }
}