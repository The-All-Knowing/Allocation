#include "Batch.hpp"


namespace Allocation::Domain
{
    Batch::Batch(const std::string& reference, const std::string& SKU, size_t quantity,
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

    OrderLine Batch::DeallocateOne()
    {
        if (_allocations.empty())
            throw std::runtime_error("No allocations to deallocate");

        auto line = *_allocations.rbegin();
        _allocations.erase(--_allocations.end());
        return line;
    }

    int Batch::GetAllocatedQuantity() const noexcept
    {
        size_t allocated = 0;
        for (const auto& line : _allocations)
            allocated += line.quantity;
        return allocated;
    }

    int Batch::GetAvailableQuantity() const noexcept
    {
        size_t allocated = 0;
        for (const auto& line : _allocations)
            allocated += line.quantity;
        return _purchasedQuantity - allocated;
    }

    int Batch::GetPurchasedQuantity() const noexcept { return _purchasedQuantity; }

    std::string Batch::GetReference() const noexcept { return _reference; }

    std::optional<std::chrono::year_month_day> Batch::GetETA() const noexcept { return _ETA; }

    std::string Batch::GetSKU() const noexcept { return _SKU; }

    std::vector<OrderLine> Batch::GetAllocations() const noexcept
    {
        return {_allocations.begin(), _allocations.end()};
    }

    bool operator<(const Batch& lhs, const Batch& rhs) noexcept
    {
        if (!lhs.GetETA().has_value())
            return true;
        if (!rhs.GetETA().has_value())
            return false;
        return lhs.GetETA().value() < rhs.GetETA().value();
    }
}