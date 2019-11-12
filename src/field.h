#pragma once

#include "types.h"
#include "target.h"

typedef std::vector<std::pair<unsigned int, unsigned int> > vector_edge;

class Field{
private:
    Target* V;
    unsigned int _size;
    vector_int captors;

public:
    Field();
    Field(const std::string &filename);
    void compute_graph(const int & r_capt, const int& r_comm);
    void make_captor(const int &u);
    void unmake_captor(const int &u);

    ~Field();

    Target* operator[](const int &i) const{return V+i;}
    Target* get_captor(const int &i) const{return V+captors[i];}
    Target* get_random_captor() const{return V+captors[rand()%captors.size()];}
    vector_int get_potentially_useless_captors() const;
    std::vector<bool> detect_captor_usefulness() const;
    vector_int fill_edges_and_detect_kind_A(vector_edge &pure_edges_1,
                                            vector_edge &pure_edges_2,
                                            vector_edge &hybrid_edges,
                                            const std::vector<bool> &potential_captor_uselessness) const;
    bool is_there_any_useless_captor(vector_int &result) const;
    unsigned int get_nb_captors() const{return captors.size();}
    Target* get_leaf() const;
    bool is_everyone_capted() const;
    bool is_communicating() const;
    bool is_communicating_fast();
    void check_solution_is_ok() const;
    unsigned int size() const{return _size;}
    int cost() const;
    void write_solution(const std::string &filename) const;

};