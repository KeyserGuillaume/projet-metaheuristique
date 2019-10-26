#include "target.h"
#include "types.h"
#include "field.h"
#include "local_search.h"

#include <sstream>



int main(){

   // std::string instance = "../../Instances/square_grid_10X10.dat";
    std::string instance = "../../Instances/captANOR900_15_20.dat";

    Field F(instance);

    F.compute_graph(1, 2);

    NbCaptors CF = NbCaptors();

    LocalSearch LS (&F, &CF);

    LS.write_solution("../../solutions/sol_1.txt");

    /*for (int i = 500; i < 550; i++) {
        std::cout << *(F[i]) << std::endl;
    } */

    LS.display();

    LS.check_solution_is_ok();

    LS.run(1000000, 10000);

    // to check we are not stuck in a local minimum, we verify that
    // moves are accepted, by raising the verbosity level on a few iterations.
    /*LS.verbose = true;
    LS.run(100, 1);

    LS.display();*/

    LS.stats();

    LS.write_solution("../../solutions/sol_2.txt");


    return 0;
}
