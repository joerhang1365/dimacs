#include <iostream>
#include <string>
#include "parse_bench.h"

int main(int argc, char * argv[])
{
    if (argc != 4)
    {
        std::cerr << "ERROR: invalid number of inputs" << std::endl;
        return 1;
    }

    std::string circuit_a_file = argv[1];
    std::string circuit_b_file = argv[2];
    std::string output_file = argv[3];

    circuit circuit_a;
    circuit circuit_b;

    std::cout << "Parsing circuit A from " << circuit_a_file << std::endl;

    if (circuit_a.parse_bench(circuit_a_file) != 0)
    {
        std::cerr << "ERROR: failed to parse circuit A" << std::endl;
        return 1;
    }

    std::cout << "Parsing circuit B from " << circuit_b_file << std::endl;

    if (circuit_b.parse_bench(circuit_b_file) != 0)
    {
        std::cerr << "ERROR: failed to parse circuit B" << std::endl;
        return 1;
    }

    circuit_a.print_bench();
    circuit_b.print_bench();

    return 0;
}