#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 10
#define TEST_SET_SIZE 1000
#define RAND_MAC (double)(rand() % CONT_SIZE)

int main(int argc, char const *argv[])
{

    srand(time(0));

    OVector3 origin = OVector3{0, 0, 0};
    OVector3 bounds = OVector3{CONT_SIZE, CONT_SIZE, CONT_SIZE};

    Octree<int> o = Octree<int>(origin, bounds, 3);

    for (size_t i = 0; i < TEST_SET_SIZE; i++)
    {
        OVector3 pos = {RAND_MAC, RAND_MAC, RAND_MAC};
        // std::cout << pos << std::endl;
        o.Put(1, pos);
    }

    std::vector<struct Storable<int>> *results;

    size_t drumroll_pls = o.GetFromArea(
        OVector3{2.5, 2.5, 2.5},
        OVector3{7.5, 7.5, 7.5},
        results
    );

    std::cout << o << std::endl;

    std::cout << drumroll_pls << std::endl;

    return 0;
}