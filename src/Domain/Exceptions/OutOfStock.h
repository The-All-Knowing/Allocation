#pragma once

#include "Precompile.h"


namespace Allocation::Domain::Exceptions
{

    class OutOfStock : public std::exception
    {
    public:
        OutOfStock(std::string_view SKU) : msg(std::format("The article {} is out of stock", SKU)) 
        {}

        char const* what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}