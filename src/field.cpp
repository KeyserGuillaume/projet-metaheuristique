#include "field.h"

#include <iostream>
#include <fstream>
#include <queue>
#include <boost/pending/disjoint_sets.hpp>
#include <assert.h>

Field::Field(const std::string &filename){
    std::ifstream targetFile (filename.c_str());
    if (!targetFile.is_open()) throw std::runtime_error("No targets file found");
    std::string line;
    // put all the targets in a temporary vector
    std::vector<Target> tmp;
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
    unsigned int s = 0;
    for (unsigned int i = 0; i < _size; i++) {
        if ((V+i)->is_captor())
            s++;
    }
    if (s != captors.size()) {
        std::cerr << "solution contains " << captors.size() << " items whereas ";
        std::cerr << "Field contains " << s << " captors !!!" << std::endl;
        throw std::logic_error("Inconsistency found in solution");
    }
    for (unsigned int i = 0; i < _size; i++){
        if (!(V+i)->is_capted()){
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
    std::vector<Target*> u_neighbors;
    std::queue<Target> my_queue;
    Target well = V[0];
    my_queue.push(well);
    std::vector<bool> captor_was_marked = std::vector<bool>(_size, false);
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

bool Field::is_communicating_fast() {
    Target *u, *v, *w;
    std::vector<Target*> u_successors, u_neighbors;
    std::vector<bool> captor_was_marked_quickly = std::vector<bool>(_size, false);
    std::vector<bool> captor_was_marked = std::vector<bool>(_size, false);
    std::queue<Target*> my_queue;
    Target* well = V;
    my_queue.push(well);
    captor_was_marked_quickly[0] = true;
    // do the marking of people whose path from the well is known
    while (my_queue.size() > 0){
        u = my_queue.front();
        my_queue.pop();
        u_successors = u->get_successors();
        for (unsigned int i = 0; i < u_successors.size(); i++) {
            w = u_successors[i];
            captor_was_marked_quickly[w->get_id()] = true;
            captor_was_marked[w->get_id()] = true;
            my_queue.push(V+w->get_id());
        }
    }
    // for every captor not marked, do a slower marking and if the connexity is verified,
    // redo a slow marking in order to speed up the next time the function is called.
    for (unsigned int j = 0; j < captor_was_marked.size(); j++){
        if (V[j].is_captor() && !captor_was_marked[j]){
            my_queue.push(V+j);
            captor_was_marked[j] = true;
            bool found = false;
            while (my_queue.size() > 0 && !found){
                u = my_queue.front();
                my_queue.pop();
                u_neighbors = u->get_delta_comm();
                for (unsigned int i = 0; !found && i < u_neighbors.size(); i++){
                    w = u_neighbors[i];
                    if (w->is_captor() && !captor_was_marked[w->get_id()]){
                        captor_was_marked[w->get_id()] = true;
                        my_queue.push(w);
                    }
                    if (w->get_id() == 0 ||
                        (w->is_captor() && captor_was_marked_quickly[w->get_id()])){
                        found = true;
                        v = V+w->get_id();
//                        my_queue.clear();
                        my_queue = std::queue<Target*>();
                    }
                }
            }
            if (!found){
                return false;
            }
            my_queue.push(v);
            captor_was_marked_quickly[v->get_id()] = true;
            while (my_queue.size() > 0){
                u = my_queue.front();
                my_queue.pop();
                u_neighbors = u->get_delta_comm();
                for (unsigned int i = 0; i < u_neighbors.size(); i++){
                    w = u_neighbors[i];
                    if (w->is_captor() && !captor_was_marked_quickly[w->get_id()]){
                        w->clear_successors();
                        captor_was_marked_quickly[w->get_id()] = true;
                        captor_was_marked[w->get_id()] = true;
                        u->add_successor(w);
                        my_queue.push(w);
                    }
                }
            }
        }
    }
    //check_solution_is_ok();
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
    std::vector<Target*> u_neighbors_comm;
    std::vector<bool> visited = std::vector<bool> (_size, false);
    std::vector<Target*> potential_u = std::vector<Target*>(1, V+rand()%_size);
    do {
        u = potential_u[rand()%potential_u.size()];
        potential_u = std::vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (u_neighbors_comm[i]->is_captor() && !visited[u_neighbors_comm[i]->get_id()])
                potential_u.push_back(u_neighbors_comm[i]);
        }
        visited[u->get_id()] = true;
    } while (potential_u.size() > 0);
    return u;
}

void Field::write_solution(const std::string &filename) const {
    std::ofstream write(filename.c_str());
    if (!write.is_open())
    {
        std::cout << "pb with opening file" << filename << std::endl; throw;
    }
    for (unsigned int i = 0; i < _size; i++){
        if ((V+i)->is_captor())
            write << (V+i)->get_original_id() << std::endl;
    }
}

void Field::make_captor(const int &u) {
    (V+u)->make_captor();
    captors.push_back(u);
}

void Field::unmake_captor(const int &u) {
    (V+u)->unmake_captor();
    for (unsigned int i = 0; i < captors.size(); i++){
        if (captors[i] == u){
            // erase in constant time by replacing with last element and erasing last element
            // not really constant time, still need O(n) to find the position...
            captors[i] = captors[captors.size()-1];
            captors.pop_back();
            return;
        }
    }
}

std::vector<bool> Field::detect_captor_usefulness() const{
    std::vector<bool> potential_captor_uselessness = std::vector<bool> (_size, true);
    std::vector<unsigned int> nb_captors = std::vector<unsigned int>(_size, 0);
    std::vector<unsigned int> last_sighted_captor = std::vector<unsigned int>(_size, 0);
    std::vector<Target*> neighbours;
    for (unsigned int i = 0; i < captors.size(); i++){
        neighbours = (V+captors[i])->get_delta_capt();
        for (unsigned int k = 0; k < neighbours.size(); k++){
            nb_captors[neighbours[k]->get_id()]++;
            last_sighted_captor[neighbours[k]->get_id()] = captors[i];
        }
    }
    for (unsigned int i = 0; i < _size; i++){
        if (nb_captors[i] == 1){
            potential_captor_uselessness[last_sighted_captor[i]] = false;
        }
    }
    return potential_captor_uselessness;
}

vector_int Field::fill_edges_and_detect_kind_A(vector_edge &pure_edges_1,
                                               vector_edge &pure_edges_2,
                                               vector_edge &hybrid_edges,
                                               const std::vector<bool> &potential_captor_uselessness) const{
    unsigned int id1, id2;
    std::vector<Target *> neighbours, neighbour_captors;
    for (unsigned int i = 0; i < captors.size(); i++) {
        int nb_edges = 0;
        id1 = captors[i];
        neighbours = (V + captors[i])->get_delta_comm();
        for (unsigned int j = 0; j < neighbours.size(); j++) {
            id2 = neighbours[j]->get_id();
            if (!neighbours[j]->is_captor() && id2 != 0)
                continue;
            nb_edges++;
            if (id2 > id1)
                continue;
            if (potential_captor_uselessness[id1] && potential_captor_uselessness[id2])
                pure_edges_1.push_back(std::pair<unsigned int, unsigned int>(id1, id2));
            else if (potential_captor_uselessness[id1] && potential_captor_uselessness[id2])
                pure_edges_2.push_back(std::pair<unsigned int, unsigned int>(id1, id2));
            else
                hybrid_edges.push_back(std::pair<unsigned int, unsigned int>(id1, id2));
        }
        if (nb_edges <= 1){
            // detected an evil useless captor of type A
            return vector_int(1, id1);
        }
    }
    return vector_int();
}

vector_int Field::get_potentially_useless_captors() const{
    // we send back a subset of captors whose usefulness needs to be checked.
    // we do what we can to keep it small. We guarantee that all useless captors are
    // in the subset we return.
    std::vector<bool> potential_captor_uselessness = detect_captor_usefulness();
    //vector_edge pure_edges_1, pure_edges_2, hybrid_edges;
    //vector_int possible_result = fill_edges_and_detect_kind_A(pure_edges_1, pure_edges_2, hybrid_edges, potential_captor_uselessness);
    //if (possible_result.size() > 0)
    //    return possible_result;
    vector_int pucs; // potentially useless captors
    for (unsigned int i = 0; i < captors.size(); i++) {
        if (potential_captor_uselessness[captors[i]]) {
            pucs.push_back(captors[i]);
        }
    }
    return pucs;
}

bool Field::is_there_any_useless_captor(vector_int &result) const {
    // draft for an algo that detects useless captors; sends back an empty vector if there are none;
    // sends back a vector containing a subset of captors containing at least one useless captor it there are any.
    // /!\ it doesn't work because the algorithm is still incorrect /!\

    std::vector<bool> potential_captor_uselessness = detect_captor_usefulness();
    bool found = false;
    for (unsigned int i = 0; !found && i < _size; i++) {
        if (potential_captor_uselessness[i] == true)
            found = true;
    }
    if (!found)
        return false;
    // next, we will determine the sets of pure edges and hybrid edges.
    // a hybrid edge is between a potentially useless captor and a non-potentially useless captor.
    // a pure edge of type 1 is between two uniquely capting captors
    // a pure edge of type 2 is between two pucs (Potentially Useless CaptorS).
    vector_edge pure_edges_1, pure_edges_2, hybrid_edges, unique_hybrid_edges;
    vector_int possible_result = fill_edges_and_detect_kind_A(pure_edges_1, pure_edges_2, hybrid_edges, potential_captor_uselessness);
    if (possible_result.size() > 0){
        result = possible_result;
        return true;
    }
    // with this set of pure_edges we will define a coloration (= partition): two uniquely capting captors have the
    // same color iff should the potentially useless captors be removed, they are in the same connex component.
    // the potentially useless captors get the dual coloration.
    std::vector<int> rank(_size);
    std::vector<int> parent(_size);
    boost::disjoint_sets<int *, int *> ds(&rank[0], &parent[0]);
    // initialize with one color per captor and one for the well
    for (int i = 0; i < captors.size(); i++) {
        ds.make_set(captors[i]);
    }
    ds.make_set(0);
    // merge the components of the uccs (Uniquely Capting CaptorS)
    for (unsigned int i = 0; i < pure_edges_1.size(); i++) {
        ds.union_set(pure_edges_1[i].first, pure_edges_1[i].second);
    }

    // finally we will completetely merge the coloration with the hybrid edges (between the potentially useless captors
    // and the others). this time if we introduce cycles it means we have found a useless captor.
    // begin by filtering the duplicate hybrid edges. set the puc as the first extremity of the edge.
    for (unsigned int i = 0; i < hybrid_edges.size(); i++) {
        bool found = false; // check whether there was a duplicate link
        for (int k = 0; k < unique_hybrid_edges.size() && !found; k++) {
            if (hybrid_edges[i].first == unique_hybrid_edges[k].first &&
                hybrid_edges[i].second == unique_hybrid_edges[k].second)
                found = true;
        }
        if (!found){
            if (potential_captor_uselessness[hybrid_edges[i].first])
                unique_hybrid_edges.push_back(
                    std::pair<unsigned int, unsigned int>(hybrid_edges[i].first, hybrid_edges[i].second));
            else
                unique_hybrid_edges.push_back(
                        std::pair<unsigned int, unsigned int>(hybrid_edges[i].second, hybrid_edges[i].first));
        }
    }

    vector_int pucs; // potentially useless captors
    for (unsigned int i = 0; i < captors.size(); i++) {
        if (potential_captor_uselessness[captors[i]]) {
            pucs.push_back(captors[i]);
        }
    }

    // build the connex components of all potentially useless captors
    std::map<unsigned int, std::vector<unsigned int> > connex_components;
    std::map<unsigned int, unsigned int> id_to_connex_component;
    for (unsigned int i = 0; i < pucs.size(); i++){
        unsigned int u = ds.find_set(pucs[i]); // determine the component of the captor
        id_to_connex_component[pucs[i]] = u;
        if (connex_components.count(u) == 0)
            connex_components[u] = std::vector<unsigned int>();
        connex_components[u].push_back(pucs[i]);
    }

    // merge the colorations
    std::map<unsigned int, unsigned int> nb_of_non_duplicate_links_of_cc;
    for (unsigned int i = 0; i < unique_hybrid_edges.size(); i++) {
        unsigned int u = ds.find_set(unique_hybrid_edges[i].first); // determine the component of the captor
        unsigned int v = ds.find_set(unique_hybrid_edges[i].second);
        if (u == v) {
            // we have detected a connex component of useless captors and will return it.
            result = connex_components[id_to_connex_component[unique_hybrid_edges[i].first]];
            std::cout << result.size() << std::endl;
            return false;
        } else {
            ds.union_set(u, v); // merge the components of the captors;
            nb_of_non_duplicate_links_of_cc[id_to_connex_component[unique_hybrid_edges[i].first]]++;
        }
    }
    // check that there is no puc connex component uselessly standing in a corner
    for (std::map<unsigned int, std::vector<unsigned int> >::iterator it = connex_components.begin();
            it != connex_components.end(); it++){
        if (nb_of_non_duplicate_links_of_cc[it->first] <= 1){
            result = it->second;
            std::cout << result.size() << std::endl;
            return true;
        }
    }
    // there is no useless captor
    return false;
}
