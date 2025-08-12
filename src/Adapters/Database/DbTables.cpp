#include "DbTables.hpp"


namespace Allocation::Adapters::Database
{
    void InitDatabase(Poco::Data::Session& session)
    {
        session.begin();

        session << R"(
            CREATE TABLE IF NOT EXISTS public.order_lines (
            id SERIAL PRIMARY KEY,
            sku VARCHAR(255), 
            qty INTEGER NOT NULL,
            orderid VARCHAR(255))
        )",
            Poco::Data::Keywords::now;

        session << R"(
            CREATE TABLE IF NOT EXISTS public.products (
                sku VARCHAR(255) PRIMARY KEY NOT NULL,
                version_number INTEGER NOT NULL DEFAULT 0
            )
        )",
            Poco::Data::Keywords::now;

        session << R"(
            CREATE TABLE IF NOT EXISTS public.batches (
                id SERIAL PRIMARY KEY,
                reference VARCHAR(255),
                sku VARCHAR(255) NOT NULL, 
                purchased_quantity INTEGER NOT NULL, 
                eta DATE, 
                FOREIGN KEY(sku) REFERENCES public.products(sku))
            )",
            Poco::Data::Keywords::now;

        session << R"(
            CREATE TABLE IF NOT EXISTS public.allocations (
                id SERIAL PRIMARY KEY,
                orderline_id INTEGER NOT NULL, 
                batch_id INTEGER NOT NULL, 
                FOREIGN KEY(orderline_id) REFERENCES public.order_lines(id), 
                FOREIGN KEY(batch_id) REFERENCES public.batches(id))
        )",
            Poco::Data::Keywords::now;

        session << R"(
            CREATE OR REPLACE VIEW public.allocations_view AS
            SELECT 
                o.orderid,
                o.sku,
                b.reference AS batchref
            FROM public.allocations a
            JOIN public.order_lines o ON a.orderline_id = o.id
            JOIN public.batches b ON a.batch_id = b.id
        )",
            Poco::Data::Keywords::now;

        session.commit();
    }

    void DropDatabase(Poco::Data::Session& session)
    {
        session.begin();

        session << "DROP VIEW IF EXISTS public.allocations_view CASCADE", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS public.allocations CASCADE", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS public.batches CASCADE", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS public.order_lines CASCADE", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS public.products CASCADE", Poco::Data::Keywords::now;

        session.commit();
    }
}