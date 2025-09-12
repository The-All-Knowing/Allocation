#pragma once

#include "Precompile.hpp"

#include "Domain/Ports/IUnitOfWork.hpp"


namespace Allocation::ServiceLayer::Views
{
    /// @brief Получает список аллокаций по идентификатору заказа.
    /// @param orderid Идентификатор заказа.
    /// @param uow Единица работы.
    /// @return Пары: артикул - ссылка на партию.
    std::vector<std::pair<std::string, std::string>> Allocations(
        std::string orderid, Domain::IUnitOfWork& uow);
}