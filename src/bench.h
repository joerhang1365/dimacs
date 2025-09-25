#ifndef BENCH_H_
#define BENCH_H_

#include <string>
#include <vector>

enum class gate_type { AND, NAND, OR, NOR, NOT, XOR, BUFF, UNKNOWN };

struct gate {
    gate_type type;
    std::string output;
    std::vector<std::string> inputs;
};

class logic_gates {
public:
    std::vector<std::string> primary_inputs;
    std::vector<std::string> primary_outputs;
    std::vector<gate> gates;

    int parse_bench(const std::string& filename);
    void print_bench() const;

private:
};

#endif