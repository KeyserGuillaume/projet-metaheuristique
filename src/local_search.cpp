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
    //if (rand()%10 == 0){musical_chairs();return;}
    flea_jump(current_solution[rand() % current_solution.size()]);
}

void LocalSearch::musical_chairs(){
    Target* u = (*F)[current_solution[rand() % current_solution.size()]];
    vector<Target*> ups, downs, u_neighbors_comm, u_neighbors_2_capt, potential_u;
    // do random walk on the captors in comm, starting from this leaf, and remove those captors
    vector<bool> visited = vector<bool> (F->size(), false);
    potential_u = vector<Target*>(1, u);
    do {
        u = potential_u[rand()%potential_u.size()];
        potential_u = vector<Target*>(0);
        u_neighbors_comm = u->get_delta_comm();
        for (unsigned int i = 0; i < u_neighbors_comm.size(); i++){
            if (u_neighbors_comm[i]->is_captor())
                potential_u.push_back(u_neighbors_comm[i]);
        }
        u->unmake_captor();
        ups.push_back(u);
    } while ((potential_u.size() > 0 && rand()%3 != 0) || (potential_u.size() > 0 && ups.size() < 2));

    // choose some captor nearby from which to start the reconstruction
    // not an infinite loop because there is the last captor we put in ups
    potential_u = vector<Target*>(0);
    do {
        u_neighbors_2_capt = ups[rand() % ups.size()]->get_delta_2_capt();
        for (unsigned int i = 0; i < u_neighbors_2_capt.size(); i++) {
            if (u_neighbors_2_capt[i]->is_captor() || u_neighbors_2_capt[i]->get_id() == 0)
                potential_u.push_back(u_neighbors_2_capt[i]);
        }
    } while (potential_u.size() == 0);
    u = potential_u[rand()%potential_u.size()];
    // reconstruct in greedy way from this captor
    int value, max; vector<int> values;
    for (unsigned int i = 0; i < ups.size(); i++){
        max = 0;
        u_neighbors_comm = u->get_delta_comm();
        values = vector<int> (u_neighbors_comm.size(), 0);
        for (unsigned int j = 0; j < u_neighbors_comm.size(); j++){
            if (u_neighbors_comm[j]->is_captor()){
                value = 0;
            }
            else{
                u_neighbors_comm[j]->make_captor();
                value = u_neighbors_comm[j]->get_uniquely_capted_targets().size();
                u_neighbors_comm[j]->unmake_captor();
            }
            if (value > max){
                max = value;
            }
            values[j] = value;
        }
        if (max > 0){
            potential_u = vector<Target*>(0);
            for (unsigned int j = 0; j < u_neighbors_comm.size(); j++){
                if (values[j] >= 0.7*max){
                    potential_u.push_back(u_neighbors_comm[j]);
                }
            }
            u = potential_u[rand()%potential_u.size()];
            downs.push_back(u);
            u->make_captor();
        }
    }
    if (!F->is_everyone_capted() || !F->is_communicating()){
        //F->write_solution("../../solutions/sol_2.txt");
        for (int i = downs.size() - 1; i >= 0; i--){
            downs[i]->unmake_captor();
        }
        for (int i = ups.size() - 1; i >= 0; i--){
            ups[i]->make_captor();
        }
        //F->write_solution("../../solutions/sol_3.txt");
        return;
    }
    for (unsigned int i = 0; i < downs.size(); i++){
        move_solution(ups[i]->get_id(), downs[i]->get_id());
    }
    for (unsigned int i = downs.size(); i < ups.size(); i++){
        remove_captor_from_solution(ups[i]->get_id());
    }
    nb_other_moves++;
    /*for (unsigned int i = 0; i < F->size(); i++){
        remove_if_useless_captor(i);
    }*/
}

