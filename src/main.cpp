#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>

struct Bill {
    std::string name;
    std::chrono::day due_date;
    int amount{0}; // cents
    bool paid{false};
};

unsigned int add_amount(unsigned int init, Bill b) {
    return init + b.amount;
}

std::string dollar_amount(unsigned long long amount) {
    return "$" + std::to_string(amount / 100) + "." + std::to_string(amount % 100);
}

std::string dollar_amount(Bill b) {
    return dollar_amount(b.amount);
}

unsigned int days_until_due(Bill b) {
    const std::chrono::time_point now{std::chrono::system_clock::now()};
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
    const std::chrono::year_month_day_last ymdl{ymd.year(), ymd.month() / std::chrono::last};

    unsigned int out = 0;

    // If the due date is before today, then the bill is due next month.
    if (b.due_date < ymd.day())
        out = (static_cast<unsigned>(ymdl.day()) - static_cast<unsigned>(ymd.day())) + static_cast<unsigned>(b.due_date);
    // If the due date is today or after today, then the bill is due this month.
    else {
        out = static_cast<unsigned>(b.due_date) - static_cast<unsigned>(ymd.day());
        // unless the bill has been paid (in advance) this month, in which case
        // the bill is due next month.
        if (b.paid) out += static_cast<unsigned>(ymdl.day());
    }

    return out;
}

// due on 18, currently 21, 18 should go later than 22
bool bill_compare_date(Bill a, Bill b) {
    return days_until_due(a) < days_until_due(b);
}

void parse_bills(std::string_view bills_source) {
    // If character encountered is a space, discard and move on.
    while (not bills_source.empty() and std::isspace(bills_source.at(0)))
        bills_source.remove_prefix(1);

    if (bills_source.empty()) {
        std::cout << "Source file empty: no bills\n";
        return;
    }

    // skip dollar sign, if present.
    if (bills_source.at(0) == '$') bills_source.remove_prefix(1);

    // invalid amount error message
    if (not std::isdigit(bills_source.at(0))) {
        std::cerr << "ERROR in bills source file: amount does not begin with digit\n";
    }

    // parse number
    auto amount = std::stol(bills_source.data());
    std::cout << "Parsed bill amount '" << amount << "'\n";

    // eat number
    while (not bills_source.empty() and std::isdigit(bills_source.at(0)))
        bills_source.remove_prefix(1);

    // skip whitespace to reach beginning of bill's name
    while (not bills_source.empty() and std::isspace(bills_source.at(0)))
        bills_source.remove_prefix(1);

    // everything up until the due date digit(s) is part of the bill name
    std::string name{};
    while (not bills_source.empty() and not std::isdigit(bills_source.at(0))) {
        name += bills_source.at(0);
        bills_source.remove_prefix(1);
    }
    auto name_end = name.find_last_not_of(" \r\n\t\v");
    name.erase(name_end + 1);
    std::cout << "Parsed bill name '" << name << "'\n";

    // parse due date
    auto due_date = std::stol(bills_source.data());
    std::cout << "Parsed bill due date '" << due_date << "'\n";
    // eat number
    while (not bills_source.empty() and std::isdigit(bills_source.at(0)))
        bills_source.remove_prefix(1);

    // eat 'st', 'nd', 'rd', or 'th' (ordinal affix)
    if (bills_source.starts_with("st") or bills_source.starts_with("nd") or
        bills_source.starts_with("rd") or bills_source.starts_with("th")) {
        bills_source.remove_prefix(2);
    }

    std::cout << "Parsed bill '" << name << "' of amount '" << amount << "' with due date '" << due_date << "'\n";
    std::cout << "Remaining source:\n'" << bills_source << "'\n";
}

int main(int argc, char** argv) {
    // TODO: Handle "pay <bill_name>" command to set a bill as paid
    // TODO: Handle "unpay <bill_name>", "unpay" command to set a bill or all
    // bills as paid
    // TODO: Handle "new_bill <bill_name> <bill_due_date> <bill_amount_cents>"
    // command to create a new bill.
    if (argc > 1) {
        std::cout << "Sorry, but arguments aren't handled yet.\n";
        return 0;
    }

    parse_bills("$1325        Rent                 1st   $325         Car Insurance        5th   $262.62      Car Payment          21st");

    const std::chrono::time_point now{std::chrono::system_clock::now()};
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
    const std::chrono::year_month_day_last ymdl{ymd.year(), ymd.month() / std::chrono::last};

    // TODO: Make data driven (read/parse from file at runtime)
    std::vector<Bill> bills{
        {"Rent", std::chrono::day(1), 132500},
        {"Car Insurance", std::chrono::day(5), 32500},
        {"Car Payment", std::chrono::day(21), 26262},
        {"Internet", std::chrono::day(18), 8800},
        {"Phone", std::chrono::day(17), 6800},
        {"Electricity", std::chrono::day(15), 15000},
        {"Renter's Insurance", std::chrono::day(12), 1500},
    };

    // Sort by due date
    std::sort(bills.begin(), bills.end(), bill_compare_date);

    // calculate monthly expenses
    std::cout << "Total Monthly Expenses: " << dollar_amount(std::accumulate(bills.begin(), bills.end(), 0, add_amount)) << "\n";

    for (const auto b : bills) {
        // print dollar amount
        std::cout << std::setw(15) << std::left << dollar_amount(b);
        // print bill description/name
        std::cout << std::setw(20) << std::left << b.name;

        if (b.paid) {
            std::cout << "PAID! ";
        }

        // print due date
        std::cout << "due on " << b.due_date;
        // print days remaining
        std::cout << " (in " << days_until_due(b) << " days)";
        std::cout << "\n";
    }

    return 0;
}
