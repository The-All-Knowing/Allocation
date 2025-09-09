#pragma once

#include "Precompile.hpp"


namespace Allocation::Entrypoints::Rest::Handlers
{
    /// @brief Обрабатывает HTTP-запрос на полчение партии и артикула товара.
    ///
    /// Реализует обработчик REST-эндпоинта, который принимает входящий запрос,
    /// разбирает JSON-данные и загружает данные из модели чтения.
    class AllocationsViewHandler final : public Poco::Net::HTTPRequestHandler
    {
    public:
        /// @brief Обрабатывает входящий HTTP-запрос и формирует HTTP-ответ.
        /// @param request HTTP-запрос от клиента.
        /// @param response HTTP-ответ, отправляемый клиенту.
        void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
    };
}