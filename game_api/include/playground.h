//
// Created by Mat on 2/26/2025.
//

#ifndef SIMPLECHAT_PLAYGROUND_H
#define SIMPLECHAT_PLAYGROUND_H

#include <array>

namespace power_of_four {
static constexpr std::int32_t kNumberOfRows = 5;
static constexpr std::int32_t kNumberOfColumns = 5;

enum class TileState {
  Empty,
  Player1,
  Player2
};

class Playground {
 private :
  std::array<TileState, kNumberOfRows * kNumberOfColumns> game_tiles_{};

 public:
  Playground() {
    for (auto &tile : game_tiles_) {
      tile = TileState::Empty;
    }
  }

  //set tiles
  void InsertCoin(TileState player, int col);

  TileState ReturnState(int index);

  TileState CheckIfAnyPlayersWins();

  //draw in console : debug
  void DrawInConsole();
};
}

#endif //SIMPLECHAT_PLAYGROUND_H
