#pragma once

#include "Precompile.h"


namespace Allocation::Domain
{

    class OrderLine
    {
    public:
        OrderLine(const std::string& reference, const std::string& SKU, size_t quantity);
    
        const std::string& GetOrderReference() const;

        const  std::string& GetSKU() const;

        size_t GetQuantity() const;

    private:
        std::string _reference;
        std::string _SKU;
        size_t _quantity;
    };

    bool operator==(const OrderLine& lsh, const OrderLine& rsh);

}