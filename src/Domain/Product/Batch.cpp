#include "Batch.hpp"


namespace Allocation::Domain
{
    Batch::Batch(const std::string& reference, const std::string& sku, size_t quantity,
        std::optional<std::chrono::year_month_day> eta)
        : _reference(reference), _sku(sku), _purchasedQuantity(quantity), _eta(eta)
    {
    }

    void Batch::SetPurchasedQuantity(size_t newQty) noexcept { _purchasedQuantity = newQty; }

    bool Batch::CanAllocate(const OrderLine& line) const noexcept
    {
        return line.sku == _sku && GetAvailableQuantity() >= line.quantity;
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

        auto it = _allocations.begin();
        OrderLine line = *it;
        _allocations.erase(it);

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

    std::optional<std::chrono::year_month_day> Batch::GetETA() const noexcept { return _eta; }

    std::string Batch::GetSKU() const noexcept { return _sku; }

    std::vector<OrderLine> Batch::GetAllocations() const noexcept
    {
        return {_allocations.begin(), _allocations.end()};
    }

    bool operator==(const Batch& lhs, const Batch& rhs) noexcept
    {
        return lhs.GetReference() == rhs.GetReference();
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