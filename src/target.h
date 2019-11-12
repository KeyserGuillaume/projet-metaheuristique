#pragma once

#include "types.h"
#include <iostream>

class Target{
private:
    int id;
    int original_id;
    fpair coords;
    bool am_i_a_captor;
    std::vector<Target*> delta_capt, delta_comm, delta_2_capt, successors;

public:
    Target();
    Target(std::string input_line, const int &my_id);
    Target(int my_id, fpair my_coords);
    Target(int my_id);
    Target(const Target &S);

    void add_target_capt(Target* a);
    void add_target_2_capt(Target* a);
    void add_target_comm(Target* a);
    void make_captor();
    void unmake_captor();
    void add_successor(Target* a){successors.push_back(a);}
    void set_successors(std::vector<Target*> b){successors = b;}
    void clear_successors(){successors.clear();}

    int get_id() const{return id;}
    int get_original_id() const{return original_id;}
    fpair get_coords() const{return coords;}
    bool is_captor() const{return am_i_a_captor;}
    bool has_any_uniquely_capted_targets() const;
    bool is_capted() const;
    std::vector<Target*> get_delta_capt() const{return delta_capt;}
    std::vector<Target*> get_delta_2_capt() const{return delta_2_capt;}
    std::vector<Target*> get_delta_comm() const{return delta_comm;}
    std::vector<Target*> get_successors() const{return successors;}
    std::vector<Target*> get_uniquely_capted_targets() const;
    int get_nb_of_captors() const;

    float distance(const Target& v) const;

};


std::ostream& operator<<(std::ostream& str, const Target& s);