#include <fstream>
#include <iostream>
#include <algorithm>
#include "cnf.h"

cnf::cnf() : var_cnt(0) {}

int cnf::get_var_id(const std::string& name) 
{
    auto it = var_map.find(name);
    
    // check if variable does not exist
    if (it == var_map.end()) 
    {
        var_map[name] = ++var_cnt;
    }

    return var_map[name];
}


void cnf::gate_to_cnf(const gate& g)
{

}