#pragma once

#include "Precompile.h"
#include "Utilities/OrderLineHash.h"
#include "OrderLine.h" 


namespace Allocation::Domain
{

    class Batch
    {
    public:
        Batch(const std::string& reference, const std::string& SKU,
              size_t quantity, std::optional<std::chrono::year_month_day> ETA = std::nullopt);
        
        bool CanAllocate(const OrderLine& line) const;

        void Allocate(const OrderLine& line);

        void Deallocate(const OrderLine& line);

        size_t GetAvailableQuantity() const;

        std::string GetReference() const;

        std::optional<std::chrono::year_month_day> GetETA() const;

        std::string GetSKU() const;

        std::vector<OrderLine> GetAllocations() const;

    private:
        std::string _reference;
        std::string _SKU;
        size_t _purchasedQuantity{ 0 };
        size_t _availableQuantity;
        std::optional<std::chrono::year_month_day> _ETA;
        std::unordered_set<OrderLine, Utilities::OrderLineHash> _allocations;    
    };

    bool operator==(const Batch& lsh, const Batch& rsh);
    bool operator<(const Batch& lsh, const Batch& rsh);
    bool operator>(const Batch& lsh, const Batch& rsh);

}