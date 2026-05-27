#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

namespace MiniGames {

    // список слів для гри
    const int WORDS_COUNT = 10;

    inline string easyWords[WORDS_COUNT] = { "кіт", "дім", "сир", "ліс", "дах", "ніж", "рак", "дуб", "жук", "м'яч" };
    inline string easyHints[WORDS_COUNT] = { "Хатня тварина", "Там, де живуть", "Миші це полюбляють", "Там багато дерев", "Верхня частина будинку", "Ним ріжуть хліб", "Тварина з клешнями", "Велике міцне дерево", "Маленька комаха", "Ним грають у футбол" };
    inline string medWords[WORDS_COUNT] = { "собака", "дерево", "космос", "яблуко", "машина", "гітара", "квітка", "дракон", "золото", "колесо" };
    inline string medHints[WORDS_COUNT] = { "Друг людини", "Росте на вулиці", "Там знаходяться зорі", "Фрукт, що впав на Ньютона", "Транспортний засіб", "Музичний інструмент з 6 струнами", "Росте в саду і гарно пахне", "Міфічна істота, що дихає вогнем", "Дорогоцінний жовтий метал", "Кругла деталь автомобіля" };
    inline string hardWords[WORDS_COUNT] = { "програмування", "препроцесор", "компілятор", "алгоритм", "архітектура", "бібліотека", "шифрування", "оптимізація", "навігація", "ідентифікатор" };
    inline string hardHints[WORDS_COUNT] = { "Процес написання коду", "Обробляє макроси до компіляції", "Перекладає код для комп'ютера", "Послідовність дій для розв'язання задачі", "Побудова або структура системи", "Набір готових функцій у коді", "Захист даних від прочитання", "Покращення швидкодії програми", "Визначення місцезнаходження", "Унікальне ім'я змінної" };

    inline void DrawHangman(int mistakes) {
        cout << "\n  +---+\n";
        cout << "  |   |\n";
        if (mistakes >= 1) cout << "  O   |\n";
        else cout << "      |\n";
        if (mistakes == 2) cout << "  |   |\n";
        else if (mistakes == 3) cout << " /|   |\n";
        else if (mistakes >= 4) cout << " /|\\  |\n";
        else cout << "      |\n";
        if (mistakes == 5) cout << " /    |\n";
        else if (mistakes >= 6) cout << " / \\  |\n";
        else cout << "      |\n";
        cout << "      |\n";
        cout << "=========\n\n";
    }

	// функція для розбиття рядка на літери щоб працювало кодування 65001 (UTF-8)
    inline vector<string> SplitToLetters(const string& str) {
        vector<string> letters;
        for (size_t i = 0; i < str.length(); ) {
            size_t len = 1;
            unsigned char c = str[i];
            if (c >= 0xf0) len = 4;
            else if (c >= 0xe0) len = 3;
            else if (c >= 0xc0) len = 2;

            if (i + len > str.length()) len = str.length() - i;
            letters.push_back(str.substr(i, len));
            i += len;
        }
        return letters;
    }


    inline void PlayRound(string word, string hint, int& coins) {
        int mistakes = 0;
        vector<string> letters = SplitToLetters(word);
        vector<bool> povt(letters.size(), false);

        for (size_t i = 0; i < letters.size(); i++) {
            if (letters[i] == "'" || letters[i] == "-" || letters[i] == " ") {
                povt[i] = true;
            }
        }

        string usedLetters = "";
        string alertMessage = "";

        while (mistakes < 6) {
            system("cls");

            if (alertMessage != "") {
                cout << "--- " << alertMessage << " ---\n";
                alertMessage = "";
            }

            cout << "    ГРА ПОЧАЛАСЯ    \n";
            cout << "Ваш баланс: " << coins << " монет\n";
            DrawHangman(mistakes);

            cout << "Підказка: " << hint << "\n";
            cout << "Слово: ";
            for (size_t i = 0; i < letters.size(); i++) {
                if (povt[i]) cout << letters[i] << " ";
                else cout << "_ ";
            }

            cout << "\n\nВикористані літери: " << usedLetters << "\n";

            bool allOpened = true;
            for (bool b : povt) {
                if (!b) allOpened = false;
            }
            if (allOpened) break;

            cout << "\nВведіть маленьку літеру (або '1', щоб купити підказку за 15 монет): ";
            string charr;
            cin >> charr;

            if (charr == "1") {
                if (coins >= 15) {
                    coins -= 15;
                    int randIndex;
                    do {
                        randIndex = rand() % letters.size();
                    } while (povt[randIndex]);

                    string hintLetter = letters[randIndex];
                    for (size_t i = 0; i < letters.size(); i++) {
                        if (letters[i] == hintLetter) povt[i] = true;
                    }
                }
                else alertMessage = "Недостатньо монет для купівлі підказки";
                continue;
            }

            if (usedLetters.find(charr) != string::npos) {
                alertMessage = "Ви вже вводили цю літеру";
                continue;
            }

            usedLetters += charr + " ";

            // перевірка літери
            bool found = false;
            for (size_t i = 0; i < letters.size(); i++) {
                if (letters[i] == charr) {
                    povt[i] = true;
                    found = true;
                }
            }
            if (!found) mistakes++;
        }

        system("cls");
        DrawHangman(mistakes);

        // перевірка чи відкриті всі літери
        bool gameWon = true;
        for (bool b : povt) {
            if (!b) gameWon = false;
        }

        if (gameWon) {
            cout << "\nВітаємо! Ви вгадали слово: " << word << "\n";
            cout << "+ 10 монет\n";
            coins += 10;
        }
        else {
            cout << "\nВи програли! Загадане слово було: " << word << "\n";
        }
        system("pause");
    }

    inline long long PlayHangman() {
        srand((unsigned int)time(NULL));
        int coins = 0;
        int choice = 0;

        while (choice != 2) {
            system("cls");
            cout << "\n           ГРА ШИБЕНИЦЯ             \n";
            cout << "Ваш баланс: " << coins << " монет\n\n";
            cout << "1. Грати\n";
            cout << "2. Вийти та забрати зароблені гроші\n";
            cout << "Оберіть дію: ";
            cin >> choice;

            if (choice == 1) {
                cout << "\nОберіть рівень складності:\n1 - Легкий\n2 - Середній\n3 - Складний\nВибір: ";
                int ch; cin >> ch;
                int randomIndex = rand() % WORDS_COUNT;
                string word_g, hint_g;

                switch (ch) {
                case 1: word_g = easyWords[randomIndex]; hint_g = easyHints[randomIndex]; break;
                case 2: word_g = medWords[randomIndex]; hint_g = medHints[randomIndex]; break;
                case 3: word_g = hardWords[randomIndex]; hint_g = hardHints[randomIndex]; break;
                default: cout << "Невірний вибір!\n"; continue;
                }
                PlayRound(word_g, hint_g, coins);
            }
        }
        return (long long)coins * 1000;
    }
}