#pragma once

#include "types.h"
#include <iostream>

using namespace std;

class Target{
private:
    int id;
    fpair coords;
    bool am_i_a_captor;
    std::vector<Target*> delta_capt, delta_comm, delta_2_capt;

public:
    Target();
    Target(std::string input_line);
    Target(int my_id, fpair my_coords);
    Target(const Target &S);

    void add_target_capt(Target* a);
    void add_target_2_capt(Target* a);
    void add_target_comm(Target* a);
    void make_captor(){if (am_i_a_captor) throw std::invalid_argument("already a captor"); am_i_a_captor = true;}
    //void make_captor(){am_i_a_captor = true;}
    void unmake_captor(){if (!am_i_a_captor) throw std::invalid_argument("not a captor");am_i_a_captor = false;}
    //void unmake_captor(){am_i_a_captor = false;}

    int get_id() const{return id;}
    fpair get_coords() const{return coords;}
    bool is_captor() const{return am_i_a_captor;}
    bool has_any_uniquely_capted_targets() const;
    bool is_capted() const;
    vector<Target*> get_delta_capt() const{return delta_capt;}
    vector<Target*> get_delta_2_capt() const{return delta_2_capt;}
    vector<Target*> get_delta_comm() const{return delta_comm;}
    vector<Target*> get_uniquely_capted_targets() const;
    int get_nb_of_captors() const;

    float distance(const Target& v) const;

};


ostream& operator<<(ostream& str, const Target& s);