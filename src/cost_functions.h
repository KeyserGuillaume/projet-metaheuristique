#pragma once
#include "target.h"

class CostFunction{
    /*
     * it is a unitary cost function :
     * the cost of the solution is the sum of the costs
     * of the targets.
     */
public:
    virtual int operator() (Target* target) const;
    int operator() (const std::vector<Target*> & targets) const;
};

class NbCaptors: public CostFunction{
    /*
     * The simplest possible: constant
     */
public:
    virtual int operator() (Target* target) const;
};

class MaxCover: public CostFunction{
    /*
     * Maximize the number of covered targets
     * Seems the iterations are less efficient ...?
     */
public:
    virtual int operator() (Target* target) const;
};

class MaxLogCover: public CostFunction{
    /*
     * Maximize the log of number of covered targets
     */
public:
    virtual int operator() (Target* target) const;
};

class MinUniqueCover: public CostFunction{
    /*
     * Minimize the number of uniquely covered targets
     * no effect on caterpillar move, probably not on grids either
     */
public:
    virtual int operator() (Target* target) const;
};

class MinLogUniqueCover: public MinUniqueCover{
    /*
     * Minimize the log of the number of uniquely covered targets
     * no effect on caterpillar move, probably not on grids either
     */
public:
    virtual int operator() (Target* target) const;
};

class ShakeUp: public CostFunction{
    /*
     * Move up every captor
     */
public:
    virtual int operator() (Target* target) const;
};