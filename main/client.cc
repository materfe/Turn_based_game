
#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <iostream>

#include "const.h"
#include "playlist.h"
#include "playground.h"
#include "SFML/Graphics/CircleShape.hpp"

enum class Status {
  NOT_CONNECTED,
  CONNECTED
};

int main() {
  //SET the playground of the game
  power_of_four::Playground playground{};
  auto final_result = power_of_four::TileState::Empty;
  bool has_anyone_won = false;
  const int radius = 50;
  const int spacing = 100;
  auto circle = sf::CircleShape(radius);
  auto position = sf::Vector2<float>(30, 30);
  circle.setFillColor(sf::Color::Green);
  circle.setPosition(position);

  sf::RenderWindow window(sf::VideoMode({1280, 720}), "Simple Chat");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  bool isOpen = true;
  sf::Clock deltaClock;
  Status status = Status::NOT_CONNECTED;

  std::vector<std::string> receivedMessages;
  std::int32_t current_received_instruction_index;
  power_of_four::TileState current_received_instruction_player;

  sf::TcpSocket socket;
  socket.setBlocking(false);
  std::string serverAddress = "localhost";
  serverAddress.resize(50, 0);
  short portNumber = PORT_NUMBER;
  //message you send
  std::string sendMessage;
  sendMessage.resize(MAX_MESSAGE_LENGTH, 0);

  //index send when a player tries to take a tile
  std::string Index;
  Index.resize(MAX_MESSAGE_LENGTH, 0);

  //message received
  std::string receivedMessage;
  receivedMessage.resize(MAX_MESSAGE_LENGTH, 0);

  //received instructions from serv
  std::string received_instruction_index;
  received_instruction_index.resize(MAX_MESSAGE_LENGTH, 0);
  std::string received_instruction_player;
  received_instruction_player.resize(MAX_MESSAGE_LENGTH, 0);

  while (isOpen) {
    while (const std::optional event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, *event);
      if (event->is<sf::Event::Closed>()) {
        isOpen = false;
      }
      if (event->is<sf::Event::MouseButtonPressed>()) {
        auto mousePos = sf::Mouse::getPosition(window);

        // Find which column was clicked
        int clickedCol = (mousePos.x - 2 * radius) / spacing;

        size_t dataSent;
        sf::TcpSocket::Status sendStatus;
        Index = std::to_string(clickedCol);
        do {
          sendStatus = socket.send(Index.data(), MAX_MESSAGE_LENGTH, dataSent);
        } while (sendStatus == sf::Socket::Status::Partial);

        //TODO serv checks
        final_result = playground.CheckIfAnyPlayersWins();
      }
    }

    if (status == Status::CONNECTED) {
      //message receiving
      size_t actuallyReceived;
      //std::cout << "trying index\n";
      //index receiving
      auto receivedStatus = socket.receive(received_instruction_index.data(), MAX_MESSAGE_LENGTH, actuallyReceived);
      if (receivedStatus == sf::Socket::Status::Done) {
        received_instruction_index.resize(actuallyReceived);  // Trim to actual length

        // Convert index to an integer
        int actualNumber = std::stoi(received_instruction_index);

        //std::cout << "Index received: " << actualNumber << std::endl;

        // Receive player ID
        int receivedPlayer;
        size_t receivedDataCount;
        sf::TcpSocket::Status receivePlayerStatus = socket.receive(&receivedPlayer, sizeof(receivedPlayer), receivedDataCount);

        if (receivePlayerStatus == sf::Socket::Status::Done) {
          //std::cout << "Player received: " << receivedPlayer << std::endl;

          // Process the move in the game
          playground.InsertCoin(static_cast<power_of_four::TileState>(receivedPlayer), actualNumber);
        } else {
          std::cerr << "Error receiving player ID\n";
        }
      }

      std::cout << "local port\n";
      if (socket.getLocalPort() == 0) {
        status = Status::NOT_CONNECTED;
      }
    }
    ImGui::SFML::Update(window, deltaClock.restart());
    auto [x, y] = window.getSize();
    ImGui::SetNextWindowSize({(float) x, (float) y}, ImGuiCond_Always);
    ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_Always);
    ImGui::Begin("Simple Chat", nullptr, ImGuiWindowFlags_NoTitleBar);
    switch (status) {
      case Status::NOT_CONNECTED: {
        ImGui::InputText("Host Address", serverAddress.data(), serverAddress.size());
        ImGui::SameLine();
        ImGui::Text("%hd", portNumber);
        if (ImGui::Button("Connect")) {
          if (auto address = sf::IpAddress::resolve(serverAddress)) {
            socket.setBlocking(true);
            const auto connectionStatus = socket.connect(address.value(), portNumber);
            switch (connectionStatus) {
              case sf::Socket::Status::Done:status = Status::CONNECTED;
                break;
              case sf::Socket::Status::NotReady:std::cerr << "Socket not ready\n";
                break;
              case sf::Socket::Status::Partial:std::cerr << "Partial\n";
                break;
              case sf::Socket::Status::Disconnected:std::cerr << "Socket disconnected\n";
                break;
              case sf::Socket::Status::Error:std::cerr << "Socket error\n";
                break;
            }
            socket.setBlocking(false);
          }
        }
        break;
      }

      case Status::CONNECTED: {
        //who wins
//        if (final_result == power_of_four::TileState::Player1 && !has_anyone_won) {
//          size_t dataSent;
//          sf::TcpSocket::Status sendStatus;
//          sendMessage = "player one wins";
//          do {
//            sendStatus = socket.send(sendMessage.data(), MAX_MESSAGE_LENGTH, dataSent);
//          } while (sendStatus == sf::Socket::Status::Partial);
//          has_anyone_won = true;
//        }
//        for (const auto &message : receivedMessages) {
//          ImGui::Text("Received message: %s", message.data());
//        }
//        if (final_result == power_of_four::TileState::Player2 && !has_anyone_won) {
//          size_t dataSent;
//          sf::TcpSocket::Status sendStatus;
//          sendMessage = "player two wins";
//          do {
//            sendStatus = socket.send(sendMessage.data(), MAX_MESSAGE_LENGTH, dataSent);
//          } while (sendStatus == sf::Socket::Status::Partial);
//          has_anyone_won = true;
//        }
//        for (const auto &message : receivedMessages) {
//          ImGui::Text("Received message: %s", message.data());
//        }

        //where things are done (colors)
        window.clear(sf::Color::White);  // Clear with background color
        for (int row = 0; row < power_of_four::kNumberOfRows; row++) {
          for (int col = 0; col < power_of_four::kNumberOfColumns; col++) {
            int index = row * power_of_four::kNumberOfColumns + col;  // Get index

            // Set position for circle in grid layout
            circle.setPosition(sf::Vector2f(static_cast<float>(col) * spacing + radius * 2,
                                            static_cast<float>(row) * spacing + radius * 2));

            // Set color based on tile state
            if (playground.ReturnState(index) == power_of_four::TileState::Player1) {
              circle.setFillColor(sf::Color::Red);
            } else if (playground.ReturnState(index) == power_of_four::TileState::Player2) {
              circle.setFillColor(sf::Color::Blue);
            } else {
              circle.setFillColor(sf::Color(20, 20, 20, 255));  // Empty tile
            }
            window.draw(circle);  // Draw each circle
          }

        }

        break;
      }
    }
    ImGui::End();
    ImGui::SFML::Render(window);
    window.display();
  }
  ImGui::SFML::Shutdown();
}