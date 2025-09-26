#include <fstream>
#include <iostream>
#include <algorithm>
#include "cnf.h"

cnf::cnf()
{
    var_cnt = 0;
}

int cnf::get_var(const std::string& name) 
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

// assign each gate primary input, primary output, and gate output to a unique
// variable. The transformations for each gate are hard coded in. Maybe there is
// a more generic way to do this using trees but idk how rn.

// Tseitin transformations:
// AND gate:  (C = A & B)    => (~A | ~B | C) & (A | ~C) & (B | ~C)
// NAND gate: (C = ~(A | B)) => (~A | ~B | ~C) & (A | C) & (B | C)
// OR  gate:  (C = A | B)    => (A | B | ~C) & (~A | C) & (~B | C)
// NOR gate:  (C = ~(A & B)) => (A | B | C) & (~A | ~C) & (~B | ~C)
// NOT gate:  (C = ~A)       => (~A | ~C) & (A | C)
// XOR gate:  (C = A ^ B)    => (~A | ~B | ~C) & (A | B | ~C) & (A | ~B | C) & (~A | B | C)
// BUFF gate: (C = A)        => (~A | C) & (A | ~C)

// One optimization I can do is build the final long clause with all the 
// variables for each gate while doing to pair input and output clauses
// this makes it harder to read tho so will keep it seperate for now
int cnf::transform(const logic_gates& lg) 
{
    // convert each gate to CNF clauses

    for (const auto& gate : lg.gates) 
    {
        int output_var = get_var(gate.output);

        if (gate.type == gate_type::AND)
        {
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clauses.push_back({input_var, -output_var});
            }

            std::vector<int> clause;
            clause.push_back(output_var);
        
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clause.push_back(-input_var);
            }

            clauses.push_back(clause);
        }
        else if (gate.type == gate_type::NAND)
        {
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clauses.push_back({input_var, output_var});
            }

            std::vector<int> clause;
            clause.push_back(-output_var);

            for (const auto& input: gate.inputs)
            {
                int input_var = get_var(input);
                clause.push_back(-input_var);
            }

            clauses.push_back(clause);
        }
        else if (gate.type == gate_type::OR)
        {
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clauses.push_back({-input_var, output_var});
            }

            std::vector<int> clause;
            clause.push_back(-output_var);
        
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clause.push_back(input_var);
            }

            clauses.push_back(clause);
        }
        else if (gate.type == gate_type::NOR)
        {
            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clauses.push_back({-input_var, -output_var});
            }

            std::vector<int> clause;
            clause.push_back(output_var);

            for (const auto& input : gate.inputs)
            {
                int input_var = get_var(input);
                clause.push_back(input_var);
            }

            clauses.push_back(clause);
        }
        else if (gate.type == gate_type::NOT)
        {
            // TODO: might want to verify that there is only one input
            int input_var = get_var(gate.inputs[0]);

            clauses.push_back({-input_var, -output_var});
            clauses.push_back({input_var, output_var});
        }
        else if (gate.type == gate_type::XOR)
        {
            // FIXME: assume gate has exactly two inputs
       
            int output_var = get_var(gate.output);
            int input_var1 = get_var(gate.inputs[0]);
            int input_var2 = get_var(gate.inputs[1]);

            clauses.push_back({-input_var1, -input_var2, -output_var});
            clauses.push_back({input_var1, input_var2, -output_var});
            clauses.push_back({input_var1, -input_var2, output_var});
            clauses.push_back({-input_var1, input_var2, output_var});
        } 
        else if (gate.type == gate_type::BUFF)
        {
            // TODO: check if only one input too
            int input_var = get_var(gate.inputs[0]);

            clauses.push_back({-input_var, output_var});
            clauses.push_back({input_var, -output_var});
        }
    }

    // end with clause of gate output being true
    // idk why but discrete math shit
    clauses.push_back({get_var(lg.primary_outputs[0])});

    return 0;
}

void cnf::print_dimacs_file(const std::string& filename) const 
{
    // incase someone does not input a filename
    std::string new_filename = filename;

    if (filename.empty())
    {
        new_filename = "output.dimacs";
    }

    std::ofstream dimacs_file;
    dimacs_file.open(new_filename);

    if (!dimacs_file.is_open()) 
    {
        std::cout << "ERROR: cannot not open file " << filename << std::endl;
        return;
    }

    // write header
    dimacs_file << "p cnf " << var_cnt << " " << clauses.size() << "\n";

    for (const auto& clause : clauses) 
    {
        for (const auto& var : clause) 
        {
            dimacs_file << var << " ";
        }

        // dimacs format ends each clause with a 0
        dimacs_file << "0\n";
    }

    dimacs_file.close();
}