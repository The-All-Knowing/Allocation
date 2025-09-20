#pragma once

namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обрабатывает HTTP-запрос на добавление новой партии заказа.
    class AddBatchHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        /// @brief Обрабатывает входящий HTTP-запрос и формирует HTTP-ответ.
        /// @param request HTTP-запрос от клиента.
        /// @param response HTTP-ответ, отправляемый клиенту.
        void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
    };
}