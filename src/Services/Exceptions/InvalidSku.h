#pragma once

#include "Precompile.h"


namespace Allocation::Services::Exceptions
{

    class InvalidSku : public std::exception
    {
    public:
        InvalidSku(std::string_view SKU) : msg(std::format("Invalid sku {}", SKU)) 
        {}

        char const* what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}