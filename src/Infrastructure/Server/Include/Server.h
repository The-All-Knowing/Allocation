#pragma once

#include "Precompile.h"


namespace Allocation::Infrastructure::Server
{
    class ServerApp : public Poco::Util::ServerApplication
    {
    protected:
        int main(const std::vector<std::string>&) override;
    };
}
