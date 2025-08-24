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

std::string dollar_amount(Bill b) {
    return "$" + std::to_string(b.amount / 100) + "." + std::to_string(b.amount % 100);
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
    const std::chrono::time_point now{std::chrono::system_clock::now()};
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
    const std::chrono::year_month_day_last ymdl{ymd.year(), ymd.month() / std::chrono::last};

    std::cout << "day: " << ymd.day() << "\n";

    std::vector<Bill> bills{
        {"Rent", std::chrono::day(1), 132500},
        {"Car Insurance", std::chrono::day(5), 32500},
        {"Car Payment", std::chrono::day(21), 26262, true},
        {"Internet", std::chrono::day(18), 8800},
    };

    // Sort by due date
    std::sort(bills.begin(), bills.end(), bill_compare_date);

    for (const auto b : bills) {
        // print dollar amount
        std::cout << std::setw(15) << std::left << dollar_amount(b);
        // print bill description/name
        std::cout << b.name << ": ";

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
