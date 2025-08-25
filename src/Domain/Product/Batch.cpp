#include "Batch.hpp"


namespace Allocation::Domain
{
    Batch::Batch(std::string_view reference, std::string_view SKU, size_t quantity,
        std::optional<std::chrono::year_month_day> ETA)
        : _reference(reference), _SKU(SKU), _purchasedQuantity(quantity), _ETA(ETA)
    {
    }

    void Batch::SetPurchasedQuantity(size_t newQty) noexcept { _purchasedQuantity = newQty; }

    bool Batch::CanAllocate(const OrderLine& line) const noexcept
    {
        return line.SKU == _SKU && GetAvailableQuantity() >= line.quantity;
    }

    void Batch::Allocate(const OrderLine& line) noexcept
    {
        if (CanAllocate(line) && !_allocations.contains(line))
            _allocations.insert(line);
    }

    void Batch::Deallocate(const OrderLine& line) noexcept { _allocations.erase(line); }

    int Batch::GetAvailableQuantity() const noexcept
    {
        size_t allocated = 0;
        for (const auto& line : _allocations)
            allocated += line.quantity;
        return _purchasedQuantity - allocated;
    }

    std::string_view Batch::GetReference() const noexcept { return _reference; }
    std::optional<std::chrono::year_month_day> Batch::GetETA() const noexcept { return _ETA; }
    std::string_view Batch::GetSKU() const noexcept { return _SKU; }

    std::vector<OrderLine> Batch::GetAllocations() const noexcept
    {
        return {_allocations.begin(), _allocations.end()};
    }
}