#include <fstream>
#include <iostream>
#include <algorithm>
#include "cnf.h"

cnf::cnf()
{
    var_cnt = 0;
}

int cnf::get_var_id(const std::string& name) 
{
    auto it = var_map.find(name);

    // check if variable is already mapped
    // if not assing a new id
    if (it == var_map.end())
    {
        var_map[name] = var_cnt + 1;
        var_cnt++;
    }

    return var_map[name];
}

// Tseitin transformation of gates
// AND gate:  (C = A & B)    => (~A | ~B | C) & (A | ~C) & (B | ~C)
// NAND gate: (C = ~(A | B)) => (~A | ~B | ~C) & (A | C) & (B | C)
// OR  gate:  (C = A | B)    => (A | B | ~C) & (~A | C) & (~B | C)
// NOR gate:  (C = ~(A & B)) => (A | B | C) & (~A | ~C) & (~B | ~C)
// NOT gate:  (C = ~A)       => (~A | ~C) & (A | C)
// XOR gate:  (C = A ^ B)    => (~A | ~B | ~C) & (A | B | ~C) & (A | ~B | C) & (~A | B | C)
// BUFF gate: (C = A)        => (~A | C) & (A | ~C)
// One optimization I can do is build the final long clause for with all the 
// variables for each gate while doing to pair input and output clauses
// this makes it harder to read tho so will keep it seperate for now
void cnf::transform(const logic_gates& lg) 
{
    // convert each gate to CNF clauses

    for (const auto& g : lg.gates) 
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
        else if (g.type == gate_type::NAND)
        {
            for (const auto& input : g.inputs)
            {
                int input_var = get_var_id(input);
                clauses.push_back({input_var, output_var});
            }

            std::vector<int> clause;
            clause.push_back(-output_var);

            for (const auto& input: g.inputs)
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
        else if (g.type == gate_type::NOR)
        {
            for (const auto& input : g.inputs)
            {
                int input_var = get_var_id(input);
                clauses.push_back({-input_var, -output_var});
            }

            std::vector<int> clause;
            clause.push_back(output_var);

            for (const auto& input : g.inputs)
            {
                int input_var = get_var_id(input);
                clause.push_back(input_var);
            }

            clauses.push_back(clause);
        }
        else if (g.type == gate_type::NOT)
        {
            // TODO: might want to verify that there is only one input
            int input_var = get_var_id(g.inputs[0]);

            clauses.push_back({-input_var, -output_var});
            clauses.push_back({input_var, output_var});
        }
        else if (g.type == gate_type::XOR)
        {
            // FIXME: assume gate has exactly two inputs
       
            int output_var = get_var_id(g.output);
            int input_var1 = get_var_id(g.inputs[0]);
            int input_var2 = get_var_id(g.inputs[1]);

            clauses.push_back({-input_var1, -input_var2, -output_var});
            clauses.push_back({input_var1, input_var2, -output_var});
            clauses.push_back({input_var1, -input_var2, output_var});
            clauses.push_back({-input_var1, input_var2, output_var});
        } 
        else if (g.type == gate_type::BUFF)
        {
            // TODO: check if only one input too
            int input_var = get_var_id(g.inputs[0]);

            clauses.push_back({-input_var, output_var});
            clauses.push_back({input_var, -output_var});
        }
    }

    // end with clause of gate output being true
    // idk why but discrete math shit
    clauses.push_back({get_var_id(lg.primary_outputs[0])});
}

void cnf::print_cnt(const std::string& filename) const 
{
    std::ofstream dimacs_file;
    dimacs_file.open(filename);

    if (!dimacs_file.is_open()) 
    {
        std::cout << "ERROR: cannot not open file " << filename << std::endl;
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

        // dimacs format ends each clause with a 0
        dimacs_file << "0\n";
    }

    dimacs_file.close();
}