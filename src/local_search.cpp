#include "local_search.h"


LocalSearch::LocalSearch(Field* my_f, CostFunction* my_cost){
    F = my_f;
    cost_computer = my_cost;
    k = 0;
    init();
}

void LocalSearch::remove_if_useless_captor(const int &i) {
    Target* u = (*F)[i];
    if (!u->is_captor() || u->has_any_uniquely_capted_targets()){
        return;
    }
    u->unmake_captor();
    if (u->is_capted() && F->is_communicating()){
        remove_captor_from_solution(i);
    }
    else{
        //std::cout << "captor " << i << " is required for communication" << std::endl;
        u->make_captor();
    }
}

void LocalSearch::remove_captor_from_solution(const int &id) {
    for (unsigned int i = 0; i < current_solution.size(); i++){
        if (current_solution[i] == id){
            // erase in constant time by replacing with last element and erasing last element
            current_solution[i] = current_solution[current_value()-1];
            current_solution.pop_back();
            if (k > 0) std::cout << "removed a useless captor " << i << std::endl;
        }
    }
}

void LocalSearch::init() {
    // simplest way to initialize : make them all captors, except the well
    for (unsigned int i = 1; i < F->size(); i++){
        (*F)[i]->make_captor();
        current_solution.push_back(i);
    }
    for (unsigned int i = 1; i < F->size(); i++){
        remove_if_useless_captor(i);
    }
    //initialize random numbers generator
    srand((unsigned int)time(0));
}

void LocalSearch::run(const long &nb_iteration, const int &period_display) {
    int k0 = k;
    for (; k < k0 + nb_iteration; k++){
        jump();
        if ((k - k0)%period_display==0) {
            display();
            check_solution_is_ok(); // raises errors if pb
        }
    }
}

void LocalSearch::run_on_time_limit(const clock_t time_limit, const int &period_display) {
    int k0 = k;
    bool has_exceeded_time_limit = false;
    for (; !has_exceeded_time_limit; k++) {
        jump();
        if ((k - k0)%period_display==0) {
            display();
            check_solution_is_ok(); // raises errors if pb
        }
        if ((k - k0) % 100 == 0) {
            if (clock() > time_limit)
                has_exceeded_time_limit = true;
        }
    }

}

void LocalSearch::jump() {
    flea_move(current_solution[rand()%current_solution.size()]);
}

void LocalSearch::flea_move(const int &id) {
    /*
     * Move captor from v1 to v2 if feasible
     * id must be the id of a captor
     */
    Target* v1 = (*F)[id];
    if (!v1->is_captor())
        throw std::invalid_argument( "LocalSearch::flea_move only accepts captors.");
    int previous_cost = (*cost_computer)(v1);
    // Find targets uniquely capted by v1, not captors themselves, not the well
    vector<Target*> v1_essential = v1->get_uniquely_capted_targets();
    // If none, check that v1 is not useless. Exit function.
    if (v1_essential.size() == 0){
        v1->unmake_captor();
        if (v1->is_capted() && F->is_communicating()) {
            remove_captor_from_solution(id);
            std::cout << "careful" << std::endl;
        }
        else{
            v1->make_captor();
            caterpillar_move(id);
        }
        return;
    }
    // Pick a random such target
    // We could build a vector of acceptable neighbors for u, but
    // I think that would slow down iterations
    Target* u;
    vector<Target*> potential_v2;
    u = v1_essential[rand() % v1_essential.size()];
    potential_v2 = u->get_delta_capt();
    potential_v2.push_back(u);
    if (potential_v2.size() == u->get_delta_capt().size()) throw std::logic_error("Shit");
    // Pick a random neighbor of u for v2, not the well, not v1 (we know that it cannot be a captor)
    Target* v2;
    do {
        v2 = potential_v2[rand()%potential_v2.size()];
    } while (v2->get_id() == id); // || v2->get_id() == 0);
    // Make changes in the field, they will be reverted if the move is unfeasible
    v1->unmake_captor();
    v2->make_captor();
    // Stop there and revert change if moving the captor of v1 to v2 would mean
    // some targets would no longer be capted. The only targets that need verification
    // are those uniquely capted by v1 AND v1 itself.
    for (unsigned int i = 0; i < v1_essential.size(); i++){
        if (!v1_essential[i]->is_capted()){
            v2->unmake_captor();
            v1->make_captor();
            return;
        }
    }
    if (!v1->is_capted()){
        v2->unmake_captor();
        v1->make_captor();
        return;
    }
    // Stop there and revert change if moving the captor of v1 to v2 would mean some captors
    // would no longer be in communication with the well
    if (!F->is_communicating() || (*cost_computer)(v2) > previous_cost){
        v2->unmake_captor();
        v1->make_captor();
        return;
    }
    // If we got this far then the move was feasible, for now we accept it
    move_solution(v1->get_id(), v2->get_id());
    if (verbose)
        std::cout << "accepted to move captor of " << v1->get_id() << " to " << v2->get_id() << std::endl;
    // Check that the neighbors of v2 in the graph of radius 2*r_capt did not become useless
    vector<Target*> v2_neighbors = v2->get_delta_2_capt();
    for (unsigned int i = 0; i < v2_neighbors.size(); i++){
        remove_if_useless_captor(v2_neighbors[i]->get_id());
    }
}

