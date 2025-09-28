#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Function to randomly inject whitespace into commands
std::string injectRandomWhitespace(const std::string& command) {
    std::string result;
    for (char ch : command) {
        if (std::rand() % 17 == 0) { // 1/17 chance to inject whitespace
            result += " ";
        }
        result += ch;
        }
    return result;
}

std::string getRandomCommand(const std::vector<std::string>& commands) {
    int index = rand() % commands.size();
    return injectRandomWhitespace(commands[index]);
    //return commands[index];
}

int main() {
    std::srand(std::time(nullptr)); // Seed for random number generation

    // List of possible commands
    std::vector<std::string> commands = {
        "x=77",                    // Variables
        "x=add(5,10)",            
        "x=mul(2,3)",
        "up()", "down()",          // Nullary commands
        "forward(10)",             // Unitary commands
        "turn(90)",
        "setrot(45)",
        "setsize(2)",
        "setpos(10,20)",           // Binary commands
        "arc(5,90)",
        ""                         // Blank command
        //"setspeed(5)", // omit this from the testing to maximize the test speed
        //"LOOP3{up();down()}",       
    };

    // Open a file to write the test-script to
    std::ofstream outfile("fuzz_test.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing.\n";
        return 1;
    }

    outfile << "setspeed(9999999)\n"; // Fast tests

    // Generate 5 lines of commands
    for (int i = 0; i < 5; ++i) {
        std::string line;
        for (int j = 0; j < 7; ++j) { // 7 random commands on each line
            line += getRandomCommand(commands);
            line += ";";
        }
        outfile << line << "\n"; // Write the line to the file
    }

    outfile.close();
    std::cout << "Generated fuzz_test.txt with 5 lines of commands.\n";
    return 0;
}

