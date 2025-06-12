#pragma once

#include "Precompile.h"


namespace Allocation::Adapters::Email
{

    void SendMail(std::string_view email, std::string_view maessage);
}