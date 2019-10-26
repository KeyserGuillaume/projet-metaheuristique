#include "local_search.h"


LocalSearch::LocalSearch(Field* my_f, CostFunction* my_cost){
    F = my_f;
    cost_computer = my_cost;
    k = 0;
    init();
    verbose = false;
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
            // write_solution();
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
    if (!(*F)[id]->is_captor())
        throw std::invalid_argument( "LocalSearch::flea_move only accepts captors.");
    Target* v1 = (*F)[id];
    // Find targets uniquely capted by v1, not captors themselves, not the well
    vector<Target*> v1_essential = v1->get_uniquely_capted_targets();
    // If none, check that v1 is not useless. Exit function.
    if (v1_essential.size() == 0){
        v1->unmake_captor();
        if (v1->is_capted() && F->is_communicating()) {
            remove_captor_from_solution(id);
            std::cout << "careful" << std::endl;
        }
        else
            v1->make_captor();
        return;
    }
    // Pick a random such target
    Target u = *(v1_essential[rand()%v1_essential.size()]);
    vector<Target*> u_neighbors = u.get_delta_capt();
    // Pick a random neighbor of u for v2, not a captor, not the well
    Target* v2 = u_neighbors[rand()%u_neighbors.size()];
    while (v2->get_id() == 0 || v2->is_captor()){
        v2 = u_neighbors[rand()%u_neighbors.size()];
    }
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
    if (!F->is_communicating()){
        v2->unmake_captor();
        v1->make_captor();
        return;
    }
    // If we got this far then the move was feasible, for now we accept it
    for (unsigned int i = 0; i < current_solution.size(); i++){
        if (current_solution[i] == v1->get_id()){
            current_solution[i] = v2->get_id();
        }
    }
    if (verbose)
        std::cout << "accepted to move captor of " << v1->get_id() << " to " << v2->get_id() << std::endl;
    // Check that the neighbors of v2 in the graph of radius 2*r_capt did not become useless
    vector<Target*> v2_neighbors = v2->get_delta_2_capt();
    for (unsigned int i = 0; i < v2_neighbors.size(); i++){
        remove_if_useless_captor(v2_neighbors[i]->get_id());
    }
}

void LocalSearch::caterpillar_move(const int &id) {

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
        s += (*cost_computer)(*((*F)[i]));
    }
    return s;
}

void LocalSearch::check_solution_is_ok() const {
    F->check_solution_is_ok();
    unsigned int s = 0;
    for (unsigned int i = 1; i < F->size(); i++) {
        if ((*F)[i]->is_captor())
            s++;
    }
    if (s != current_solution.size()) {
        std::cerr << "LocalSearch::check_solution_is_ok : ";
        std::cerr << "solution contains " << current_solution.size() << " items whereas ";
        std::cerr << "Field contains " << s << " captors !!!" << std::endl;
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
