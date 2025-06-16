#include "Product.h"

#include "Events/OutOfStock.h"
#include "Events/Allocated.h"
#include "Commands/Allocate.h"


namespace Allocation::Domain
{

    Product::Product(std::string_view SKU, const std::vector<Batch>& batches, size_t versionNumber):
        _sku(SKU),  _versionNumber(versionNumber)
    {
        for (auto& batch : batches)
            AddBatch(batch); 
    }

    void Product::AddBatch(const Batch& batch) noexcept
    {
        _referenceByBatches.insert_or_assign(std::string(batch.GetReference()), batch);
    }

    void Product::AddBatches(const std::vector<Batch>& batches) noexcept
    {
        for (auto& batch : batches)
            AddBatch(batch);
    }

    std::string Product::Allocate(const OrderLine& line)
    {
        std::vector<std::reference_wrapper<Batch>> sortedBatches;
        sortedBatches.reserve(_referenceByBatches.size());

        for (auto& [_, batch] : _referenceByBatches)
            sortedBatches.emplace_back(batch);

        std::ranges::sort(sortedBatches, BatchETAComparator{});

        for (Batch& batch : sortedBatches | std::views::transform([](auto& b) -> Batch& { return b.get(); }))
        {
            if (batch.CanAllocate(line))
            {
                batch.Allocate(line);
                _versionNumber++;
                _messages.push_back(std::make_shared<Events::Allocated>(
                    line.SKU, line.reference , line.quantity));
                return std::string(batch.GetReference());
            }
        }

        _messages.push_back(std::make_shared<Events::OutOfStock>(line.SKU));
        return "";
    }

    void Product::ChangeBatchQuantity(std::string_view ref, size_t newQty)
    {
        auto it = _referenceByBatches.find(std::string(ref));
        if (it == _referenceByBatches.end()) return;

        auto& batch = it->second;
        batch.SetPurchasedQuantity(newQty);

        for (const auto& order : batch.GetAllocations())
        {
            if (batch.GetAvailableQuantity() >= 0)
                break;

            batch.Deallocate(order);
            _messages.push_back(std::make_shared<Commands::Allocate>(
                order.reference, order.SKU, order.quantity));
        }
    }

    std::vector<Batch> Product::GetBatches() const noexcept
    {
        std::vector<Batch> result;
        for (auto& [_, batch] : _referenceByBatches)
            result.push_back(batch);

        return result;
    }
    
    size_t Product::GetVersion() const noexcept
    {
        return _versionNumber;
    }

    std::string Product::GetSKU() const noexcept
    {
        return _sku;
    }

    const std::vector<Domain::IMessagePtr>& Product::Messages() const
    {
        return _messages;
    }
    
    void Product::ClearMessages()
    {
        _messages.clear();
    }

    bool operator==(const Product& lhs, const Product& rhs) noexcept
    {
        if (lhs.GetSKU() != rhs.GetSKU() || lhs.GetVersion() != rhs.GetVersion())
            return false;

        auto lhsBatches = lhs.GetBatches();
        auto rhsBatches = rhs.GetBatches();

        if (lhsBatches.size() != rhsBatches.size())
            return false;

        std::sort(lhsBatches.begin(), lhsBatches.end(), BatchETAComparator());
        std::sort(rhsBatches.begin(), rhsBatches.end(), BatchETAComparator());

        return std::equal(lhsBatches.begin(), lhsBatches.end(), rhsBatches.begin());
    }
}