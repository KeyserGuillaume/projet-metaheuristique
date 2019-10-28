#pragma once
#include "types.h"
#include "field.h"
#include "cost_functions.h"
#include <sstream>
#include <fstream>
#include <math.h>


class LocalSearch{
private:
    long k;
    Field* F;
    vector_int current_solution;
    CostFunction* cost_computer;
    void init();
    void remove_if_useless_captor(const int& i);
    void remove_captor_from_solution(const int &i);

public:
    bool verbose;
    bool keep_writing=true;
    LocalSearch(Field* my_f, CostFunction* my_cost);

    void run(const long &nb_iteration, const int &period_display);
    void jump();
    void flea_move(const int &id);
    void caterpillar_move(const int &id);
    void move_solution(const int& u, const int& v);

    int current_value() const {return current_solution.size();}
    int current_cost() const;
    void check_solution_is_ok(const bool& only_inner_consistency=false) const;
    void write_solution(const std::string &filename) const;
    void stats() const;
    void stats(CostFunction *myCost) const;
    void display() const{
        cout << k << " " << current_value() << endl;
    }
    void display_with_cost_function() const{
        cout << k << " " << current_value() << " " << current_cost() << endl;
    }

};

int nb_chiffres(int i);
void write_solution_to_file(std::string filename, vector<int> solution);
