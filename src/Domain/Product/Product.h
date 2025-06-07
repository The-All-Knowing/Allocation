#pragma once

#include "Precompile.h"
#include "Batch.h"


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

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        size_t _versionNumber;
    };
}