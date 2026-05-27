#include <windows.h> 
#include "Finance.h"
#include "Hangman.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

namespace BankSystem {

    // зміна кольору тексту у консолі
    void setConsoleColor(int colorCode) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, colorCode);
    }


    Transaction::Transaction() {
        amount = 0;
        category = "";
        isIncome = false;
        date = "00.00.0000";
    }
    Transaction::Transaction(long long a, string c, bool i, string d) : amount(a), category(c), isIncome(i), date(d) {}
    long long Transaction::getAmount() const { return amount; }
    string Transaction::getCategory() const { return category; }
    bool Transaction::getIsIncome() const { return isIncome; }
    string Transaction::getDate() const { return date; }

    // вивід історії
    void Transaction::printTransaction() const {
        cout << "[" << date << "] ";
        if (isIncome) {
            setConsoleColor(10); cout << "+ ";
        }
        else {
            setConsoleColor(12); cout << "- ";
        }
        setConsoleColor(7);                       // заповнює нулі | виділяє пам'ять под 2 цифри | модуль  
        cout << category << ": " << amount / 100 << "." << setfill('0') << setw(2) << abs(amount % 100) << " грн\n";
    }

    // вивід історії дій у текстовий файл
    void Transaction::printReport(ofstream& out) const {
        out << "[" << date << "] ";
        if (isIncome) out << "+ ";
        else out << "- ";
        out << category << ": " << amount / 100 << "." << setfill('0') << setw(2) << abs(amount % 100) << " грн\n";
    }

    void Transaction::save(ofstream& out) const {
        out << amount << " " << isIncome << " " << date << "\n" << category << "\n";
    }
    void Transaction::load(ifstream& in) {
        in >> amount >> isIncome >> date;
        in.ignore();
        getline(in, category);
    }

    Account::Account(string n, long long b) : name(n), balance(b) {}
    Account::~Account() {}
    long long Account::getBalance() const { return balance; }



    void Account::addTransaction(const Transaction& t) {
        history.push_back(t);
        if (t.getIsIncome()) balance += t.getAmount();
        else balance -= t.getAmount();
    }

    void Account::printStatus() const {
        cout << name << " | Баланс: " << balance / 100 << "." << setfill('0') << setw(2) << abs(balance % 100) << " грн\n";
    }

    void Account::printHistory() const {
        cout << "\n--- Історія: " << name << " ---\n";
        if (history.empty()) { cout << "Пусто.\n"; return; }
        long long tInc = 0, tExp = 0;
        for (size_t i = 0; i < history.size(); i++) {
            history[i].printTransaction();
            if (history[i].getIsIncome()) tInc += history[i].getAmount();
            else tExp += history[i].getAmount();
        }
        cout << "-----------------------\n";
        cout << "Доходи: " << tInc / 100 << " грн\n";
        cout << "Витрати: " << tExp / 100 << " грн\n";
    }


    // вивід всієї історії у файл
    void Account::writeReport(ofstream& out) const {
        out << "\n--- Історія транзакцій: " << name << " ---\n";
        if (history.empty()) {
            out << " Транзакцій ще не було.\n";
            return;
        }
        long long tInc = 0, tExp = 0;
        for (size_t i = 0; i < history.size(); i++) {
            out << " ";
            history[i].printReport(out);
            if (history[i].getIsIncome()) tInc += history[i].getAmount();
            else tExp += history[i].getAmount();
        }
        out << " -------------------------------------------------\n";
        out << " Загалом доходів: " << tInc / 100 << "." << setfill('0') << setw(2) << abs(tInc % 100) << " грн\n";
        out << " Загалом витрат:  " << tExp / 100 << "." << setfill('0') << setw(2) << abs(tExp % 100) << " грн\n";
    }


    // вивід інформації про картки у файл
    void Account::save(ofstream& out) const {
        out << balance << " " << history.size() << "\n";
        for (size_t i = 0; i < history.size(); i++) history[i].save(out);
    }
    void Account::load(ifstream& in) {
        size_t loadedSize;
        in >> balance >> loadedSize;
        history.clear();
        for (size_t i = 0; i < loadedSize; i++) {
            Transaction t;
            t.load(in);
            history.push_back(t);
        }
    }





    CreditAccount::CreditAccount(string n, long long l) : Account(n, l), debt(0), limit(l) {}
    long long CreditAccount::getDebt() const { return debt; }

    // кредит
    bool CreditAccount::takeCredit(long long amount, string date) {
        if (amount <= 0 || debt + amount > limit) return false;
        debt += amount;
        addTransaction(Transaction(amount, "Кредит", false, date));
        return true;
    }

    bool CreditAccount::repay(long long amount, string date) {
        if (amount <= 0 || amount > debt) return false;
        debt -= amount;
        addTransaction(Transaction(amount, "Погашення", true, date));
        return true;
    }

    void CreditAccount::applyMonthlyInterest(string date) {
        if (debt > 0) {
            long long interest = debt * 2 / 100;
            if (interest == 0) interest = 1;
            debt += interest;
            addTransaction(Transaction(interest, "Відсотки (2%)", false, date));
        }
    }


    // вивід інформації про кредитну картку
    void CreditAccount::printStatus() const {
        double closedPercent = 100.0;
        if (limit > 0) closedPercent = ((double)(limit - debt) / limit) * 100.0;

        cout << name << " | Доступно: " << balance / 100 << "." << setfill('0') << setw(2) << abs(balance % 100) << " грн\n";
        cout << "Борг: ";
        if (debt > 0) setConsoleColor(12);
        else setConsoleColor(10);
        cout << debt / 100 << "." << setfill('0') << setw(2) << abs(debt % 100) << " грн";
        setConsoleColor(7); cout << "\n";

        cout << "Прогрес: ";
        if (closedPercent == 100.0) setConsoleColor(10);
        else setConsoleColor(14);
        int barWidth = 10;
        int filledSegments = (int)(closedPercent / 10);
        for (int i = 0; i < barWidth; i++) {
            if (i < filledSegments) cout << "█"; else cout << "░";
        }
        setConsoleColor(7); cout << " " << (int)closedPercent << "%\n";
    }

    void CreditAccount::save(ofstream& out) const {
        out << balance << " " << debt << " " << limit << " " << history.size() << "\n";
        for (size_t i = 0; i < history.size(); i++) history[i].save(out);
    }
    void CreditAccount::load(ifstream& in) {
        size_t loadedSize;
        in >> balance >> debt >> limit >> loadedSize;
        history.clear();
        for (size_t i = 0; i < loadedSize; i++) {
            Transaction t;
            t.load(in);
            history.push_back(t);
        }
    }



    GameManager::GameManager() {
        mainAccount = new Account("Зарплатна", 0);
        creditCard = new CreditAccount("Універсальна", 5000000);
        cDay = 27; cMonth = 5; cYear = 2026;
        monthlySalary = 0;
        monthlyUtilities = 0;
        userPassword = "";

        if (!loadFromFile()) {
            isNewUser = true;
        }
        else {
            isNewUser = false;
            generateReport();
        }
    }

    GameManager::~GameManager() {
        delete mainAccount;
        delete creditCard;
    }

    string GameManager::getCurrentDate() const {
        string d = (cDay < 10 ? "0" : "") + to_string(cDay);
        string m = (cMonth < 10 ? "0" : "") + to_string(cMonth);
        return d + "." + m + "." + to_string(cYear);
    }


    // вхід в систему
    bool GameManager::authenticate() {
        string input;
        int attempts = 3;

        cout << "                  ВХІД                  \nЗнайдено файл даних\n";

        while (attempts > 0) {
            cout << "Введіть пароль від вашого рахунку: ";
            cin >> input;

            if (input == userPassword) {
                cout << "\nПароль вірний. Доступ дозволено!\n";
                Sleep(1000);
                system("cls");
                return true;
            }

            attempts--;
            cout << "Невірний пароль! Залишилось спроб: " << attempts << "\n\n";
        }

        cout << "Доступ заборонено. Завершення програми...\n";
        return false;
    }


    // перший запуск програми
    void GameManager::initNewUser() {
        cout << "=========================================\n";
        cout << "   Фінансовий симулятор (перший запуск)  \n";
        cout << "=========================================\n";

        cout << "1. Створіть надійний пароль для входу в систему (без пробілів): ";
        cin >> userPassword;

        long long startBalance, salary, utilities;
        cout << "2. Введіть початковий баланс зарплатної картки (грн): ";
        cin >> startBalance;

        cout << "3. Введіть вашу щомісячну зарплату (грн): ";
        cin >> salary;

        cout << "4. Введіть суму комуналки, що зніматиметься щомісяця (грн): ";
        cin >> utilities;

        monthlySalary = salary * 100;
        monthlyUtilities = utilities * 100;

        mainAccount->addTransaction(Transaction(startBalance * 100, "Початковий баланс", true, getCurrentDate()));
        mainAccount->addTransaction(Transaction(50000, "Bonus", true, getCurrentDate()));

        isNewUser = false;
        saveToFile();

        cout << "\nКартку успішно налаштовано! Переходимо до головного меню...\n";
        Sleep(2500);
        system("cls");
    }


    // вивід звіту у текстовий файл
    void GameManager::generateReport() {
        ofstream report("financial_report.txt");
        if (report.is_open()) {
            report << "====================================\n";
            report << "          Фінансовий звіт           \n";
            report << "====================================\n";
            report << " Поточний баланс картки : " << mainAccount->getBalance() / 100.0 << " грн\n";
            report << " Поточний борг по кредиту: " << creditCard->getDebt() / 100.0 << " грн\n";
            report << "------------------------------------\n";
            report << " Фіксована зарплата (ЗП) : " << monthlySalary / 100.0 << " грн/міс\n";
            report << " Фіксована комуналка    : " << monthlyUtilities / 100.0 << " грн/міс\n";
            report << " Чистий дохід за місяць  : " << (monthlySalary - monthlyUtilities) / 100.0 << " грн\n";
            report << "====================================\n";

            mainAccount->writeReport(report);
            creditCard->writeReport(report);

            report << "\n==================================\n";
            report.close();
        }
    }

    // завантаження з файлу даних
    bool GameManager::loadFromFile() {
        ifstream in("bank_data.txt");
        if (!in.is_open()) return false;

        in >> userPassword >> cDay >> cMonth >> cYear >> monthlySalary >> monthlyUtilities;

        if (in.fail()) {
            in.close();
            return false;
        }

        mainAccount->load(in);
        creditCard->load(in);
        in.close();
        return true;
    }

    // збереження даних у адмінський файл
    void GameManager::saveToFile() {
        ofstream out("bank_data.txt");
        out << userPassword << " " << cDay << " " << cMonth << " " << cYear << " " << monthlySalary << " " << monthlyUtilities << "\n";
        mainAccount->save(out);
        creditCard->save(out);
        out.close();

        generateReport();
    }



    void GameManager::skipTime(int days) {
        int oldMonth = cMonth;
        int oldYear = cYear;

        cDay += days;
        while (cDay > 30) {
            cDay -= 30;
            cMonth++;
            if (cMonth > 12) {
                cMonth = 1;
                cYear++;
            }
        }

        int monthsPassed = (cYear - oldYear) * 12 + (cMonth - oldMonth);

        for (int i = 0; i < monthsPassed; i++) {
            mainAccount->addTransaction(Transaction(monthlySalary, "Зарплата", true, getCurrentDate()));
            mainAccount->addTransaction(Transaction(monthlyUtilities, "Комуналка", false, getCurrentDate()));
            creditCard->applyMonthlyInterest(getCurrentDate());
        }

        cout << "\nЧас минув. Сьогодні: " << getCurrentDate() << "\n";
        if (monthsPassed > 0) {
            cout << "Пройшов " << monthsPassed << " місяць(ів). Отримано ЗП, знято за комуналку.\n";
        }
        generateReport();
    }

    void GameManager::showMenu() {
        if (isNewUser) {
            initNewUser();
        }
        else {
            if (!authenticate()) {
                return;
            }
        }

        int choice = 0;
        while (true) {
            cout << "\n--- Меню (" << getCurrentDate() << ") ---\n";
            cout << "1. Баланс та Фінансові Дані\n2. Фріланс (Мінігра)\n3. Взяти кредит\n4. Витрата\n5. Промотати дні\n6. Історія\n7. Погасити кредит\n0. Вийти\nВибір: ";
            cin >> choice;

            if (choice == 0) {
                saveToFile();
                break;
            }
            else if (choice == 1) {
                cout << "\n";
                mainAccount->printStatus();
                creditCard->printStatus();
                cout << "---------------------------------------\n";
                cout << "Конфігурація: ЗП = " << monthlySalary / 100 << " грн | Комуналка = " << monthlyUtilities / 100 << " грн\n";
            }
            else if (choice == 2) {
                long long earned = MiniGames::PlayHangman();
                if (earned > 0) {
                    mainAccount->addTransaction(Transaction(earned, "Фріланс", true, getCurrentDate()));
                    cout << "Зароблено і виведено на картку: " << earned / 100 << " грн\n";
                }
                else {
                    cout << "Ви нічого не заробили.\n";
                }
                saveToFile();
            }
            else if (choice == 3) {
                cout << "Сума (грн): ";
                long long sum; cin >> sum;
                if (creditCard->takeCredit(sum * 100, getCurrentDate())) {
                    mainAccount->addTransaction(Transaction(sum * 100, "Кредит", true, getCurrentDate()));
                    cout << "Гроші отримано.\n";
                }
                else cout << "Відмова. Перевищено ліміт.\n";
                saveToFile();
            }
            else if (choice == 4) {
                cout << "\nКатегорії:\n";
                cout << "1. Інтернет-магазини   6. Перекази\n";
                cout << "2. Книги               7. Розваги\n";
                cout << "3. Послуги             8. Одяг\n";
                cout << "4. Їжа                 9. Техніка\n";
                cout << "5. Транспорт           10. Інше\n";
                cout << "Номер: ";

                int catChoice; cin >> catChoice;
                string selectedCategory = "Інше";
                switch (catChoice) {
                case 1: selectedCategory = "Інтернет-магазини"; break;
                case 2: selectedCategory = "Книги"; break;
                case 3: selectedCategory = "Послуги"; break;
                case 4: selectedCategory = "Їжа"; break;
                case 5: selectedCategory = "Транспорт"; break;
                case 6: selectedCategory = "Перекази"; break;
                case 7: selectedCategory = "Розваги"; break;
                case 8: selectedCategory = "Одяг"; break;
                case 9: selectedCategory = "Техніка"; break;
                }

                long long sum;
                cout << "Сума (грн): "; cin >> sum;

                if (sum * 100 > mainAccount->getBalance()) cout << "Недостатньо грошей!\n";
                else {
                    mainAccount->addTransaction(Transaction(sum * 100, selectedCategory, false, getCurrentDate()));
                    cout << "Витрату додано.\n";
                }
                saveToFile();
            }
            else if (choice == 5) {
                cout << "На скільки днів перемотати час?: ";
                int d; cin >> d;
                skipTime(d);
                saveToFile();
            }
            else if (choice == 6) {
                mainAccount->printHistory();
                creditCard->printHistory();
            }
            else if (choice == 7) {
                long long db = creditCard->getDebt();
                if (db == 0) { cout << "Боргів немає.\n"; continue; }
                cout << "Ваш борг: " << db / 100 << " грн\n";
                cout << "Сума погашення: ";
                long long sumToRepay; cin >> sumToRepay;
                long long cKop = sumToRepay * 100;

                if (cKop <= 0) cout << "Помилка.\n";
                else if (cKop > mainAccount->getBalance()) cout << "Немає грошей.\n";
                else {
                    if (cKop > db) cKop = db;
                    if (creditCard->repay(cKop, getCurrentDate())) {
                        mainAccount->addTransaction(Transaction(cKop, "Погашення", false, getCurrentDate()));
                        cout << "Сплачено.\n";
                    }
                }
                saveToFile();
            }
        }

    }
}