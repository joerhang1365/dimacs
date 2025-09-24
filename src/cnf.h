#ifndef CNF_H_
#define CNF_H_

#include <vector>
#include <string>
#include <unordered_map>
#include "parse_bench.h"

class cnf {
public:
    cnf();
    void transform(const logic_gates& c, const std::string& filename);
    void print_cnf() const;
                         
private:
    int var_cnt;
    std::unordered_map<std::string, int> var_map;
    std::vector<std::vector<int>> clauses;

    int get_var_id(const std::string& name);
    void gate_to_cnf(const gate& g);
};

#endif