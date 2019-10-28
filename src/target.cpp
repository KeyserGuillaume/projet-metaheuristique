#include "target.h"
#include <sstream>
#include <iostream>
#include <math.h>

Target::Target(){
    am_i_a_captor = false;
}

Target::Target(std::string input_line){
    std::stringstream line(input_line);
    std::string entity = "";
    float x, y;
    while (entity == ""){
        std::getline(line, entity, ' ');
    }
    id = std::stoi(entity);

    entity = "";
    while (entity == ""){
        std::getline(line, entity, ' ');
    }
    x = std::stof(entity);

    entity = "";
    while (entity == ""){
        std::getline(line, entity, ' ');
    }
    y = std::stof(entity);

    coords = fpair(x, y);
    am_i_a_captor = false;
}

Target::Target(const Target &t){
    id = t.id;
    coords = t.coords;
    am_i_a_captor = t.am_i_a_captor;
    delta_capt = t.delta_capt;
    delta_comm = t.delta_comm;
}

Target::Target(int my_id, fpair my_coords){
    id = my_id;
    coords = my_coords;
    am_i_a_captor = false;
}

void Target::add_target_capt(Target* a){
    delta_capt.push_back(a);
}

void Target::add_target_2_capt(Target* a){
    delta_2_capt.push_back(a);
}

void Target::add_target_comm(Target* a){
    delta_comm.push_back(a);
}

float Target::distance(const Target &v) const{
    return sqrt(pow(coords.first - v.coords.first, 2) + pow(coords.second - v.coords.second, 2));
}

int Target::get_nb_of_captors() const{
    int s = 0;
    for (unsigned int i = 0; i < delta_capt.size(); i++){
        if (delta_capt[i]->is_captor())
            s++;
    }
    return s;
}

bool Target::is_capted() const {
    // NB we say that a captor is necessarily capted (even if it is by itself)
    if (id == 0 || am_i_a_captor){
        return true;
    }
    for (unsigned int j = 0; j < delta_capt.size(); j++) {
        if (delta_capt[j]->is_captor())
            return true;
    }
    return false;
}

vector<Target *> Target::get_uniquely_capted_targets() const {
    vector<Target*> v1_essential = vector<Target*>(0);
    // find targets uniquely capted by this one, not captors themselves, not the well
    for (unsigned int i = 0; i < delta_capt.size(); i++){
        if (delta_capt[i]->get_id() != 0 &&
            !delta_capt[i]->is_captor() &&
            delta_capt[i]->get_nb_of_captors() == 1){
            v1_essential.push_back(delta_capt[i]);
        }
    }
    return v1_essential;
}

bool Target::has_any_uniquely_capted_targets() const {
    if (!am_i_a_captor){
        throw std::invalid_argument("Target::has_any_uniquely_capted_targets is undefined for non-captor targets");
    }
    vector<Target*> u_neighbors;
    bool is_uniquely_capted;
    for (unsigned int i = 0; i < delta_capt.size(); i++){
        if (!delta_capt[i]->is_captor() && delta_capt[i]->get_id() != 0){
            u_neighbors = delta_capt[i]->get_delta_capt();
            is_uniquely_capted = true;
            for (unsigned int j = 0; j < u_neighbors.size() && is_uniquely_capted; j++){
                if (u_neighbors[j]->is_captor() && u_neighbors[j]->get_id() != id)
                    is_uniquely_capted = false;
            }
            if (is_uniquely_capted)
                return true;
        }
    }
    return false;
}


ostream& operator<<(ostream& str, const Target& s){
    fpair coords = s.get_coords();
    str << "id: " << s.get_id() << "  x: " << coords.first << "   y: " << coords.second;
    str << "  (is " << ((s.is_captor()) ? "" : "not ") << "a captor) ";
    str << s.get_delta_capt().size() << " " << s.get_delta_comm().size();
    return str;
}
