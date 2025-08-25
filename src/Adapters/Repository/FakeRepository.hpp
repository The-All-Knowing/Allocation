#pragma once

#include "Domain/Ports/IRepository.hpp"
#include "Domain/Product/Product.hpp"
#include "Precompile.hpp"


namespace Allocation::Adapters::Repository
{
    class FakeRepository final : public Domain::IRepository
    {
    public:
        FakeRepository() = default;
        FakeRepository(const std::vector<std::shared_ptr<Domain::Product>>& init);

        void Add(std::shared_ptr<Domain::Product> product) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> Get(std::string_view SKU) override;
        [[nodiscard]] std::shared_ptr<Domain::Product> GetByBatchRef(std::string_view ref) override;

    private:
        std::unordered_map<std::string, std::shared_ptr<Domain::Product>> _skuByProduct;
    };
}