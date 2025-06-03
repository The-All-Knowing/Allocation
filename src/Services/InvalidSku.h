#pragma once

#include "Precompile.h"


namespace Allocation::Services
{

    class InvalidSku : public std::exception
    {
    public:
        InvalidSku(std::string_view msg) : msg(msg) 
        {}

        char const* what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };

}