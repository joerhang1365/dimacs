// cnf.h
//

#ifndef CNF_H_
#define CNF_H_

#include <vector>
#include <string>
#include <unordered_map>
#include "bench.h"

class cnf {
public:
    cnf();
    int transform(const logic_gates& lg);
    void print_dimacs_file(const std::string& filename) const;

private:
    std::unordered_map<std::string, int> var_map;
    std::vector<std::vector<int>> clauses;
    int var_cnt;

    int get_var(const std::string& name);
};

#endif // CNF_H_