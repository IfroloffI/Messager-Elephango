#include <iostream>
#include <format>

int main() {
    const char *projectName = new char[20];
    projectName = "Elephango";
    std::cout << std::format("Hello, {}!", projectName) << "\n";
    return 0;
}
