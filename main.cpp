#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
using namespace std;

enum validCommands
{
    echo,
    cd,
    exit0,
    type,
    invalid,
}; 

validCommands isValid(string command) {
    command = command.substr(0,command.find(" ")); // starting from index 0 to the first space in string

    if(command == "echo") return validCommands::echo;
    if(command == "cd") return validCommands::cd;
    if(command == "exit") return validCommands::exit0;
    if(command == "type") return validCommands::type;

    return invalid; 
}



std::string get_path(const std::string& command) {
    // Get the PATH environment variable
    const char* path_env = std::getenv("PATH");
    if (!path_env) {
        return ""; // Return empty if PATH is not available
    }

    std::stringstream ss(path_env); //splits the string into delimeters
    std::string path;

    // Iterate through each directory in PATH
    while (getline(ss, path, ':')) { //extracts parts separated by :
        // Construct the absolute path to the command
        std::string abs_path = path + '/' + command;
        if (std::filesystem::exists(abs_path) && std::filesystem::is_regular_file(abs_path)) {
            return abs_path; // Return the path if the file exists
        }
    }
    return ""; // Return empty if the command is not found
}

int main() {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here!\n";

    bool exit = false;
    while (!exit) {
        // Flush after every std::cout / std:cerr
        cout << std::unitbuf;
        std::cerr << std::unitbuf;
        cout << "$ "; // Shell prompt

        string input;
        getline(std::cin, input);

        switch (isValid(input)) {
            case cd:
                // Implementation for `cd` can be added later
                break;

            case echo: {
                // Remove "echo" from the input and print the remaining string
                input.erase(0, input.find(" ") + 1);
                cout << input << "\n";
                break;
            }

            case exit0:
                exit = true; // Exit the shell
                break;

            case type: {
                // Remove "type" from the input and trim leading/trailing spaces
                input.erase(0, input.find(" ") + 1);
                input = input.substr(input.find_first_not_of(" ")); // Remove leading spaces

                if (input.empty()) {
                    cout << "type: missing argument\n";
                    break;
                }

                // Check if the argument is a shell builtin
                if (isValid(input) != invalid) {
                    std::cout << input << " is a shell builtin\n";
                } else {
                    // Search for the command in the PATH
                    std::string path = get_path(input);
                    if (!path.empty()) {
                        std::cout << input << " is " << path << "\n";
                    } else {
                        cout << input << " not found\n";
                    }
                }
                break;
            }

            default:
                cout << input << ": command not found\n";
                break;
        }
    }

    return 0;
}


