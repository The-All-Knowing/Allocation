#pragma once

#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::ServiceLayer::Views
{
    /// @brief Получает распределённые позиции заказа по идентификатору заказа клиента.
    /// @param orderid Идентификатор заказа клиента.
    /// @param uow Единица работы.
    /// @return Пары: артикул продукции - ссылка на партию заказа.
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, Domain::IUnitOfWork& uow);
}