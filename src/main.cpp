#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include "server.h"  // Подключение server.h

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

bool authenticateUser(sf::TcpSocket &socket, const std::string &username, const std::string &password) {
    sf::Packet packet;
    packet << username << password;
    socket.send(packet);

    sf::Packet responsePacket;
    if (socket.receive(responsePacket) == sf::Socket::Done) {
        std::string response;
        responsePacket >> response;
        return response != "Authentication Failed";
    }
    return false;
}

int main() {
    int startMode;
    std::cout << "0 - server, other - client";
    std::cin >> startMode;
    if (startMode == 0) {
        Server server(1234);
        server.start();
    } else {
        sf::RenderWindow authWindow(sf::VideoMode(400, 300), "Login");

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Could not load font" << std::endl;
            return -1;
        }

        sf::Text usernameText("Username:", font, 20);
        usernameText.setPosition(10, 10);
        usernameText.setFillColor(sf::Color::White);

        sf::Text passwordText("Password:", font, 20);
        passwordText.setPosition(10, 80);
        passwordText.setFillColor(sf::Color::White);

        sf::Text usernameInput("", font, 20);
        usernameInput.setPosition(10, 40);
        usernameInput.setFillColor(sf::Color::White);

        sf::Text passwordInput("", font, 20);
        passwordInput.setPosition(10, 110);
        passwordInput.setFillColor(sf::Color::White);

        std::string username, password;

        while (authWindow.isOpen()) {
            sf::Event event;
            while (authWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    authWindow.close();
                    return 0;
                } else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\r') { // Enter key
                        authWindow.close();
                        break;
                    } else if (event.text.unicode == '\b') { // Backspace key
                        if (usernameInput.getString().getSize() > 0) {
                            std::string str = usernameInput.getString();
                            str.pop_back();
                            usernameInput.setString(str);
                        }
                    } else if (event.text.unicode == '\t') { // Tab key
                        // Switch to password input
                        passwordInput.setString(usernameInput.getString());
                        usernameInput.setString("");
                    } else {
                        usernameInput.setString(usernameInput.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }

            authWindow.clear();
            authWindow.draw(usernameText);
            authWindow.draw(passwordText);
            authWindow.draw(usernameInput);
            authWindow.draw(passwordInput);
            authWindow.display();
        }

        username = usernameInput.getString();
        password = passwordInput.getString();

        sf::RenderWindow window(sf::VideoMode(800, 600), "Messenger");

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

        if (!authenticateUser(socket, username, password)) {
            std::cerr << "Authentication failed" << std::endl;
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
