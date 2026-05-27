#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace BankSystem {

    class Transaction {
        long long amount;
        std::string category;
        bool isIncome;
        std::string date;
    public:
        Transaction();
        Transaction(long long a, std::string c, bool i, std::string d);
        long long getAmount() const;
        std::string getCategory() const;
        bool getIsIncome() const;
        std::string getDate() const;

        void printTransaction() const;
        void printReport(std::ofstream& out) const;

        void save(std::ofstream& out) const;
        void load(std::ifstream& in);
    };

    class Account {
    protected:
        std::string name;
        long long balance;
        std::vector<Transaction> history;
    public:
        Account(std::string n, long long b);
        virtual ~Account();
        long long getBalance() const;
        void addTransaction(const Transaction& t);

        virtual void printStatus() const;
        void printHistory() const;
        void writeReport(std::ofstream& out) const;

        virtual void save(std::ofstream& out) const;
        virtual void load(std::ifstream& in);
    };

    class CreditAccount : public Account {
        long long debt;
        long long limit;
    public:
        CreditAccount(std::string n, long long l);
        long long getDebt() const;
        bool takeCredit(long long amount, std::string date);
        bool repay(long long amount, std::string date);
        void applyMonthlyInterest(std::string date);

        void printStatus() const override;
        void save(std::ofstream& out) const override;
        void load(std::ifstream& in) override;
    };

    class GameManager {
    private:
        Account* mainAccount;
        CreditAccount* creditCard;
        int cDay, cMonth, cYear;

        long long monthlySalary;
        long long monthlyUtilities;
        bool isNewUser;
        std::string userPassword;

        std::string getCurrentDate() const;
        bool loadFromFile();
        void saveToFile();
        void skipTime(int days);

        void initNewUser();
        void generateReport();
        bool authenticate();

    public:
        GameManager();
        ~GameManager();
        void showMenu();
    };
}