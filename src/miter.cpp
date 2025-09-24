#include "miter.h"
#include <iostream>
#include <algorithm>

logic_gates create_miter(const logic_gates& a, const logic_gates& b)
{
    // check that both circuits have the same primary input names
    if (a.primary_inputs != b.primary_inputs)
    {
        std::cerr << "Error: circuits have different primary inputs" << std::endl;
        return logic_gates();
    }

    // check that both circuits have the same number of primary outputs
    if (a.primary_outputs != b.primary_outputs) 
    {
        std::cerr << "Error: circuits have different number of primary outputs" << std::endl;
        return logic_gates();
    }

    logic_gates miter;

    // copy primary inputs from both circuits
    miter.primary_inputs = a.primary_inputs;

    // change names of gate inputs and outputs to avoid conflicts

    for (auto& a_gate : a.gates)
    {
        gate new_gate = a_gate;
        new_gate.output = "a_" + new_gate.output;

        // check if gate input is a primary input
        // if so, do not change its name
        for (auto& input : new_gate.inputs)
        {
            if (std::find(a.primary_inputs.begin(), a.primary_inputs.end(), input) == a.primary_inputs.end())
            {
                input = "a_" + input;
            }
        }

        miter.gates.push_back(new_gate);
    }

    for (auto& b_gate : b.gates)
    {
        gate new_gate = b_gate;
        new_gate.output = "b_" + new_gate.output;

        // check if gate input is a primary input
        // if so, do not change its name
        for (auto& input : new_gate.inputs)
        {
            if (std::find(b.primary_inputs.begin(), b.primary_inputs.end(), input) == b.primary_inputs.end())
            {
                input = "b_" + input;
            }
        }

        miter.gates.push_back(new_gate);
    }

    // create XOR gates for each output pair
    // FIXME: outputs might not be at the same index of each circuit
    // so might need to search for the name instead
    
    std::vector<std::string> xor_outputs;

    for (size_t i = 0; i < a.primary_outputs.size(); i++) 
    {
        std::string xor_gate_name;
        gate xor_gate;

        xor_gate_name = "xor_out_" + std::to_string(i);

        xor_gate.type = gate_type::XOR;
        xor_gate.output = xor_gate_name;
        xor_gate.inputs.push_back("a_" + a.primary_outputs[i]);
        xor_gate.inputs.push_back("b_" + b.primary_outputs[i]);

        xor_outputs.push_back(xor_gate_name);
        miter.gates.push_back(xor_gate);
    }

    // create OR gate to combine all XOR outputs

    gate or_gate;

    or_gate.type = gate_type::OR;
    or_gate.output = "miter_output";
    or_gate.inputs = xor_outputs;

    miter.gates.push_back(or_gate);
    miter.primary_outputs.push_back("miter_output");

    return miter;
}