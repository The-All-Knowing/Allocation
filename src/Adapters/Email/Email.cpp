#include "Email.hpp"


namespace Allocation::Adapters::Email
{
    void SendMail(std::string_view email, std::string_view maessage)
    {
        std::cout << "SENDING EMAIL:" << email << " " << maessage << std::endl;
    }
}