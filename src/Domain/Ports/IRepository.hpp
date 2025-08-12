#pragma once

#include "Domain/Product/Product.hpp"


namespace Allocation::Domain
{
    class IRepository
    {
    public:
        virtual ~IRepository() = default;

        virtual void Add(std::shared_ptr<Product> product) = 0;
        [[nodiscard]] virtual std::shared_ptr<Product> Get(std::string_view SKU) = 0;
        [[nodiscard]] virtual std::shared_ptr<Product> GetByBatchRef(std::string_view ref) = 0;
    };
}