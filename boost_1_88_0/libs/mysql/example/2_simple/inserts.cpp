//
// Copyright (c) 2019-2025 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/awaitable.hpp>
#ifdef BOOST_ASIO_HAS_CO_AWAIT

//[example_inserts

/**
 * This example demonstrates how to use INSERT statements,
 * the results::last_insert_id() function, and optionals
 * to represent potentially NULL values.
 *
 * The program inserts an employee, given their first name,
 * last name and company ID. It then prints the ID of the newly
 * inserted employee.
 *
 * It uses C++20 coroutines. If you need, you can backport
 * it to C++11 by using callbacks, asio::yield_context
 * or sync functions instead of coroutines.
 *
 * This example uses the 'boost_mysql_examples' database, which you
 * can get by running db_setup.sql.
 */

#include <boost/mysql/any_connection.hpp>
#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/with_params.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace mysql = boost::mysql;
namespace asio = boost::asio;

// The main coroutine
asio::awaitable<void> coro_main(
    std::string_view server_hostname,
    std::string_view username,
    std::string_view password,
    std::string_view first_name,
    std::string_view last_name,
    std::string_view company_id,
    std::optional<std::uint32_t> salary  // empty optional means that a NULL value should be inserted
)
{
    // Create a connection.
    // Will use the same executor as the coroutine.
    mysql::any_connection conn(co_await asio::this_coro::executor);

    // The server host, username, password and database to use.
    mysql::connect_params params;
    params.server_address.emplace_host_and_port(std::string(server_hostname));
    params.username = std::move(username);
    params.password = std::move(password);
    params.database = "boost_mysql_examples";

    // Connect to the server
    co_await conn.async_connect(params);

    // Perform the insertion.
    // If salary is empty, the last {} will be replaced by NULL.
    mysql::results result;
    co_await conn.async_execute(
        mysql::with_params(
            "INSERT INTO employee (first_name, last_name, company_id, salary) VALUES ({}, {}, {}, {})",
            first_name,
            last_name,
            company_id,
            salary
        ),
        result
    );

    // results::last_insert_id retrieves the value of the latest
    // AUTO_INCREMENT field generated by the executed query, if any.
    // In this case, this is the generated employee_id.
    // If we needed the entire generated employee, we'd need a transaction
    // and multi-queries.
    std::cout << "Successfully created employee with ID: " << result.last_insert_id() << std::endl;

    // Notify the MySQL server we want to quit, then close the underlying connection.
    co_await conn.async_close();
}

void main_impl(int argc, char** argv)
{
    if (argc < 7 || argc > 8)
    {
        std::cerr
            << "Usage: " << argv[0]
            << " <username> <password> <server-hostname> <first-name> <last-name> <company-id> [<salary>]\n";
        exit(1);
    }

    // In DB, salary is an UNSIGNED INT (32-bit) representing employee salary in USD
    // It may be NULL (e.g. for contractors).
    // Parse the command line argument, if present, and validate it's within a sane range
    std::optional<std::uint32_t> salary;
    if (argc == 8)
    {
        int parsed_salary = std::stoi(argv[7]);
        if (parsed_salary < 10000 || parsed_salary >= 1000000)
        {
            std::cerr << "Salary should be between 10000 and 1000000\n";
            exit(1);
        }
        salary = static_cast<std::uint32_t>(parsed_salary);
    }

    // Create an I/O context, required by all I/O objects
    asio::io_context ctx;

    // Launch our coroutine
    asio::co_spawn(
        ctx,
        [=] { return coro_main(argv[3], argv[1], argv[2], argv[4], argv[5], argv[6], salary); },
        // If any exception is thrown in the coroutine body, rethrow it.
        [](std::exception_ptr ptr) {
            if (ptr)
            {
                std::rethrow_exception(ptr);
            }
        }
    );

    // Calling run will actually execute the coroutine until completion
    ctx.run();

    std::cout << "Done\n";
}

int main(int argc, char** argv)
{
    try
    {
        main_impl(argc, argv);
    }
    catch (const boost::mysql::error_with_diagnostics& err)
    {
        // Some errors include additional diagnostics, like server-provided error messages.
        // Security note: diagnostics::server_message may contain user-supplied values (e.g. the
        // field value that caused the error) and is encoded using to the connection's character set
        // (UTF-8 by default). Treat is as untrusted input.
        std::cerr << "Error: " << err.what() << ", error code: " << err.code() << '\n'
                  << "Server diagnostics: " << err.get_diagnostics().server_message() << std::endl;
        return 1;
    }
    catch (const std::exception& err)
    {
        std::cerr << "Error: " << err.what() << std::endl;
        return 1;
    }
}

//]

#else

#include <iostream>

int main()
{
    std::cout << "Sorry, your compiler doesn't have the required capabilities to run this example"
              << std::endl;
}

#endif
