#include "Handlers/AddBatchHandler.h"

#include "Utilities/Common.h"
#include "Services/Services.h"
#include "Adapters/Database/Session/SessionPool.h"
#include "Adapters/Repository/SqlRepository.h"


namespace Allocation::Infrastructure::Server::Handlers
{
    void AddBatchHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
    {
        std::istream& bodyStream = request.stream();
        std::ostringstream body;
        body << bodyStream.rdbuf();

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(body.str());
        Poco::JSON::Object::Ptr json = result.extract<Poco::JSON::Object::Ptr>();
        
        std::string ref = json->getValue<std::string>("ref");
        std::string sku = json->getValue<std::string>("sku");
        int qty = json->getValue<int>("qty");
        std::optional<std::chrono::year_month_day> eta;
        if (json->has("eta") && !json->isNull("eta"))
            eta = Convert(json->getValue<Poco::DateTime>("eta"));

        try
        {
            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            auto repo = std::make_shared<Adapters::Repository::SqlRepository>(session);

            Services::AddBatch(repo, session, ref, sku, qty, eta);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
            response.setContentType("application/json");
            response.send() << "{\"message\":\"Batch added\"}";
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            response.send() << "{\"error\":\"" << ex.what() << "\"}";
        }
    }
}