#include "bench.h"
#include <fstream>
#include <sstream>
#include <iostream>

static gate_type string_to_type(const std::string& str);
static std::string type_to_string(const gate_type& type);

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
            std::string input;
            size_t start = line.find('(');
            size_t end = line.find(')');
            
            input = line.substr(start + 1, end - start - 1);
            primary_inputs.push_back(input);
        }
        else if (line.substr(0, 6) == "OUTPUT")
        {
            std::string output;
            size_t start = line.find('(');
            size_t end = line.find(')');
            
            output = line.substr(start + 1, end - start - 1);
            primary_outputs.push_back(output);
        }
        else if (line.find('=') != std::string::npos)
        {
            // gate format: output_pin = GATE(input1, input2, ...)

            gate gate;
    
            size_t equal = line.find('=');
            size_t left = line.find('(');
            size_t right = line.find(')');

            // parse output pin
            
            std::string output;
            output = line.substr(0, equal);

            // remove any trailing spaces
            output.erase(output.find_last_not_of(" \t") + 1, output.size());
            gate.output = output;

            // parse gate type

            std::string type;
            type = line.substr(equal + 1, left - equal - 1);

            // remove any trailing spaces
            type.erase(0, type.find_first_not_of(" \t"));
            type.erase(type.find_last_not_of(" \t") + 1);
            gate.type = string_to_type(type);

            // parse input pins

            std::string inputs;
            inputs = line.substr(left + 1, right - left - 1);

            std::stringstream ss;
            ss << inputs;
            std::string input;

            while (std::getline(ss, input, ','))
            {
                // remove both leading and trailing spaces
                input.erase(0, input.find_first_not_of(" \t"));
                input.erase(input.find_last_not_of(" \t") + 1);
                
                gate.inputs.push_back(input);
            }

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