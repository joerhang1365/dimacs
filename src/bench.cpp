#include "bench.h"
#include <fstream>
#include <iostream>
#include <string.h>

static gate_type string_to_type(const std::string& str);
static std::string type_to_string(const gate_type& type);
static std::string parse_pin_name(const std::string& str);

gate_type string_to_type(const std::string& str)
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

std::string type_to_string(const gate_type& type)
{
    switch (type)
    {
    case gate_type::AND: 
        return "AND";
    case gate_type::NAND: 
        return "NAND";
    case gate_type::OR: 
        return "OR";
    case gate_type::NOR: 
        return "NOR";
    case gate_type::NOT: 
        return "NOT";
    case gate_type::XOR: 
        return "XOR";
    case gate_type::BUFF: 
        return "BUFF";
    default: 
        return "UNKNOWN";
    }
}

std::string parse_pin_name(const std::string& str)
{
    size_t start = str.find('(');
    size_t end = str.find(')');

    // get the pin name for INPUT(pin_name) or OUTPUT(pin_name)
    if (start != std::string::npos && end != std::string::npos && start < end)
    {
        return str.substr(start + 1, end - start - 1);
    }

    return "";
}

int logic_gates::parse_bench(const std::string& filename)
{
    std::ifstream file;
    
    file.open(filename);

    if (!file.is_open())
    {
        std::cout << "ERROR: cannot open file " << filename << std::endl;
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

            // turn std::string into char * because strtok is honestly easier
            // use a seperate string so I can save the original pointer to free later
            char * str = new char[line.size() + 1];
            strcpy(str, line.c_str());

            // parse the output pin name

            char * token = strtok(str, " =");

            if (token == nullptr)
            {
                std::cout << "ERROR: cannot find gate output in line: " << line << std::endl;
                delete[] str; // gotta free the memory
                continue;
            }

            gate.output = std::string(token);
            
            // parse gate type and inputs

            token = strtok(nullptr, "= (");

            if (token == nullptr)
            {
                std::cout << "ERROR: cannot find gate type in line: " << line << std::endl;
                delete[] str;
                continue;
            }

            std::string type_str = std::string(token);
            gate.type = string_to_type(type_str);
            
            // parse a variable number of inputs
            
            token = strtok(nullptr, ")");

            while (token != nullptr)
            {
                std::string input = std::string(token);
                gate.inputs.push_back(input);
                token = strtok(nullptr, ", ");
            }

            if (gate.inputs.empty())
            {
                std::cout << "ERROR: gate has no inputs in line: " << line << std::endl;
                delete[] str;
                continue;
            }

            delete[] str;

            gates.push_back(gate);
        }
    }

    file.close();

    return 0;
}

void logic_gates::print_bench() const
{
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
        std::cout << type_to_string(gate.type);
        std::cout << "(";

        for (size_t i = 0; i < gate.inputs.size(); i++)
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