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

    logic_gates circuit_a;
    logic_gates circuit_b;
    logic_gates miter;

    cnf cnf;

    if (circuit_a.parse_bench_file(argv[1]) != 0)
    {
        std::cout << "ERROR: failed to parse circuit a" << std::endl;
        return 1;
    }

    //circuit_a.print();

    if (circuit_b.parse_bench_file(argv[2]) != 0)
    {
        std::cout << "ERROR: failed to parse circuit b" << std::endl;
        return 1;
    }

    //circuit_b.print();

    miter = miter_structure(circuit_a, circuit_b);
    //miter.print();

    cnf.transform(miter);
    cnf.print_dimacs_file(argv[3]);

    return 0;
}