#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <regex>
#include <QString>
#include <QColor>
#include <QCoreApplication>

#include "parser.hpp"

Parser::Parser(QObject *parent) : QObject(parent) {vars = {}; funcs = {}; movement_done = true;}

void Parser::animation_done(){ movement_done = true; }

std::vector<std::string> Parser::parse_script(std::ifstream& file) {
    std::vector<std::string> parsed_commands; // Vector of commands that were parsed correctly and actually run

    std::string line;

    // Regex strings for function and loop definitions
    std::regex funcdef_regex(R"(^\s*DEF\s+([a-zA-Z_]\w*)\s*\(\s*([a-zA-Z_]\w*)?\s*\)\s*\{\s*$)");
    std::regex loopdef_regex(R"(^\s*LOOP\s*(\d+)\s*\{\s*$)");

    while (std::getline(file, line)) {
        // Wait for animation to be done before processing next lines
        while (!movement_done){
            QCoreApplication::processEvents(); // this is required to process the on_movement_completed signal
        }

        std::smatch match;

        // Try function definition
        if (std::regex_match(line, match, funcdef_regex)) {
            std::string func_name = match[1];
            std::string arg_name = match[2];
            
            std::ostringstream func_body;
            while (std::getline(file, line)) { // Collect function body
                if (line.find("}") != std::string::npos) break;
                func_body << line << "\n";
            }
            funcs[func_name] = func_body.str();

            // store the name of the function argument variable only if it exists
            if (!arg_name.empty()) { func_arg_names[func_name] = arg_name; }

            std::cout << "Function defined: " << func_name << " with arg: " << arg_name << std::endl;
            continue;
        }

        // Try loop definition
        if (std::regex_match(line, match, loopdef_regex)) {
            int loopCount = std::stoi(match[1]);
            std::ostringstream loopBody;
            while (std::getline(file, line)) { // Collect loop body
                if (line.find("}") != std::string::npos) break;
                loopBody << line << "\n";
            }
            std::vector<std::string> loop_cmds = run_loop(loopCount, loopBody.str());
            for (const auto &cmd : loop_cmds) { // push the commands run by loop to the parsed_commands for this script
                parsed_commands.push_back(cmd);
            }
            continue;
        }

        // Otherwise, parse line normally
        std::vector<std::string> line_cmds = parse_line(QString::fromStdString(line));
        for (const auto &cmd : line_cmds) { // push the commands run by line to the parsed_commands for this script
            parsed_commands.push_back(cmd);
        }
    }
    return parsed_commands;
}

std::vector<std::string> Parser::run_loop(int count, const std::string& loop_body) {
    std::vector<std::string> parsed_commands; // Vector of commands that were parsed correctly and actually run

    for (int i = 0; i < count; ++i) {
        std::istringstream loopStream(loop_body); // reset the loop stream
        std::string line;
        while (std::getline(loopStream, line)) {
            std::vector<std::string> loop_cmds = parse_line(QString::fromStdString(line));
            for (const auto &cmd : loop_cmds) { // push the commands run by line to the parsed_commands for this loop
                parsed_commands.push_back(cmd);
            }
        }
    }
    return parsed_commands;
}

// Private helper function that tries to change ALL variable names in the command into their values 
// (used in parse_line for each command)
// Returns the command string with float-values substituted into it
std::string Parser::varname_to_value(const std::string& command) {
    std::string ret; // the transformed command-string that is returned by this function
    std::regex var_regex(R"(\(([^\)]+)\))"); // variables occur inside parentheses
    std::smatch match;
    
    std::string::const_iterator search_start = command.cbegin();
    while (std::regex_search(search_start, command.cend(), match, var_regex)) {
        ret.append(search_start, match.prefix().second); // The command-string up to the match
        std::string contents = match[1]; // text inside brackets
        std::istringstream var_stream(contents);

        ret.append("(");
        // Go through all comma-separated variables
        bool first_var = true; std::string var_name;
        while (std::getline(var_stream, var_name, ',')){
            std::string var_value = var_name; // if value is not found just use the var_name as is... parse_line will encounter the error later

            // handle possible minus sign in front of variable name
            int sign = 1;
            if (!var_name.empty() && var_name[0] == '-') { sign = -1; var_name = var_name.substr(1); }

            // search vars for value
            auto it = vars.find(var_name);
            if (it != vars.end()) { var_value = std::to_string(sign*(it->second)); } // Variable has a value defined -> sub it in

            if (!first_var) { ret.append(","); }
            ret.append(var_value);
            first_var = false;
        }
        ret.append(")");

        search_start = match.suffix().first;
    }
    ret.append(search_start, command.cend()); // Add remaining command-string

    // check for variable assignment command, eg. for a=b, try to sub in value of b:
    std::regex varassign_regex(R"(^\s*([a-zA-Z_]\w*)\s*=\s*([a-zA-Z_]\w*)\s*$)");
    if (std::regex_match(ret, match, varassign_regex)) {
        std::string assignee_var = match[1];
        std::string var_name = match[2];
        auto it = vars.find(var_name);
        if (it != vars.end()) { ret = assignee_var + "=" + std::to_string(it->second); }
    }

    // whitespace MAY find its way into ret somehow... Erase whitespace here too
    ret.erase(std::remove_if(ret.begin(), ret.end(), ::isspace), ret.end());
    return ret;
}

