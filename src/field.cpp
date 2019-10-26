#include "field.h"
#include <iostream>
#include <fstream>
#include <queue>

Field::Field(const std::string &filename){
    std::ifstream targetFile (filename.c_str());
    if (!targetFile.is_open()) throw std::runtime_error("No targets file found");
    std::string line;
    // put all the targets in a temporary vector
    vector<Target> tmp;
    while (std::getline(targetFile, line)){
        tmp.push_back(Target(line));
    }
    // copy them into the dynamic table
    // I chose to do this because at first I wanted V to be a vector
    // of targets in order not to have to deal with memory allocation
    // but I wanted V[i] to give me a pointer on the target occupying this
    // place and I couldn't do it
    _size = tmp.size();
    V = new Target[_size];
    for (unsigned int i = 0; i < tmp.size(); i++){
        V[i] = tmp[i];
    }
    targetFile.close();
}

int Field::cost() const{
    int res = 0;
    for (unsigned int i = 0; i < _size; i++){
        if (V[i].is_captor()){
            res++;
        }
    }
    return res;
}

void Field::compute_graph(const int &r_capt, const int &r_comm) {
    float d;
    for (unsigned int i = 0; i < _size; i++){
        for (unsigned int j = i+1; j < _size; j++){
            // I have to add a small quantity to the distance because in square grids, the
            // distance is exactly equal to the radius and I don't have perfect float precision.
            d = V[i].distance(V[j]) - 0.0000001;
            if (d < r_capt){
                V[i].add_target_capt(V+j);
                V[j].add_target_capt(V+i);
            }
            if (d < r_comm){
                V[i].add_target_comm(V+j);
                V[j].add_target_comm(V+i);
            }
        }
    }
}

void Field::check_solution_is_ok() const {
    for (unsigned int i = 0; i < _size; i++){
        if (!V[i].is_capted()){
            throw std::logic_error("The target " + std::to_string(V[i].get_id()) + "  is not capted !");
        }
    }
    if (!this->is_communicating()){
        throw std::logic_error("All the captors do not communicate with the well !");
    }
}

bool Field::is_communicating() const {
    // do a marking procedure
    Target u;
    vector<Target*> u_neighbors;
    std::queue<Target> my_queue;
    Target well = V[0];
    my_queue.push(well);
    vector<bool> captor_was_marked = vector<bool>(_size, false);
    captor_was_marked[0] = true;
    while (my_queue.size() > 0){
        u = my_queue.front();
        my_queue.pop();
        // std::cout << u.get_id() << std::endl;
        u_neighbors = u.get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors.size(); i++){
            if (u_neighbors[i]->is_captor() && !captor_was_marked[u_neighbors[i]->get_id()]){
                captor_was_marked[u_neighbors[i]->get_id()] = true;
                my_queue.push(*(u_neighbors[i]));
            }
        }
    }
    // checking for any unmarked captors
    for (unsigned int i = 0; i < captor_was_marked.size(); i++){
        if (V[i].is_captor() && !captor_was_marked[i]){
            // std::cout << "captor " << V[i].get_id() << " does not communicate" << endl;
            return false;
        }
    }
    return true;
}

Field::~Field() {
    delete [] V;
}
