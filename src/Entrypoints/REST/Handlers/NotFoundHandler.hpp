#pragma once

namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обработчик запросов для несуществующих маршрутов.
    class NotFoundHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(
            Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override
        {
            response.set("Access-Control-Allow-Origin", "*");
            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            response.send() << "404 Not Found";
        }
    };
}