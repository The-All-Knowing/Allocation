#include "Product.hpp"

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/Deallocated.hpp"
#include "Domain/Events/OutOfStock.hpp"


namespace Allocation::Domain
{
    Product::Product(std::string_view SKU, const std::vector<Batch>& batches, size_t versionNumber)
        : _sku(SKU), _versionNumber(versionNumber)
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

    std::optional<std::string> Product::Allocate(const OrderLine& line)
    {
        std::vector<std::reference_wrapper<Batch>> sortedBatches;
        sortedBatches.reserve(_referenceByBatches.size());
        for (auto& [_, batch] : _referenceByBatches)
            sortedBatches.emplace_back(batch);
        std::ranges::sort(
            sortedBatches, [](const auto& lhs, const auto& rhs) { return lhs.get() < rhs.get(); });

        for (Batch& batch :
            sortedBatches | std::views::transform([](auto& b) -> Batch& { return b.get(); }))
        {
            if (!batch.CanAllocate(line))
                continue;

            batch.Allocate(line);
            _versionNumber++;
            _messages.push_back(std::make_shared<Events::Allocated>(
                line.reference, line.SKU, line.quantity, std::string(batch.GetReference())));
            return std::string(batch.GetReference());
        }

        _messages.push_back(std::make_shared<Events::OutOfStock>(line.SKU));
        return std::nullopt;
    }

    void Product::ChangeBatchQuantity(std::string_view ref, size_t newQty)
    {
        auto it = _referenceByBatches.find(std::string(ref));
        if (it == _referenceByBatches.end())
            return;
        auto& batch = it->second;
        batch.SetPurchasedQuantity(newQty);

        while (batch.GetAvailableQuantity() < 0)
        {
            auto order = batch.DeallocateOne();
            _messages.push_back(
                std::make_shared<Events::Deallocated>(order.reference, order.SKU, order.quantity));
        }
    }

    std::vector<Batch> Product::GetBatches() const noexcept
    {
        std::vector<Batch> result;
        for (auto& [_, batch] : _referenceByBatches)
            result.push_back(batch);

        return result;
    }

    std::optional<Batch> Product::GetBatch(const std::string& reference) const noexcept
    {
        if (auto it = _referenceByBatches.find(reference); it != _referenceByBatches.end())
            return it->second;

        return std::nullopt;
    }

    size_t Product::GetVersion() const noexcept { return _versionNumber; }

    std::string Product::GetSKU() const noexcept { return _sku; }

    const std::vector<Domain::IMessagePtr>& Product::Messages() const noexcept { return _messages; }

    void Product::ClearMessages() noexcept { _messages.clear(); }

    bool operator==(const Product& lhs, const Product& rhs) noexcept
    {
        if (lhs.GetSKU() != rhs.GetSKU() || lhs.GetVersion() != rhs.GetVersion())
            return false;
        auto lhsBatches = lhs.GetBatches();
        auto rhsBatches = rhs.GetBatches();
        if (lhsBatches.size() != rhsBatches.size())
            return false;

        std::sort(lhsBatches.begin(), lhsBatches.end());
        std::sort(rhsBatches.begin(), rhsBatches.end());
        return std::equal(lhsBatches.begin(), lhsBatches.end(), rhsBatches.begin());
    }

    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept
    {
        if (!lhs || !rhs)
            return false;

        return *lhs == *rhs; 
    }
}