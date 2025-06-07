#include "Batch.h"


namespace Allocation::Domain
{
    Batch::Batch(std::string_view reference, std::string_view SKU, size_t quantity,
        std::optional<std::chrono::year_month_day> ETA):
        _reference(reference), _SKU(SKU),
        _availableQuantity(quantity), _ETA(ETA)
    {}

    bool Batch::CanAllocate(const OrderLine& line) const noexcept
    {
        return line.SKU == _SKU && _availableQuantity >= line.quantity;
    }

    void Batch::Allocate(const OrderLine& line) noexcept
    {
        if (CanAllocate(line) && !_allocations.contains(line))
        {
            _allocations.insert(line);
            _availableQuantity -= line.quantity;

        }
    }

    void Batch::Deallocate(const OrderLine& line) noexcept
    {
        if (_allocations.contains(line))
        {
            _availableQuantity += line.quantity;
            _allocations.erase(line);
        }
    }

    size_t Batch::GetAvailableQuantity() const noexcept
    {
        return _availableQuantity;
    }

    std::string_view Batch::GetReference() const noexcept
    {
        return _reference;
    }

    std::optional<std::chrono::year_month_day> Batch::GetETA() const noexcept
    {
        return _ETA;
    }

    std::string_view Batch::GetSKU() const noexcept
    {
        return _SKU;
    }

    std::vector<OrderLine> Batch::GetAllocations() const noexcept
    {
        return {_allocations.begin(), _allocations.end()};
    }

    auto Batch::operator<=>(const Batch& other) const
    {
        return GetReference() <=> other.GetReference();
    }

}