#include "Batch.h"


namespace Allocation::Domain
{
    Batch::Batch(const std::string& reference, const std::string& SKU,
                 size_t quantity, std::chrono::year_month_day ETA):
                 _reference(reference), _SKU(SKU), _availableQuantity(quantity), _ETA(ETA)

    {}

    bool Batch::CanAllocate(const OrderLine& line) const
    {
        return line.GetSKU() == _SKU && _availableQuantity >= line.GetQuantity();
    }

    void Batch::Allocate(const OrderLine& line)
    {
        if (CanAllocate(line) && !_allocations.contains(line))
        {
            _allocations.insert(line);
            _availableQuantity -= line.GetQuantity();
            _purchasedQuantity += line.GetQuantity();

        }
    }

    void Batch::Deallocate(const OrderLine& line)
    {
        if (_allocations.contains(line))
        {
            _availableQuantity += line.GetQuantity();
            _purchasedQuantity -= line.GetQuantity();
            _allocations.erase(line);
        }
    }

    size_t Batch::GetAvailableQuantity() const
    {
        return _availableQuantity;
    }

    std::string Batch::GetReference() const
    {
        return _reference;
    }

    std::chrono::year_month_day Batch::GetETA() const
    {
        return _ETA;
    }

    bool operator<(const Batch& lsh, const Batch& rsh)
    {
        return lsh.GetETA() < rsh.GetETA();
    }

    bool operator>(const Batch& lsh, const Batch& rsh)
    {
        return lsh.GetETA() > rsh.GetETA();
    }

}