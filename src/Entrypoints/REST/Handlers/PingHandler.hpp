#pragma once

namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обработчик health-check запроса.
    class PingHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(
            Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override
        {
            response.set("Access-Control-Allow-Origin", "*");
            response.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
            response.send() << "OK";
        }
    };
}