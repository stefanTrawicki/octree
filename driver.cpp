#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 10

int main(int argc, char const *argv[])
{

    OVector3 origin = OVector3{0, 0, 0};
    OVector3 bounds = OVector3{CONT_SIZE, CONT_SIZE, CONT_SIZE};

    Octree<int> o = Octree<int>(origin, bounds, 2);

    double val = 0;
    for (unsigned short i = 0; i < CONT_SIZE; i++)
    {
        std::cout << *(o.FindContainer(OVector3{val+i, 0, 0})) << std::endl;
    }

    std::cout << o << std::endl;

    return 0;
}