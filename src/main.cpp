#include "target.h"
#include "types.h"
#include "field.h"
#include "local_search.h"

#include <sstream>



int main(int argc, char *argv[]){ //std::cout << argv[1] << std::endl; return 0;

    std::string instance;
    int r_capt, r_comm;

    if (argc == 1){
        instance = "../../Instances/square_grid_40X40.dat";
        //instance = "../../Instances/grille3030_1_ok.dat";
        //instance = "../../Instances/captANOR225_9_20.dat";
        r_capt = 1;
        r_comm = 1;
    }
    else{
        if (argc != 4) throw std::invalid_argument( "No arguments given for r_capt and r_comm.");
        instance = "../../Instances/" + std::string(argv[1]);
        r_capt = std::stoi(argv[2]);
        r_comm = std::stoi(argv[3]);
    }
    std::cout << instance << r_capt << r_comm << std::endl;

    Field F(instance);
    F.compute_graph(r_capt, r_comm);
    CostFunction CF = CostFunction();
    LocalSearch LS (&F, &CF);

    //LS.write_solution("../../solutions/sol_1.txt");

    /*for (int i = 500; i < 550; i++) {
        std::cout << *(F[i]) << std::endl;
    } */

    LS.display();
    LS.check_solution_is_ok();
    LS.run(100000, 10000);

    // to check we are not stuck in a local minimum, we verify that
    // moves are accepted, by raising the verbosity level on a few iterations.
    LS.verbose = true;
    LS.run(100, 1);
    LS.display();

    LS.stats();

    LS.write_solution("../../solutions/sol_1.txt");

    std::cout << "found solution with nb of captors equal to :      ";

    std::cout << LS.current_value();

    return 0;
}
