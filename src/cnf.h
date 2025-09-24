#ifndef CNF_H_
#define CNF_H_

#include <vector>
#include <string>
#include <unordered_map>
#include "parse_bench.h"

class cnf {
public:
    cnf();
    int transform_to_cnf(const circuit& a, const circuit& b,
                         const std::string &output_filename);
                         
private:
    int var_count;
    std::unordered_map<std::string, int> var_map;
    std::vector<std::vector<int>> clauses;

    int get_var(const std::string& name);
    void add_clause(const std::vector<int>& clause);
    void write_cnf(const std::string& filename);
};

#endif