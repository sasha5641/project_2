#include <iostream>
#include "Finance.h"
#include <windows.h>

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    BankSystem::GameManager game;
    game.showMenu();
}