#include "parse_bench.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

inline std::string trim(const std::string& s) 
{
    auto start = std::find_if_not(s.begin(), s.end(), ::isspace);
    auto end = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    return (start < end) ? std::string(start, end) : "";
}

gate_type circuit::string_to_gate_type(const std::string& str)
{
    if (str == "AND")
    {
        return gate_type::AND;
    }
    else if (str == "NAND")
    { 
        return gate_type::NAND;
    }
    else if (str == "OR")
    {
        return gate_type::OR;
    }
    else if (str == "NOR") 
    {
        return gate_type::NOR;
    }
    else if (str == "NOT") 
    {
        return gate_type::NOT;
    }
    else if (str == "XOR") 
    {
        return gate_type::XOR;
    }
    else if (str == "BUFF") 
    {
        return gate_type::BUFF;
    }
    else 
    {
        return gate_type::UNKNOWN;
    }
}   

std::string circuit::parse_pin_name(const std::string& str)
{
    size_t start = str.find('(');
    size_t end = str.find(')');

    if (start != std::string::npos && end != std::string::npos && start < end)
    {
        return str.substr(start + 1, end - start - 1);
    }

    return "";
}

int circuit::parse_bench(const std::string& filename)
{
    std::ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(file, line))
    {
        // skip empty lines and comments
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        if (line.substr(0, 5) == "INPUT")
        {
            std::string input = parse_pin_name(line);
            primary_inputs.push_back(input);
        }
        else if (line.substr(0, 6) == "OUTPUT")
        {
            std::string output = parse_pin_name(line);
            primary_outputs.push_back(output);
        }
        else if (line.find('=') != std::string::npos)
        {
            // gate format: output_pin = GATE(input1, input2, ...)
            gate gate;

            // output pin
            size_t equal_pos = line.find('=');
            
            if (equal_pos == std::string::npos)
            {
                continue;
            }

            gate.output = line.substr(0, equal_pos);
            gate.output = trim(gate.output);
            
            // gate type and inputs
            size_t left_parenthesis = line.find('(', equal_pos);
            size_t right_parenthesis = line.find(')', equal_pos);

            if (left_parenthesis == std::string::npos || right_parenthesis == std::string::npos || 
                left_parenthesis >= right_parenthesis)
            {
                continue;
            }

            std::string type_str = line.substr(equal_pos + 1, left_parenthesis - equal_pos - 1);
            type_str = trim(type_str);
            gate.type = string_to_gate_type(type_str);
            
            // inputs
            std::string inputs_str = line.substr(left_parenthesis + 1, right_parenthesis - left_parenthesis - 1);
            std::istringstream iss(inputs_str);
            std::string input;

            while (std::getline(iss, input, ','))
            {
                gate.inputs.push_back(trim(input));
            }

            gates.push_back(gate);
        }
    }

    file.close();

    return 0;
}

void circuit::print_bench() const
{
    std::cout <<"*** Circuit ***\n";
    for (const auto& input : primary_inputs)
    {
        std::cout << "INPUT(" << input << ")\n";
    }

    std::cout << "\n";

    for (const auto& output : primary_outputs)
    {
        std::cout << "OUTPUT(" << output << ")\n";
    }

    std::cout << "\n";

    for (const auto& gate : gates)
    {
        std::cout << gate.output << " = ";

        switch (gate.type)
        {
        case gate_type::AND: 
            std::cout << "AND"; 
            break;
        case gate_type::NAND: 
            std::cout << "NAND"; 
            break;
        case gate_type::OR: 
            std::cout << "OR"; 
            break;
        case gate_type::NOR: 
            std::cout << "NOR"; 
            break;
        case gate_type::NOT: 
            std::cout << "NOT"; 
            break;
        case gate_type::XOR: 
            std::cout << "XOR"; 
            break;
        case gate_type::BUFF: 
            std::cout << "BUFF"; 
            break;
        default: 
            std::cout << "UNKNOWN"; 
            break;
        }

        std::cout << "(";
        for (size_t i = 0; i < gate.inputs.size(); ++i)
        {
            std::cout << gate.inputs[i];
            if (i < gate.inputs.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
    }

    std::cout << "\n";
}