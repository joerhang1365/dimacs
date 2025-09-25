#include <iostream>
#include <string>
#include "bench.h"
#include "miter.h"
#include "cnf.h"

int main(int argc, char * argv[])
{
    if (argc != 4)
    {
        std::cout << "ERROR: invalid number of inputs" << std::endl;
        return 1;
    }

    std::string circuit_a_file = argv[1];
    std::string circuit_b_file = argv[2];
    std::string output_file = argv[3];

    logic_gates circuit_a;
    logic_gates circuit_b;
    logic_gates miter;

    cnf cnf;

    if (circuit_a.parse_bench(circuit_a_file) != 0)
    {
        std::cout << "ERROR: failed to parse circuit a" << std::endl;
        return 1;
    }

    circuit_a.print_bench();

    if (circuit_b.parse_bench(circuit_b_file) != 0)
    {
        std::cout << "ERROR: failed to parse circuit b" << std::endl;
        return 1;
    }

    circuit_b.print_bench();
    miter = miter_structure(circuit_a, circuit_b);
    miter.print_bench();

    cnf.transform(miter);
    cnf.print_cnt(output_file);

    return 0;
}