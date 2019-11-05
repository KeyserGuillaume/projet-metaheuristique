#include "cost_functions.h"
#include <math.h>

int CostFunction::operator() (const vector<Target*> & targets) const{
    /*
     * sum of unitary costs
     */
    int s = 0;
    for (unsigned int i = 0; i < targets.size(); i++){
        s += operator()(targets[i]);
    }
    return s;
}


int CostFunction::operator() (Target* target) const{
    return -1;
}

int NbCaptors::operator() (Target* target) const{
    return (target->is_captor()) ? 1 : 0;
}

int MaxCover::operator() (Target* target) const{
    return (target->is_captor()) ? - target->get_delta_capt().size() : 0;
}

int MaxLogCover::operator() (Target* target) const{
    return (target->is_captor()) ? - log(target->get_delta_capt().size()) : 0;
}

int MinUniqueCover::operator() (Target* target) const{
    if (target->is_captor()){
        return target->get_uniquely_capted_targets().size();
    }
    else{
        return 0;
    }
}

int MinLogUniqueCover::operator() (Target* target) const{
    int res = MinUniqueCover::operator()(target);
    return (res == 0) ? 0 : 10*log(res);
}

int ShakeUp::operator()(Target *target) const {
    return abs(int(10 - target->get_coords().second)) + abs(int(20 - target->get_coords().first));
}
