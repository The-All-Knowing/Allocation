#pragma once

#include "Precompile.h"
#include "Domain/Product/Product.h"
#include "Domain/Ports/IRepository.h"


namespace Allocation::Adapters::Repository
{

    class FakeRepository final : public Domain::IRepository
    {
    public:
        FakeRepository() = default;
        FakeRepository(const std::vector<std::shared_ptr<Domain::Product>>& init);

        virtual void Add(std::shared_ptr<Domain::Product> product) override;
        virtual [[nodiscard]] std::shared_ptr<Domain::Product> Get(std::string_view SKU) override;

    private:
        std::unordered_map<std::string, std::shared_ptr<Domain::Product>> _skuByProduct;
    };
}