#include <array>
#include <random>
#include <iostream>
#include "playground.h"


int main() {
    bool while_open = true;

    power_of_four::Playground ground;
    auto size = power_of_four::kNumberOfRows * power_of_four::kNumberOfColumns;

    srand(NULL);

    while (while_open) {
        auto index_x = rand() % power_of_four::kNumberOfColumns + 1;
        auto index_y = rand() % power_of_four::kNumberOfRows + 1;
        auto choice = rand() % 2;

        int final_index =
                index_y * power_of_four::kNumberOfRows - (power_of_four::kNumberOfColumns - index_x) - 1;

        if (final_index < size && choice == 0) {
            ground.InsertCoin(TileState::Player1, final_index);

        } else if (final_index < size && choice == 1)
        {
            ground.InsertCoin(TileState::Player2, final_index);
        }
        ground.DrawInConsole();

        std::cout << "\nchecking\n-------------------------------------------------\n";

        if(ground.CheckIfAnyPlayersWins())
        {
            while_open = false;
        }
    }

    return EXIT_SUCCESS;
}