void LocalSearch::caterpillar_move(const int &id) {
    // we assume u does not have any uniquely capted targets
    Target* u;
    // this is the tail of the caterpillar, ie the targets whose captors we will remove
    vector<Target*> tail = vector<Target*>(0);
    vector<Target*> potential_u = vector<Target*>(1, (*F)[id]);
    vector<Target*> u_neighbors_comm;
    int previous_cost = 0;
    int u_old = -1;
    bool caterpillar_tail = true;
    // trace the tail + body of the caterpillar
    do {
        u = potential_u[rand()%potential_u.size()];
        if (u->has_any_uniquely_capted_targets()){
            caterpillar_tail = false;
        }
        if (caterpillar_tail){
            previous_cost += (*cost_computer)(u);
            u->unmake_captor();
            if (u->is_capted()){
                tail.push_back(u);
            }
            else{
                u->make_captor();
                caterpillar_tail = false;
                previous_cost -= (*cost_computer)(u);
            }
        }
        potential_u = vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (u_neighbors_comm[i]->is_captor() && u_neighbors_comm[i]->get_id() != u_old)
                potential_u.push_back(u_neighbors_comm[i]);
        }
        u_old = u->get_id();
    } while (potential_u.size() > 0 && rand()%10 < 9);
    // if tail is empty (ie the captor of id "id" didn't have any captor to capt him) there is nothing to do
    if (tail.size() == 0){
        return;
    }
    if (tail[0]->get_id() != id) throw std::logic_error("Dafuck?");
    // define the head (ie where we will place the new captors)
    vector<Target*> head = vector<Target*>(0);
    for (unsigned int i = 0; i < tail.size(); i++){
        potential_u = vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (!u_neighbors_comm[i]->is_captor())
                potential_u.push_back(u_neighbors_comm[i]);
        }
        if (potential_u.size() == 0){
            for (unsigned int i = 0; i < F->size(); i++){
                if (!(*F)[i]->is_captor())
                    potential_u.push_back((*F)[i]);
            }
        }
        u = potential_u[rand()%potential_u.size()];
        u->make_captor();
        head.push_back(u);
    }

    // if the new solution is feasible, we accept it
    // if it is not, revert changes
    if (F->is_communicating() && previous_cost <= (*cost_computer)(head)){
        for (unsigned int i = 0; i < tail.size(); i++){
            move_solution(tail[i]->get_id(), head[i]->get_id());
        }
        for (unsigned int i = 1; i < F->size(); i++){
            remove_if_useless_captor(i);
        }
        if (verbose) std::cout << "caterpillar move accepted" << std::endl;
    }
    else{
        // do not change the order of the two lines below, they are inversed
        // compared to the order above and it must stay so because sometimes
        // some captor goes into the head and then into the tail...
        // do not group them together either.
        for (unsigned int i = 0; i < tail.size(); i++){
            head[i]->unmake_captor();
        }
        for (unsigned int i = 0; i < tail.size(); i++){
            tail[i]->make_captor();
        }
    }
}

void LocalSearch::write_solution(const std::string &filename) const {
    write_solution_to_file(filename, current_solution);
}


int nb_chiffres(int i){
    if (i < 2)
        return 1;
    else
        return 1 + int(log(i)/log(10));
}
void LocalSearch::stats() const {
    map<int, int> count;
    int nb_uniquely_capted_targets;
    for (unsigned int i = 1; i < current_solution.size(); i++){
        nb_uniquely_capted_targets = (*F)[current_solution[i]]->get_uniquely_capted_targets().size();
        count[nb_uniquely_capted_targets]++;
    }
    int max_nb = count.rbegin()->first;
    cout << "  stats : histogram of the nb of uniquely capted targets" << endl;
    for (int i=0; i<max_nb+1; i++)
        cout << i << "       ";
    cout << endl;
    for (int i=0; i<max_nb+1; i++){
        cout << count[i] << string(7-nb_chiffres(count[i])+nb_chiffres(i), ' ');
    }
    cout << endl;
}

void LocalSearch::stats(CostFunction *myCost) const {

}

int LocalSearch::current_cost() const {
    int s= 0;
    Target u;
    for (unsigned int i = 0; i < F->size(); i++){
        s += (*cost_computer)((*F)[i]);
    }
    return s;
}

void LocalSearch::check_solution_is_ok(const bool &only_inner_consistency) const {
    unsigned int s = 0;
    for (unsigned int i = 0; i < F->size(); i++) {
        if ((*F)[i]->is_captor())
            s++;
    }
    if (s != current_solution.size()) {
        std::cerr << "solution contains " << current_solution.size() << " items whereas ";
        std::cerr << "Field contains " << s << " captors !!!" << std::endl;
        throw std::logic_error("Inconsistency found in solution");
    }
    if (!only_inner_consistency){
        F->check_solution_is_ok();
    }
}

void LocalSearch::move_solution(const int &v1, const int &v2) {
    bool found = false;
    for (unsigned int i = 0; i < current_solution.size() && !found; i++){
        if (current_solution[i] == v1){
            current_solution[i] = v2;
            found = true;
        }
    }
    if (!found){
        throw std::logic_error("Target " + std::to_string(v1) + " should have been in the solution.");
    }
}

void write_solution_to_file(std::string filename, vector<int> solution) {
    ofstream write(filename.c_str());
    if (!write.is_open())
    {
        cout << "pb with opening file" << filename << endl; throw;
    }
    for (typename vector<int>::iterator it = solution.begin(); it!= solution.end(); ++it){
        write << *it << endl;
    }
}