// Parses lines of commands from CLI or from script
std::vector<std::string> Parser::parse_line(const QString& inputQ) {
    std::string input = inputQ.toStdString();
    std::istringstream input_stream(input);
    std::vector<std::string> parsed_commands; // Vector of commands that were parsed correctly and actually run

    //////// REGEX DEFINITIONS ////////
    
    // Regex for single-line loops //
    std::regex cli_loop_regex(R"(^\s*LOOP(\d+)\s*\{\s*(.*?)\s*\}\s*$)");
    
    // Regex for variable handling //
    std::regex vardef_regex(R"(^\s*([a-zA-Z_]\w*)\s*=\s*(-?\d+(?:\.\d+)?)\s*$)");
    std::regex varadd_regex(R"(^\s*([a-zA-Z_]\w*)\s*=\s*add\(\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\)\s*$)");
    std::regex varmul_regex(R"(^\s*([a-zA-Z_]\w*)\s*=\s*mul\(\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\)\s*$)");

    // Regex for each of the basic commands/functions:
    
    // Nullary functions (can be called either as up or up(), brackets are optional)
    std::regex up_regex(R"(^\s*up\s*\(?\)?\s*$)");
    std::regex down_regex(R"(^\s*down\s*\(?\)?\s*$)");

    // Unitary functions (single float as argument)
    std::regex forward_regex(R"(^\s*forward\s*\(\s*(-?\d*\.?\d+)\s*\)\s*$)");
    std::regex turn_regex(R"(^\s*turn\s*\(\s*(-?\d*\.?\d+)\s*\)\s*$)");
    std::regex setrot_regex(R"(^\s*setrot\s*\(\s*(-?\d*\.?\d+)\s*\)\s*$)");
    std::regex setspeed_regex(R"(^\s*setspeed\s*\(\s*(-?\d*\.?\d+)\s*\)\s*$)");
    std::regex setsize_regex(R"(^\s*setsize\s*\(\s*(-?\d*\.?\d+)\s*\)\s*$)");

    // Binary functions (2 floats)
    std::regex setpos_regex(R"(^\s*setpos\s*\(\s*(-?\d*\.?\d+)\s*,\s*(-?\d*\.?\d+)\s*\)\s*$)");
    std::regex arc_regex(R"(^\s*arc\s*\(\s*(-?\d*\.?\d+)\s*,\s*(-?\d*\.?\d+)\s*\)\s*$)");

    // Tertiary functions (3 floats) 
    std::regex setcolor_regex(R"(^\s*setcolor\s*\(\s*(-?\d*\.?\d+)\s*,\s*(-?\d*\.?\d+)\s*,\s*(-?\d*\.?\d+)\s*\)\s*$)"); 

    //////// START PARSING LINE ////////
    std::string command;
    std::getline(input_stream, command);
    
    // Remove ALL whitespace from command
    command.erase(std::remove_if(command.begin(), command.end(), ::isspace), command.end());

    std::smatch match; // regex-match var
    
    // Single-line loops needs to be handled here before the main while-loop below
    // because possible ';'-characters in the loop would sunder the loop-body into separate commands
    if (std::regex_match(command, match, cli_loop_regex)) {
        int loop_count = std::stoi(match[1]);
        std::string loop_body = match[2];
        std::cout << "matched loop:" << loop_body << std::endl;
        std::vector<std::string> loop_cmds = run_loop(loop_count, loop_body);
        for (const auto &cmd : loop_cmds) { // push the commands run by line to the parsed_commands for this line
            parsed_commands.push_back(cmd);
        }
        return parsed_commands;
    }

    input_stream.str(input); // reset the input_stream after failing to match single-line loop above
    input_stream.clear();

    // Multiple cmds can be input on a single line by delimiting with ';'
    while (std::getline(input_stream, command, ';')) {

        // Wait for animation to be done
        while (!movement_done){
            QCoreApplication::processEvents(); // this is required to process the signal to Parser
        }
        // Now execute the loop normally:

        // Attempt to change any variable names in the command into the corresponding float-values
        command = varname_to_value(command);

        // Check if command calls a script-defined function
        std::regex general_function_regex(R"((\w+)\(([-+]?\d*\.?\d+)?\))"); std::smatch fmatch;
                
        if (std::regex_match(command, fmatch, general_function_regex)) {
            std::string function_name = fmatch[1]; std::string argument = fmatch[2];

            // If the function is defined by user
            if (funcs.find(function_name) != funcs.end()) {
                // Define a variable for use as the function argument (if the argument is defined and not "")
                if (argument != ""){ vars[func_arg_names[function_name]] = std::stof(argument); }

                std::istringstream func_body_stream(funcs[function_name]);
                std::string fline;

                // Go through each line of the function body and parse_line to run it
                while (std::getline(func_body_stream, fline)) { parse_line(QString::fromStdString(fline)); }
                continue; // move on to the next command after function "call"
            }
        }

        // push the command to the command history (this is undone below if an error is encountered) 
        parsed_commands.push_back(command);

        // Turtle movement commands
        if (std::regex_match(command, match, forward_regex)) {
            float distance = std::stof(match[1].str());
            emit forward(distance);
            movement_done = false; // start waiting for the forward-animation to be done

        } else if (std::regex_match(command, match, turn_regex)) {
            float angle = std::stof(match[1].str());
            emit turn(angle);

        } else if (std::regex_match(command, match, setrot_regex)) {
            float rot = std::stof(match[1].str());
            emit setrot(rot);

        } else if (std::regex_match(command, match, setpos_regex)) {
            float x = std::stof(match[1].str()); float y = std::stof(match[2].str());
            QPointF pos(x, y);
            emit setpos(pos);

        } else if (std::regex_match(command, match, arc_regex)) {
            float radius = std::stof(match[1].str()); float angle = std::stof(match[2].str());
            emit arc(radius, angle);

        // Pen commands (up, down, etc.)  
        } else if (std::regex_match(command, up_regex)) {
            emit up();

        } else if (std::regex_match(command, down_regex)) {
            emit down();
        
        } else if (std::regex_match(command, match, setsize_regex)) {
            float size = std::stof(match[1].str());
            emit setsize(size);

        // Other commands
        } else if (std::regex_match(command, match, setspeed_regex)) {
            float speed = std::stof(match[1].str());
            emit setspeed(speed);

        } else if (std::regex_match(command, match, setcolor_regex)) {
            int r = static_cast<int>(std::floor(std::abs(std::stof(match[1].str()))));
            int g = static_cast<int>(std::floor(std::abs(std::stof(match[2].str()))));
            int b = static_cast<int>(std::floor(std::abs(std::stof(match[3].str()))));
            QColor color(r, g, b);
            emit setcolor(color);
        
        } else if (std::regex_match(command, match, vardef_regex)) {
            std::string var_name = match[1]; float value = std::stof(match[2]);
            vars[var_name] = value;

        } else if (std::regex_match(command, match, varadd_regex)) {
            std::string var_name = match[1];
            float value1 = std::stof(match[2]); float value2 = std::stof(match[3]);
            float result = value1 + value2; vars[var_name] = result;

        } else if (std::regex_match(command, match, varmul_regex)) {
            std::string var_name = match[1];
            float value1 = std::stof(match[2]); float value2 = std::stof(match[3]);
            float result = value1 * value2; vars[var_name] = result;

        } else {
            parsed_commands.pop_back(); // remove the erroneous command from history
            std::cout << "Parser failed to match the given input to a valid command\n";
        }
    }
    return parsed_commands;
}
