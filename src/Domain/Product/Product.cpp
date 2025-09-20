#include "Product.hpp"

#include "Domain/Commands/Allocate.hpp"
#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/Deallocated.hpp"
#include "Domain/Events/OutOfStock.hpp"


namespace Allocation::Domain
{
    Product::Product(
        const std::string& sku, const std::vector<Batch>& batches, size_t versionNumber, bool isNew)
        : _sku(sku), _versionNumber(versionNumber), _isModified(isNew)
    {
        for (const auto& batch : batches)
            _referenceByBatches.insert({batch.GetReference(), batch});
    }

    void Product::SetModified(bool modified) noexcept { _isModified = modified; }

    bool Product::IsModified() const noexcept { return _isModified; }

    bool Product::AddBatch(const Batch& batch)
    {
        if (batch.GetSKU() != _sku)
            throw std::invalid_argument("Batch SKU does not match Product SKU");

        if (_referenceByBatches.contains(batch.GetReference()))
            return false;
        auto batchRef = batch.GetReference();
        _referenceByBatches.insert({batchRef, batch});
        _modifiedBatchRefs.insert(batchRef);
        _isModified = true;
        return true;
    }

    bool Product::AddBatches(const std::vector<Batch>& batches)
    {
        for (const auto& batch : batches)
        {
            if (batch.GetSKU() != _sku)
                throw std::invalid_argument("Batch SKU does not match Product SKU");
            if (_referenceByBatches.contains(batch.GetReference()))
                return false;
        }

        for (const auto& batch : batches)
        {
            auto batchRef = batch.GetReference();
            _referenceByBatches.insert({batchRef, batch});
            _modifiedBatchRefs.insert(batchRef);
        }
        _isModified = true;
        return true;
    }

    std::optional<std::string> Product::Allocate(const OrderLine& line)
    {
        if (line.sku != _sku)
            throw std::invalid_argument("OrderLine SKU does not match Product SKU");

        if (_referenceByBatches.empty())
            return std::nullopt;

        std::vector<Batch*> sortedBatches;
        sortedBatches.reserve(_referenceByBatches.size());
        for (auto& [_, batch] : _referenceByBatches)
            sortedBatches.push_back(&batch);

        std::ranges::sort(sortedBatches, [](Batch* lhs, Batch* rhs) { return *lhs < *rhs; });

        for (Batch* batch : sortedBatches)
        {
            if (!batch->CanAllocate(line))
                continue;

            auto batchRef = batch->GetReference();
            batch->Allocate(line);
            _versionNumber++;
            _modifiedBatchRefs.insert(batchRef);
            _isModified = true;
            _messages.push_back(std::make_shared<Events::Allocated>(
                line.reference, line.sku, line.quantity, batchRef));
            return batchRef;
        }

        _messages.push_back(std::make_shared<Events::OutOfStock>(line.sku));
        return std::nullopt;
    }

    bool Product::ChangeBatchQuantity(const std::string& ref, size_t newQty)
    {
        auto it = _referenceByBatches.find(ref);
        if (it == _referenceByBatches.end())
            return false;
        auto& batch = it->second;
        batch.SetPurchasedQuantity(newQty);
        _modifiedBatchRefs.insert(batch.GetReference());
        _isModified = true;

        while (batch.GetAvailableQuantity() < 0)
        {
            auto order = batch.DeallocateOne();
            _messages.push_back(
                std::make_shared<Events::Deallocated>(order.reference, order.sku, order.quantity));
        }
        return true;
    }

    std::vector<Batch> Product::GetBatches() const noexcept
    {
        std::vector<Batch> result;
        for (const auto& [_, batch] : _referenceByBatches)
            result.push_back(batch);

        return result;
    }

    std::optional<Batch> Product::GetBatch(const std::string& batchReference) const noexcept
    {
        if (auto it = _referenceByBatches.find(batchReference); it != _referenceByBatches.end())
            return it->second;

        return std::nullopt;
    }

    std::vector<std::string> Product::GetModifiedBatches() const noexcept
    {
        return std::vector<std::string>(_modifiedBatchRefs.begin(), _modifiedBatchRefs.end());
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

        for (const auto& lhsBatch : lhsBatches)
        {
            auto rhsBatch = rhs.GetBatch(lhsBatch.GetReference());
            if (!rhsBatch.has_value() || lhsBatch != rhsBatch)
                return false;
        }
        return true;
    }

    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept
    {
        if (!lhs && !rhs)
            return true;
        if (!lhs || !rhs)
            return false;
        return *lhs == *rhs;
    }
}