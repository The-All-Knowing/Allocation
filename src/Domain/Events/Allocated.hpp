#pragma once

#include "AbstractEvent.hpp"


namespace Allocation::Domain::Events
{
    /// @brief Событие "Allocated".
    struct Allocated final : public AbstractEvent
    {
        /// @brief Конструктор события "Allocated".
        /// @param orderid Идентификатор заказа.
        /// @param SKU Артикул товара.
        /// @param batchref Ссылка на партию.
        /// @param qty Количество.
        Allocated(std::string orderid, std::string SKU, size_t qty, std::string batchref)
            : orderid(std::move(orderid)),
              SKU(std::move(SKU)),
              qty(qty),
              batchref(std::move(batchref)){};

        /// @brief Получить имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override { return "Allocated"; };

        std::string orderid;
        std::string SKU;
        size_t qty;
        std::string batchref;
    };

    /// @brief Читает аттрибуты события.
    /// @param event Событие.
    /// @return Атрибуты события: название - значение.
    template <>
    inline std::vector<EventAttribute> GetAttributes<Allocated>(EventPtr event)
    {
        auto p = std::static_pointer_cast<Allocated>(event);
        return {{"orderid", p->orderid}, {"SKU", p->SKU}, {"qty", std::to_string(p->qty)},
            {"batchref", p->batchref}};
    };
}