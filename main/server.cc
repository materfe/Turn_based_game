
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include <vector>
#include <array>
#include <memory>
#include <iostream>

#include "const.h"
#include "playground.h"

struct Player {
  std::unique_ptr<sf::TcpSocket> socket_;
  std::int32_t id_ = -1;
};

int main() {
  power_of_four::Playground playground{};
  std::int32_t number_turn = 1;
  auto who_won = power_of_four::TileState::Empty;
  auto game_finished = false;

  std::vector<std::unique_ptr<Player>> players;
  players.reserve(2);
  sf::TcpListener listener;
  sf::SocketSelector socketSelector;

  listener.setBlocking(false);
  const auto listenerStatus = listener.listen(PORT_NUMBER);
  switch (listenerStatus) {
    case sf::Socket::Status::Done: break;
    default: std::cerr << "Error while listening\n";
      return EXIT_FAILURE;
  }
  while (true) {
    for (auto &socket : players) {
      if (socket->socket_ == nullptr)
        continue;
      if (socket->socket_->getLocalPort() == 0) {
        socket->socket_ = nullptr;
      }
    }
    {
      sf::TcpSocket socket;
      socket.setBlocking(false);
      const auto newSocketStatus = listener.accept(socket);
      if (newSocketStatus == sf::Socket::Status::Done) {
        const auto new_player_id = static_cast<int>(players.size());
        auto new_socket = std::make_unique<sf::TcpSocket>(std::move(socket));

        auto new_player = std::make_unique<Player>();
        new_player->socket_ = std::move(new_socket);
        new_player->id_ = new_player_id;

        socketSelector.add(*new_player->socket_);
        auto it = std::ranges::find_if(players, [](auto &local_socket) {
          return local_socket->socket_ == nullptr;
        });
        if (it != players.end()) {
          auto temp = std::move(*it);
          temp->socket_ = std::move(new_player->socket_);
        } else {
          if (players.size() < 2) {
            std::cout << "New player connected! Assigned ID: " << new_player_id << "\n";
            players.push_back(std::move(new_player));
          }
        }
      }
    }
    if (socketSelector.wait(sf::milliseconds(100))) {
      for (auto &socket : players) {
        if (socket->socket_ == nullptr)
          continue;

        if (socketSelector.isReady(*socket->socket_)) {
          std::string index;
          index.resize(MAX_MESSAGE_LENGTH, 0);
          size_t actualLength;

          sf::TcpSocket::Status
              receiveStatus = socket->socket_->receive(index.data(), MAX_MESSAGE_LENGTH, actualLength);

          //switch for the indexes
          switch (receiveStatus) {
            case sf::Socket::Status::Done: {
              const auto actual_number = std::stoi(index);
              //std::cout << "index received: " << actual_number << "\n";

              bool has_check_passed = false;

              //set the expected player of the current turn
              int expectedPlayer = 0;
              //the -1 is beacuse it counts the Empty in the enum
              if (number_turn % 2 == 0) {
                expectedPlayer = static_cast<int>(power_of_four::TileState::Player2) - 1;
              } else {
                expectedPlayer = static_cast<int>(power_of_four::TileState::Player1) - 1;
              }
              const auto current_right_player = static_cast<power_of_four::TileState>(socket->id_);

              //std::cout << "socket : " << socket->id_ << "actual requirred : " << expectedPlayer << "\n";

              //chose which player moves + locks the game if not two players are here
              if (socket->id_ != expectedPlayer or players.size() < 2) {
                break;
              }

              //check for validation + turn
              if (actual_number >= 0 && actual_number < power_of_four::kNumberOfColumns) {
                // Insert the coin in the lowest available row in that column

                playground.InsertCoin(current_right_player, actual_number);
                who_won = playground.CheckIfAnyPlayersWins();

                has_check_passed = true;
                number_turn++;
              }

              if (who_won == power_of_four::TileState::Player1) {
                std::cout << "player one wins the game!!!!!!!!!!!!!!!!!!!\n";
                game_finished = true;
              } else if (who_won == power_of_four::TileState::Player2) {
                std::cout << "player two wins the game!!!!!!!!!!!!!!!!!!!\n";
                game_finished = true;
              }

              //send messages to clients
              for (auto &otherSocket : players) {
                if (otherSocket->socket_ == nullptr) {
                  continue;
                }
                size_t sentDataCount;
                sf::TcpSocket::Status sendStatusIndex;
                sf::TcpSocket::Status sendStatusPlayer;
                const int player_int = static_cast<int>(expectedPlayer + 1);
                //send index
                do {
                  if (has_check_passed and !game_finished) {
                    const auto send_index = std::to_string(actual_number);
                    //std::cout << "sending this index : " << send_index << "\n";
                    sendStatusIndex = otherSocket->socket_->send(send_index.data(), actualLength, sentDataCount);
                  }
                } while (sendStatusIndex == sf::Socket::Status::Partial);
                //send player
                do {
                  if (has_check_passed and !game_finished) {
                    sendStatusPlayer = otherSocket->socket_->send(&player_int, sizeof(player_int), sentDataCount);
                  }
                } while (sendStatusPlayer == sf::Socket::Status::Partial);
              }
              break;
            }
            case sf::Socket::Status::Partial: {
              std::cerr << "Partial received...\n";
              break;
            }
            case sf::Socket::Status::Error: {
              std::cerr << "Error receiving\n";
              break;
            }

            case sf::Socket::Status::NotReady: {
              std::cerr << "Not ready on received\n";
              break;
            }
          }
        }
      }
    }
  }
}