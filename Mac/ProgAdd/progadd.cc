#include <iostream>
#include <cstdlib>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <cstring>

std::string readPassword() {
    struct termios oldT, newT;
    tcgetattr(STDIN_FILENO, &oldT);
    newT = oldT;
    newT.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newT);

    char password[100]; // Adjust the size as needed
    std::cin.getline(password, sizeof(password));

    tcsetattr(STDIN_FILENO, TCSANOW, &oldT); // Restore terminal settings

    return std::string(password);
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cout << "Error, no arguments given\n";
        std::cerr << "Usage: " << argv[0] << " path/to/program" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];

    std::cout << "Enter user password: ";
    std::string password = readPassword();
    std::cout << "\n";

    std::string command = "echo " + password + " | sudo -S mv " + filePath + " /usr/local/bin/";

    int result = system(command.c_str());

    if(result){
        std::cout << "Error: Command execution failed.\n";
        return 1;
    }

    return 0;
}