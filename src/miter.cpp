#include "miter.h"
#include <iostream>
#include <algorithm>

logic_gates miter_structure(const logic_gates& lg_a, const logic_gates& lg_b)
{
    // check that both circuits have the same primary inputs
    // of not return empty logic_gates structure
    if (lg_a.primary_inputs != lg_b.primary_inputs)
    {
        std::cout << "Error: circuits have different primary inputs" << std::endl;
        return logic_gates();
    }

    // check that both circuits have the same primary outputs
    if (lg_a.primary_outputs != lg_b.primary_outputs) 
    {
        std::cout << "Error: circuits have different number of primary outputs" << std::endl;
        return logic_gates();
    }

    logic_gates miter;

    // copy primary inputs
    // both circuits share the smae primary inputs
    miter.primary_inputs = lg_a.primary_inputs;

    // change names of gate inputs and outputs to avoid conflicts
    // add a prefix "a_" for circuit a and "b_" for circuit b

    for (size_t i = 0; i < lg_a.gates.size(); i++)
    {
        gate updated_gate = lg_a.gates[i];

        // rename gate output
        updated_gate.output = "a_" + lg_a.gates[i].output;

        // check if gate input is a primary input
        // if not change its name
        for (auto& input : updated_gate.inputs) 
        {
            auto it = std::find(lg_a.primary_inputs.begin(), lg_a.primary_inputs.end(), input);
            
            if (it == lg_a.primary_inputs.end())
            {
                input = "a_" + input;
            }
        }

        miter.gates.push_back(updated_gate);
    }

    for (size_t i = 0; i < lg_b.gates.size(); i++) 
    {
        gate updated_gate = lg_b.gates[i];

        // rename gate output
        updated_gate.output = "b_" + lg_b.gates[i].output;

        // check if gate input is a primary input
        // if not change its name
        for (auto& input : updated_gate.inputs) 
        {
            auto it = std::find(lg_b.primary_inputs.begin(), lg_b.primary_inputs.end(), input);
            
            if (it == lg_b.primary_inputs.end())
            {
                input = "b_" + input;
            }
        }

        miter.gates.push_back(updated_gate);
    }

    // create XOR gates for each circuits matching primary output
    // FIXME: outputs might not always be at the same index in each logic gate circuit
    // so might need to search for the name instead but works for now
    
    // save XOR gate outputs so we can connect them to the OR gate later
    std::vector<std::string> xor_outputs;

    for (size_t i = 0; i < lg_a.primary_outputs.size(); i++) 
    {
        std::string xor_gate_name;
        gate xor_gate;

        xor_gate_name = "xor_out_" + std::to_string(i);

        xor_gate.type = gate_type::XOR;
        xor_gate.output = xor_gate_name;
        xor_gate.inputs.push_back("a_" + lg_a.primary_outputs[i]);
        xor_gate.inputs.push_back("b_" + lg_b.primary_outputs[i]);

        xor_outputs.push_back(xor_gate_name);
        miter.gates.push_back(xor_gate);
    }

    // create OR gate and combine all XOR outputs

    gate or_gate;

    or_gate.type = gate_type::OR;
    or_gate.output = "miter_output";
    or_gate.inputs = xor_outputs;

    miter.gates.push_back(or_gate);
    miter.primary_outputs.push_back("miter_output");

    return miter;
}