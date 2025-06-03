#include "Handlers/AllocateHandler.h"

#include "Adapters/Repository/SqlRepository.h"
#include "Services/Services.h"
#include "Adapters/Database/Session/SessionPool.h"


namespace Allocation::Infrastructure::Server
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
            auto session = Adapters::Database::SessionPool::Instance().GetSession();
            auto repo = std::make_shared<Adapters::Repository::SqlRepository>(session);

            Domain::OrderLine line(orderid, sku, qty);
            std::string batchRef = Services::Allocate(line, repo, session);

            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            ostr << "{\"batchref\": \"" << batchRef << "\"}";
        }
        catch (const std::exception& ex)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("application/json");
            std::ostream& ostr = response.send();
            std::string msg = ex.what();
            ostr << "{\"message\": \"" << msg << "\"}";
        }
    }
}