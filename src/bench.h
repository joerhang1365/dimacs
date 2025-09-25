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

//static gate_type string_to_type(const std::string& str);
//static std::string type_to_string(const gate_type& type);
//static std::string parse_pin_name(const std::string& str);

#endif