#include "OrderLine.h"


namespace Allocation::Domain
{

    OrderLine::OrderLine( const std::string& reference, const std::string& SKU, size_t quantity):
            _reference(reference), _SKU(SKU), _quantity(quantity) {} 
    
    const std::string& OrderLine::GetOrderReference() const
    {
        return _reference;
    }

    const std::string& OrderLine::GetSKU() const
    {
        return _SKU;
    }

    size_t OrderLine::GetQuantity() const
    {
        return _quantity; 
    }

    bool operator==(const OrderLine& lsh, const OrderLine& rsh)
    {
        return lsh.GetOrderReference() == rsh.GetOrderReference() &&
               lsh.GetQuantity() == rsh.GetQuantity() &&
               lsh.GetSKU() == rsh.GetSKU();
    };

}