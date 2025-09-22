#pragma once

namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обрабатывает HTTP-запрос для получения CORS-заголовков.
    class OptionsHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        /// @brief Обрабатывает входящий HTTP-запрос и формирует HTTP-ответ.
        /// @param request HTTP-запрос от клиента.
        /// @param response HTTP-ответ, отправляемый клиенту.
        void handleRequest(
            Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override
        {
            response.set("Access-Control-Allow-Origin", "*");
            response.set("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            response.set("Access-Control-Allow-Headers", "Content-Type");
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.send();
        }
    };
}