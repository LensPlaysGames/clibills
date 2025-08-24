#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>

struct Bill {
    std::string name;
    std::chrono::day due_date;
    int amount{0}; // cents
    bool paid{false};
};

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
    unsigned long long int expenses{0};
    for (const auto b : bills) {
        expenses += b.amount;
    }
    std::cout << "Total Monthly Expenses: " << dollar_amount(expenses) << "\n";

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
