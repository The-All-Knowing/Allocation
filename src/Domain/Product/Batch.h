#pragma once

#include "Precompile.h"
#include "OrderLine.h" 


namespace Allocation::Domain
{

    class Batch
    {
    public:
        explicit Batch(std::string_view reference, std::string_view SKU, size_t quantity,
                std::optional<std::chrono::year_month_day> ETA = std::nullopt);
        
        [[nodiscard]] bool CanAllocate(const OrderLine& line) const noexcept;
        void Allocate(const OrderLine& line) noexcept;
        void Deallocate(const OrderLine& line) noexcept;

        [[nodiscard]] size_t GetAvailableQuantity() const noexcept;
        [[nodiscard]] std::string_view GetReference() const noexcept;
        [[nodiscard]] std::optional<std::chrono::year_month_day> GetETA() const noexcept;
        [[nodiscard]] std::string_view GetSKU() const noexcept;
        [[nodiscard]] std::vector<OrderLine> GetAllocations() const noexcept;

        auto operator<=>(const Batch& other) const;

    private:
        std::string _reference;
        std::string _SKU;
        size_t _availableQuantity;
        std::optional<std::chrono::year_month_day> _ETA;
        std::unordered_set<OrderLine> _allocations;    
    };

    struct BatchETAComparator
    {
        bool operator()(const Batch& lhs, const Batch& rhs) const
        {
            return std::make_tuple(lhs.GetETA(), lhs.GetReference()) < std::make_tuple(rhs.GetETA(), rhs.GetReference());
        }
    };
}