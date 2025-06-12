#pragma once

#include "Precompile.h"
#include "Batch.h"
#include "Events/IEvent.h"


namespace Allocation::Domain
{

    class Product
    {
    public:
        explicit Product(std::string_view SKU,
                        const std::vector<Batch>& batches = {},
                        size_t versionNumber = 0);

        void AddBatch(const Batch& batch) noexcept;
        void AddBatches(const std::vector<Batch>& batches) noexcept;

        [[nodiscard]] std::string Allocate(const OrderLine& line);
        [[nodiscard]] std::vector<Batch> GetBatches() const noexcept;
        [[nodiscard]] size_t GetVersion() const noexcept;
        [[nodiscard]] std::string GetSKU() const noexcept;

        [[nodiscard]] const std::vector<Events::IEventPtr>& Events() const;
        void ClearEvents();

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        std::vector<Events::IEventPtr> _events;
        size_t _versionNumber;
    };
}