void LocalSearch::flea_jump(const int &id) {
    /*
     * Move captor from v1 to v2 if feasible
     * if not feasible, try to repair with a captor in v3 moving to v4
     * id must be the id of a captor
     */
    Target *v1, *v2, *v3, *v4, *u, *t;
    v1 = (*F)[id];
    if (!v1->is_captor())
        throw std::invalid_argument( "LocalSearch::flea_jump only accepts captors.");
    int previous_cost = (*cost_computer)(v1);
    // Find targets uniquely capted by v1, not captors themselves, not the well
    vector<Target*> v1_essential = v1->get_uniquely_capted_targets();
    // If none, check that v1 is not useless. Exit function.
    if (v1_essential.size() == 0){
        v1->unmake_captor();
        if (v1->is_capted() && F->is_communicating()) {
            std::cout << "stumbled on a useless captor by chance" << std::endl;
            remove_captor_from_solution(id);
        }
        else{
            v1->make_captor();
            caterpillar_move(id, true);
        }
        return;
    }
    // Pick a random such target
    // We could build a vector of acceptable neighbors for u, but
    // I think that would slow down iterations
    vector<Target*> potential_v2;
    u = v1_essential[rand() % v1_essential.size()];
    potential_v2 = u->get_delta_capt();
    potential_v2.push_back(u);
    // Pick a random neighbor of u for v2, not v1 (we know that it cannot be a captor)
    do {
        v2 = potential_v2[rand()%potential_v2.size()];
    } while (v2->get_id() == id);
    // Make changes in the field, they will be reverted if the move is unfeasible
    v1->unmake_captor();
    v2->make_captor();
    // Stop there and revert change if moving the captor of v1 to v2 would mean
    // some targets would no longer be capted. The only targets that need verification
    // are those uniquely capted by v1 AND v1 itself.
    bool need_to_repair = false;
    for (unsigned i = 0; i < v1_essential.size() && !need_to_repair; i++){
        if (!v1_essential[i]->is_capted()){
            need_to_repair = true;
            t = v1_essential[i];
        }
    }
    // try to fix this by moving a captor v3 to v4
    if (need_to_repair){
        vector<Target*> potential_v3, potential_v4, t_neighbors;
        potential_v4 = t->get_delta_capt();
        potential_v4.push_back(t);
        // we know there is at least v1 and t in potential_v4, we exclude v1
        do{
            v4 = potential_v4[rand()%potential_v4.size()];
        } while (v4->get_id() == v1->get_id());
        t_neighbors = t->get_delta_comm();
        potential_v3 = vector<Target*> (0);
        // we make sure to exclude v2 from the choice for v3
        for (unsigned int i = 0; i < t_neighbors.size(); i++){
            if (t_neighbors[i]->is_captor() && t_neighbors[i]->get_id() != v2->get_id())
                potential_v3.push_back(t_neighbors[i]);
        }
        if (potential_v3.size() == 0){
            v2->unmake_captor();
            v1->make_captor();
            return;
        }
        v3 = potential_v3[rand()%potential_v3.size()];
        v3->unmake_captor();
        v4->make_captor();

        bool found = false;

        // verify v1's neighbors are capted
        for (unsigned int i = 0; i < v1_essential.size() && !found; i++){
            if (!v1_essential[i]->is_capted()){
                found = true;
            }
        }
        // verify v3's neighbors are capted
        vector<Target*> v3_neighbors = v3->get_delta_capt();
        for (unsigned int i = 0; i < v3_neighbors.size() && !found; i++){
            if (!v3_neighbors[i]->is_capted()){
                found = true;
            }
        }

        if (found || !v3->is_capted()){
            v1->make_captor();
            v2->unmake_captor();
            v3->make_captor();
            v4->unmake_captor();
            return;
        }
    }

    if (!v1->is_capted()){
        v1->make_captor();
        v2->unmake_captor();
        if (need_to_repair){
            v3->make_captor();
            v4->unmake_captor();
        }
        return;
    }
    // Stop there and revert change if moving the captor of v1 to v2 would mean some captors
    // would no longer be in communication with the well
    // note that the cost thing is not correct anymore
    if (!F->is_communicating() || (*cost_computer)(v2) > previous_cost){
        v1->make_captor();
        v2->unmake_captor();
        if (need_to_repair){
            v3->make_captor();
            v4->unmake_captor();
        }
        return;
    }
    // If we got this far then the move was feasible, for now we accept it
    move_solution(v1->get_id(), v2->get_id());
    nb_flea_jumps++;
    if (verbose)
        std::cout << "accepted to move captor of " << v1->get_id() << " to " << v2->get_id() << std::endl;
    if (need_to_repair) {
        move_solution(v3->get_id(), v4->get_id());
        if (verbose)
            std::cout << "moved captor of " << v1->get_id() << " to " << v2->get_id() << " at the same time" << std::endl;
    }
    // Check that the neighbors of v2 in the graph of radius 2*r_capt did not become useless
    vector<Target*> v2_neighbors = v2->get_delta_2_capt();
    for (unsigned int i = 0; i < v2_neighbors.size(); i++){
        remove_if_useless_captor(v2_neighbors[i]->get_id());
    }
}

void LocalSearch::caterpillar_move(const int &id, const bool &repeat) {
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
        nb_caterpillar_moves++;
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
        if (repeat && rand()%3 != 0)
            caterpillar_move(id, repeat);
    }
}

void LocalSearch::write_solution(const std::string &filename) const {
    F->write_solution(filename);
    //write_solution_to_file(filename, current_solution);
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

void LocalSearch::do_very_bad_things_to_triangle() {
    vector<Target*> triangle = F->get_triangle();
    if (triangle.size() == 0)
        return;


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
