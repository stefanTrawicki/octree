#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 10

int main(int argc, char const *argv[])
{

    OVector3 origin = OVector3{5, 5, 5};
    OVector3 bounds = OVector3{CONT_SIZE, CONT_SIZE, CONT_SIZE};

    Octree<int> o = Octree<int>(origin, bounds, 3);
    // std::cout << o.FindContainer(OVector3{13, 7, 7}) << std::endl;
    // std::cout << o.FindContainer(OVector3{0, 0, 0}) << std::endl;
    // std::cout << o.FindContainer(OVector3{5, 5, 5}) << std::endl;
    // std::cout << o.FindContainer(OVector3{15, 15, 15}) << std::endl;
    // std::cout << o.FindContainer(OVector3{16, 16, 16}) << std::endl;

    int x = 5;
    OVector3 position = {13, 7, 7};

    for (int i = 0; i < 2048; i++)
    {
        o.Put(i, position);
    }

    std::vector<int *> results = o.GetFromPosition(OVector3{0, 0, 0});

    int val = 11;
    std::cout << *(o.FindContainer(OVector3{val+-3, 5, 5})) << std::endl;
    std::cout << *(o.FindContainer(OVector3{val, 5, 5})) << std::endl;
    std::cout << *(o.FindContainer(OVector3{val+3, 5, 5})) << std::endl;
    std::cout << *(o.FindContainer(OVector3{val+6, 5, 5})) << std::endl;
    std::cout << *(o.FindContainer(OVector3{val+9, 5, 5})) << std::endl;
    std::cout << *(o.FindContainer(OVector3{val+12, 5, 5})) << std::endl;

    std::cout << o << std::endl;

    return 0;
}