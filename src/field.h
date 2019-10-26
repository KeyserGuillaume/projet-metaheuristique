#pragma once

#include "types.h"
#include "target.h"

class Field{
private:
    Target* V;
    unsigned int _size;

public:
    Field(const std::string &filename);
    void compute_graph(const int & r_capt, const int& r_comm);

    ~Field();

    Target* operator[](const int &i) const{return V+i;}
    bool is_communicating() const;
    void check_solution_is_ok() const;
    int size() const{return _size;}
    int cost() const;

};