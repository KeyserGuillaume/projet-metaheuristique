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
    Target t;
    //int r = 4;
    while (std::getline(targetFile, line)){
        t = Target(line, tmp.size());
        //if ((t.get_coords().first < r || t.get_coords().first > 39-r) || (t.get_coords().second < r || t.get_coords().second > 39-r))
            tmp.push_back(t);
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



Field::Field() {
    _size = 3;
    V = new Target[_size];
    V[0] = Target(0, fpair(0, 1));
    V[1] = Target(1, fpair(2, 0));
    V[2] = Target(2, fpair(2, 3));
    V[0].make_captor();
    V[1].make_captor();
    V[2].make_captor();
    compute_graph(2, 3);
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
            d = V[i].distance(V[j]);// - 0.0000001;
            if (d <= r_capt){
                V[i].add_target_capt(V+j);
                V[j].add_target_capt(V+i);
            }
            if (d <= r_comm){
                V[i].add_target_comm(V+j);
                V[j].add_target_comm(V+i);
            }
            if (d <= 2*r_capt){
                V[i].add_target_2_capt(V+j);
                V[j].add_target_2_capt(V+i);
            }
        }
    }
}

void Field::check_solution_is_ok() const {
    for (unsigned int i = 0; i < _size; i++){
        if (!V[i].is_capted()){
            throw std::logic_error("The target " + std::to_string(V[i].get_id()) + "  is not capted !");
        }
        if (V[i].get_id() != i){
            throw std::logic_error("The target of id " + std::to_string(V[i].get_id()) + " is in the " + std::to_string(i) + "th position !");
        }
    }
    if (!this->is_communicating()){
        throw std::logic_error("All the captors do not communicate with the well !");
    }
}


bool Field::is_everyone_capted() const {
    for (unsigned int i = 0; i < _size; i++) {
        if (!V[i].is_capted())
            return false;
    }
    return true;
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
            return false;
        }
    }
    return true;
}

Field::~Field() {
    delete [] V;
}

Target* Field::get_leaf() const {
    /*
     * finite random walk on the captors in communication. visit each captor at most once.
     * return the last visited captor. if the graph looks like a tree, it's probably a leaf.
     */
    Target* u;
    vector<Target*> u_neighbors_comm;
    vector<bool> visited = vector<bool> (_size, false);
    vector<Target*> potential_u = vector<Target*>(1, V+rand()%_size);
    do {
        u = potential_u[rand()%potential_u.size()];
        potential_u = vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (u_neighbors_comm[i]->is_captor() && !visited[u_neighbors_comm[i]->get_id()])
                potential_u.push_back(u_neighbors_comm[i]);
        }
        visited[u->get_id()] = true;
    } while (potential_u.size() > 0);
    return u;
}

vector<Target *> Field::get_triangle() const {
    Target *u, *u_old, *u_ancient;
    Target u_tmp = Target(-1);
    vector<Target*> u_neighbors_comm;
    bool found = false;
    vector<Target*> potential_u = vector<Target*>();
    for (unsigned int j = 0; j < 10 && !found; j++) {
        if (potential_u.size() == 0){
            do{
                potential_u = vector<Target*>(1, V+rand()%_size);
            } while (!potential_u[0]->is_captor());
            u_old  = &u_tmp;
            u_ancient = &u_tmp;
        }
        else{
            u_ancient = u_old;
            u_old = u;
        }
        u = potential_u[rand()%potential_u.size()];
        //std::cout << u->get_id() << " " << u_old->get_id() << " " << u_ancient->get_id() << std::endl;
        potential_u = vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (u_neighbors_comm[i]->is_captor()
                && u_neighbors_comm[i]->get_id() != u_old->get_id()) {
                potential_u.push_back(u_neighbors_comm[i]);
                if (u_neighbors_comm[i]->get_id() == u_ancient->get_id()){
                    found = true;
                }
            }
        }
    }
    if (found){
        vector<Target*> triangle;
        if (u->distance(*u_old) <= u_old->distance(*u_ancient) && u->distance(*u_ancient) <= u_old->distance(*u_ancient)){
            triangle.push_back(u_ancient); triangle.push_back(u_old); triangle.push_back(u);
        }
        else if(u_old->distance(*u) <= u->distance(*u_ancient) && u_old->distance(*u_ancient) <= u->distance(*u_ancient)){
            triangle.push_back(u); triangle.push_back(u_ancient); triangle.push_back(u_old);
        }
        else{
            triangle.push_back(u_old); triangle.push_back(u); triangle.push_back(u_ancient);
        }
        return triangle;
    }
    else{
        return vector<Target*>();
    }
}

void Field::write_solution(const std::string &filename) const {
    ofstream write(filename.c_str());
    if (!write.is_open())
    {
        cout << "pb with opening file" << filename << endl; throw;
    }
    for (unsigned int i = 0; i < _size; i++){
        if ((V+i)->is_captor())
            write << (V+i)->get_original_id() << endl;
    }
}
