#include <fstream>
#include <iostream>
#include <algorithm>
#include "cnf.h"

cnf::cnf() : var_cnt(0) {}

int cnf::get_var_id(const std::string& name) 
{
    auto it = var_map.find(name);
    
    // check if variable does not exist
    if (it == var_map.end()) 
    {
        var_map[name] = ++var_cnt;
    }

    return var_map[name];
}

// AND gate: (C = A & B) => (~A | ~B | C) & (A | ~C) & (B | ~C)
// OR  gate: (C = A | B) => (A | B | ~C)  & (~A | C) & (~B | C)
void cnf::gate_to_cnf(const gate& g)
{
    int output_var = get_var_id(g.output);

    if (g.type == gate_type::AND)
    {
        for (const auto& input : g.inputs)
        {
            int input_var = get_var_id(input);
            clauses.push_back({input_var, -output_var});
        }

        std::vector<int> clause;
        clause.push_back(output_var);
        
        for (const auto& input : g.inputs)
        {
            int input_var = get_var_id(input);
            clause.push_back(-input_var);
        }

        clauses.push_back(clause);
    }
    else if (g.type == gate_type::OR)
    {
        for (const auto& input : g.inputs)
        {
            int input_var = get_var_id(input);
            clauses.push_back({-input_var, output_var});
        }

        std::vector<int> clause;
        clause.push_back(-output_var);
        
        for (const auto& input : g.inputs)
        {
            int input_var = get_var_id(input);
            clause.push_back(input_var);
        }

        clauses.push_back(clause);
    }
    else if (g.type == gate_type::XOR)
    {
        // FIXME: assume gate has exactly two inputs
        if (g.inputs.size() != 2) 
        {
            std::cerr << "ERROR: XOR gate must have exactly two inputs" << std::endl;
            return;
        }

        int output_var = get_var_id(g.output);
        int input_var1 = get_var_id(g.inputs[0]);
        int input_var2 = get_var_id(g.inputs[1]);

        clauses.push_back({-input_var1, -input_var2, -output_var});
        clauses.push_back({input_var1, input_var2, -output_var});
        clauses.push_back({input_var1, -input_var2, output_var});
        clauses.push_back({-input_var1, input_var2, output_var});
    }
}

void cnf::transform(const logic_gates& c, const std::string& filename) 
{
    // convert each gate to CNF clauses
    for (const auto& g : c.gates) 
    {
        gate_to_cnf(g);
    }

    // end with clause of gate output being true
    clauses.push_back({get_var_id(c.primary_outputs[0])});

    // write to file in DIMACS format
    std::ofstream dimacs_file;
    dimacs_file.open(filename);
    
    if (!dimacs_file.is_open()) 
    {
        std::cerr << "ERROR: could not open file " << filename << " for writing" << std::endl;
        return;
    }

    // write header
    dimacs_file << "p cnf " << var_cnt << " " << clauses.size() << "\n";

    for (const auto& clause : clauses) 
    {
        for (const auto& literal : clause) 
        {
            dimacs_file << literal << " ";
        }
        dimacs_file << "0\n";
    }

    dimacs_file.close();
}