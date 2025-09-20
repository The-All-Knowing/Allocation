#pragma once

namespace Allocation::Entrypoints::Rest
{
    /// @brief Фабрика обработчиков REST запросов.
    class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        /// @brief Создаёт обработчик по маршруту запроса.
        /// @param request Запрос.
        /// @return Обработчик.
        Poco::Net::HTTPRequestHandler* createRequestHandler(
            const Poco::Net::HTTPServerRequest& request) override;
    };
}