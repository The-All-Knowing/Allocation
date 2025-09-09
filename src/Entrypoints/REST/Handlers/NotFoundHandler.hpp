#pragma once

#include "Precompile.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обработчик запросов для несуществующих маршрутов (404 Not Found).
    class NotFoundHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        void handleRequest(
            Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            response.send() << "404 Not Found";
        }
    };
}