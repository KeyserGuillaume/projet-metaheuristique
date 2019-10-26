#include "cost_functions.h"
#include <math.h>

int CostFunction::operator() (const vector<Target*> & targets) const{
    /*
     * sum of unitary costs
     */
    int s = 0;
    for (unsigned int i = 0; i < targets.size(); i++){
        s += operator()(*targets[i]);
    }
    return s;
}

int CostFunction::operator() (const vector<Target*> & old_targets,
                              const vector<Target*> & new_targets) const{
    /*
     * cost of new solution minus cost of old solutions
     */
    return operator()(new_targets) - operator()(old_targets);
}

int NbCaptors::operator() (const Target & target) const{
    return (target.is_captor()) ? 1 : 0;
}

int MaxCover::operator() (const Target & target) const{
    return (target.is_captor()) ? - target.get_delta_capt().size() : 0;
}

int MaxLogCover::operator() (const Target & target) const{
    return (target.is_captor()) ? - log(target.get_delta_capt().size()) : 0;
}

int MinUniqueCover::operator() (const Target & target) const{
    if (target.is_captor()){
        int s = 0;
        int id = target.get_id();
        vector<Target*> neighbors = target.get_delta_capt();
        vector<Target*> possible_captors;
        bool is_capted_by_someone_else;
        for (unsigned int i = 0; i < neighbors.size(); i++){
            // maybe the well shouldn't be counted here
            if (!neighbors[i]->is_captor()){
                possible_captors = target.get_delta_capt();
                is_capted_by_someone_else = false;
                for (unsigned int j = 0; j < possible_captors.size(); j++){
                    if (possible_captors[j]->get_id() != id && possible_captors[j]->is_captor())
                        is_capted_by_someone_else = true;
                }
                if (!is_capted_by_someone_else)
                    s += 1;
            }
        }
        return s;
    }
    else{
        return 0;
    }
}

int MinLogUniqueCover::operator() (const Target & target) const{
    int res = MinUniqueCover::operator()(target);
    return (res == 0) ? 0 : log(res);
}