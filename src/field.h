#pragma once

#include "types.h"
#include "target.h"

class Field{
private:
    Target* V;
    unsigned int _size;

public:
    Field();
    Field(const std::string &filename);
    void compute_graph(const int & r_capt, const int& r_comm);

    ~Field();

    Target* operator[](const int &i) const{return V+i;}
    Target* get_leaf() const;
    bool is_everyone_capted() const;
    bool is_communicating() const;
    void check_solution_is_ok() const;
    unsigned int size() const{return _size;}
    int cost() const;
    void write_solution(const std::string &filename) const;
    vector<Target*> get_triangle() const;

};