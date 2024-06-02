#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include "server/server.h"  // Подключение server.h

std::vector<std::string> messages;
std::mutex messagesMutex;

void receiveMessages(sf::TcpSocket &socket) {
    sf::Packet packet;
    while (true) {
        if (socket.receive(packet) == sf::Socket::Done) {
            std::string message;
            packet >> message;
            std::lock_guard<std::mutex> lock(messagesMutex);
            messages.push_back(message);
            packet.clear();
        }
    }
}

int main() {
    int startMode;
    std::cout << "0 - server, other - client";
    std::cin >> startMode;
    if (startMode == 0) {
        Server server(1234);
        server.start();
    } else {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Messanger");

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Could not load font" << std::endl;
            return -1;
        }

        sf::Text userList("", font, 20);
        userList.setPosition(10, 10);
        userList.setFillColor(sf::Color::White);

        sf::Text chatBox("", font, 20);
        chatBox.setPosition(150, 10);
        chatBox.setFillColor(sf::Color::White);

        sf::Text messageInput("", font, 20);
        messageInput.setPosition(10, 550);
        messageInput.setFillColor(sf::Color::White);

        sf::TcpSocket socket;
        if (socket.connect("localhost", 1234) != sf::Socket::Done) {
            std::cerr << "Error connecting to server" << std::endl;
            return -1;
        }

        std::thread receiver(receiveMessages, std::ref(socket));
        receiver.detach();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                } else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\r') { // Enter key
                        std::string message = messageInput.getString();
                        sf::Packet packet;
                        packet << message;
                        socket.send(packet);
                        {
                            std::lock_guard<std::mutex> lock(messagesMutex);
                            messages.push_back("Me: " + message);
                        }
                        messageInput.setString("");
                    } else if (event.text.unicode == '\b') { // Backspace key
                        std::string str = messageInput.getString();
                        if (!str.empty()) {
                            str.pop_back();
                            messageInput.setString(str);
                        }
                    } else {
                        messageInput.setString(messageInput.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(messagesMutex);
                std::string chatContent;
                for (const auto &msg: messages) {
                    chatContent += msg + "\n";
                }
                chatBox.setString(chatContent);
            }

            window.clear();
            window.draw(userList);
            window.draw(chatBox);
            window.draw(messageInput);
            window.display();
        }
    }
    return 0;
}
