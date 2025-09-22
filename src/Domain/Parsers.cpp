#include "Parsers.hpp"

#include "Utilities/Common.hpp"


namespace Allocation::Domain
{
    template <>
    IMessagePtr FromJson<Commands::CreateBatch>(Poco::JSON::Object::Ptr json)
    {
        if (!json)
            throw std::invalid_argument("JSON object is null");

        if (!json->has("ref") || !json->has("sku") || !json->has("qty"))
            throw std::invalid_argument("It doesn't have the required ref or sku or qty fields.");

        auto ref = json->get("ref");
        if (!ref.isString())
            throw std::invalid_argument("Field 'ref' must be a string");
        auto sku = json->get("sku");
        if (!sku.isString())
            throw std::invalid_argument("Field 'sku' must be a string");
        auto qty = json->get("qty");
        if (!qty.isInteger())
            throw std::invalid_argument("Field 'qty' must be a integer");
        std::optional<std::chrono::year_month_day> eta;
        if (json->has("eta") && !json->isNull("eta"))
        {
            auto jsonEta = json->get("eta");
            if (!jsonEta.isString())
                throw std::invalid_argument("Field 'eta' must be a string (ISO 8601 datetime)");

            auto etaStr = jsonEta.convert<std::string>();
            Poco::DateTime dt;
            int tz = 0;
            if (!Poco::DateTimeParser::tryParse(
                    Poco::DateTimeFormat::ISO8601_FORMAT, etaStr, dt, tz))
                throw std::invalid_argument(
                    "Invalid datetime format, expected ISO 8601 (e.g. 2011-01-02T00:00:00)");

            eta = Convert(dt);
        }

        return Make<Commands::CreateBatch>(
            ref.convert<std::string>(), sku.convert<std::string>(), qty.convert<int>(), eta);
    }

    template <>
    IMessagePtr FromJson<Commands::Allocate>(Poco::JSON::Object::Ptr json)
    {
        if (!json)
            throw std::invalid_argument("JSON object is null");

        if (!json->has("orderid") || !json->has("sku") || !json->has("qty"))
            throw std::invalid_argument(
                "It doesn't have the required orderid or sku or qty fields.");

        auto orderid = json->get("orderid");
        if (!orderid.isString())
            throw std::invalid_argument("Field 'orderid' must be a string");
        auto sku = json->get("sku");
        if (!sku.isString())
            throw std::invalid_argument("Field 'sku' must be a string");
        auto qty = json->get("qty");
        if (!qty.isInteger())
            throw std::invalid_argument("Field 'qty' must be a integer");

        return Make<Commands::Allocate>(
            orderid.convert<std::string>(), sku.convert<std::string>(), qty.convert<int>());
    }

    template <>
    IMessagePtr FromJson<Commands::ChangeBatchQuantity>(Poco::JSON::Object::Ptr json)
    {
        if (!json)
            throw std::invalid_argument("JSON object is null");

        if (!json->has("batchref") || !json->has("qty"))
            throw std::invalid_argument("It doesn't have the required batchref or qty fields.");

        auto batchRef = json->get("batchref");
        if (!batchRef.isString())
            throw std::invalid_argument("Field 'batchRef' must be a string");
        auto qty = json->get("qty");
        if (!qty.isInteger())
            throw std::invalid_argument("Field 'qty' must be a integer");

        return Make<Commands::ChangeBatchQuantity>(
            batchRef.convert<std::string>(), qty.convert<int>());
    }
}