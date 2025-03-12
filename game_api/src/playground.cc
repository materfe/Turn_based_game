#include "playground.h"
#include <iostream>

namespace power_of_four {

void Playground::InsertCoin(TileState player, int col) {
  if (col < 0 || col >= kNumberOfColumns) {
    throw std::runtime_error("Invalid column index: too high or low");
  }

  // Start from the bottom row and find the first empty spot
  for (int row = kNumberOfRows - 1; row >= 0; row--) {
    int index = row * kNumberOfColumns + col; // Get correct index
    if (game_tiles_[index] == TileState::Empty) {
      game_tiles_[index] = player; // Place the coin here
      return;
    }
  }
}

void Playground::DrawInConsole() {
  int index = 0;
  for (auto &tile : game_tiles_) {
    if (tile == TileState::Empty) {
      std::cout << "\tX\t";
    } else if (tile == TileState::Player1) {
      std::cout << "\t1\t";
    } else if (tile == TileState::Player2) {
      std::cout << "\t2\t";
    }
    index++;
    if (index % kNumberOfColumns == 0) {
      std::cout << "\n";
    }
  }
}

TileState Playground::CheckIfAnyPlayersWins() {

  const std::int32_t amount_to_win = 4;

  // Check columns
  for (int col = 0; col < kNumberOfColumns; col++) {
    int player1_count = 0, player2_count = 0;
    for (int row = 0; row < kNumberOfRows; row++) {
      int index = col + (row * kNumberOfColumns);
      if (game_tiles_[index] == TileState::Player1) {
        player1_count++;
        player2_count = 0;
      } else if (game_tiles_[index] == TileState::Player2) {
        player2_count++;
        player1_count = 0;
      } else {
        player1_count = 0;
        player2_count = 0;
      }
      if (player1_count == amount_to_win) {
        std::cout << "column checking true for player 1\n";
        return TileState::Player1;
      }
      if (player2_count == amount_to_win) {
        std::cout << "column checking true for player 2\n";
        return TileState::Player2;
      }
    }
  }

  // Check rows
  for (int row = 0; row < kNumberOfRows; row++) {
    int player1_count = 0, player2_count = 0;
    for (int col = 0; col < kNumberOfColumns; col++) {
      int index = row * kNumberOfColumns + col;
      if (game_tiles_[index] == TileState::Player1) {
        player1_count++;
        player2_count = 0;
      } else if (game_tiles_[index] == TileState::Player2) {
        player2_count++;
        player1_count = 0;
      } else {
        player1_count = 0;
        player2_count = 0;
      }
      if (player1_count == amount_to_win) {
        std::cout << "row checking true for player 1\n";
        return TileState::Player1;
      }
      if (player2_count == amount_to_win) {
        std::cout << "row checking true for player 2\n";
        return TileState::Player2;
      }
    }
  }

  //TODO : look for bug in console when playing

  // Check diagonal (\ direction)
  for (int row = 0; row <= kNumberOfRows - amount_to_win; row++) {
    for (int col = 0; col <= kNumberOfColumns - amount_to_win; col++) {
      int player1_count = 0, player2_count = 0;
      for (int i = 0; i < amount_to_win; i++) {
        int index = (row + i) * kNumberOfColumns + (col + i);
        if (game_tiles_[index] == TileState::Player1) {
          player1_count++;
          player2_count = 0;
        } else if (game_tiles_[index] == TileState::Player2) {
          player2_count++;
          player1_count = 0;
        } else {
          player1_count = 0;
          player2_count = 0;
        }
        if (player1_count == amount_to_win) {
          std::cout << "diagonal left up and right down checking true for player 1\n";
          return TileState::Player1;
        }
        if (player2_count == amount_to_win) {
          std::cout << "diagonal left up and right down checking true for player 2\n";
          return TileState::Player2;
        }
      }
    }
  }

  // Check diagonal (/ direction)
  for (int row = 0; row <= kNumberOfRows - amount_to_win; row++) {
    for (int col = amount_to_win - 1; col < kNumberOfColumns; col++) {
      int player1_count = 0, player2_count = 0;
      for (int i = 0; i < amount_to_win; i++) {
        int index = (row + i) * kNumberOfColumns + (col - i);
        if (game_tiles_[index] == TileState::Player1) {
          player1_count++;
          player2_count = 0;
        } else if (game_tiles_[index] == TileState::Player2) {
          player2_count++;
          player1_count = 0;
        } else {
          player1_count = 0;
          player2_count = 0;
        }
        if (player1_count == amount_to_win) {
          std::cout << "diagonal left down and right up checking true for player 1\n";
          return TileState::Player1;
        }
        if (player2_count == amount_to_win) {
          std::cout << "diagonal left down and right up checking true for player 2\n";
          return TileState::Player2;
        }
      }
    }
  }

  return TileState::Empty;  // No win detected
}

TileState Playground::ReturnState(int index) {

  if(index < game_tiles_.size() and index >= 0)
  {
    return game_tiles_[index];
  }
  return TileState::Empty;
}

} //namespace power_of_four