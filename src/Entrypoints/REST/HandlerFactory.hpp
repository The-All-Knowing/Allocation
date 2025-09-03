#pragma once

#include "Precompile.hpp"

#include "Domain/Commands/AbstractCommand.hpp"

namespace Allocation::Entrypoints::Rest
{
    /// @brief Фабрика обработчиков REST запросов.
    class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        /// @brief Создаёт обработчик по запросу.
        /// @param request Запрос.
        /// @return Обработчик.
        Poco::Net::HTTPRequestHandler* createRequestHandler(
            const Poco::Net::HTTPServerRequest& request) override;
    };
}