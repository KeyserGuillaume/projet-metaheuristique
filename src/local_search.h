#pragma once
#include "types.h"
#include "field.h"
#include "cost_functions.h"
#include <sstream>
#include <fstream>
#include <math.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */


class LocalSearch{
private:
    long k;
    Field* F;
    CostFunction* cost_computer;
    void init();
    bool remove_if_useless_captor(const int &i, const bool &check_captation = true);
    void remove_captor_from_solution(const int &i);
    int nb_flea_jumps = 0;
    int nb_caterpillar_moves = 0;
    int nb_other_moves = 0;

public:
    bool verbose = false;
    LocalSearch(Field* my_f, CostFunction* my_cost);
    void set_cost_function(CostFunction* my_cost){cost_computer = my_cost;}

    void run_on_time_limit(const clock_t time_limit, const int &period_display);
    void run(const long &nb_iteration, const int &period_display);
    void jump();
    void flea_jump();
    void caterpillar_move(const int &id, const bool &repeat);
    void musical_chairs();
    void move_solution(const int& u, const int& v);

    int current_value() const {return F->get_nb_captors();}
    int current_cost() const;
    void check_solution_is_ok() const{F->check_solution_is_ok();}
    void write_solution(const std::string &filename) const;
    void stats() const;
    void display() const{
        std::cout << k << " " << current_value() << " " << nb_flea_jumps << " " << nb_caterpillar_moves << " " << nb_other_moves << std::endl;
    }

};

int nb_chiffres(int i);
void write_solution_to_file(std::string filename, std::vector<int> solution);
