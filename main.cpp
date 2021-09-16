#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 1000

int main(int argc, char const *argv[])
{

    OVector3 origin = OVector3{0, 0, 0};
    OVector3 bounds = OVector3{CONT_SIZE, CONT_SIZE, CONT_SIZE};

    Octree<int> o = Octree<int>(256, bounds, origin);

    // srand(time(NULL));

    // double x = 0, y = 0, z = 0;

    // for (size_t i = 0; i < (60000 * 3); i++)
    // {
    //     x = rand() % CONT_SIZE, y = rand() % CONT_SIZE, z = rand() % CONT_SIZE;
    //     if (!o.put(5, OVector3{x, y, z}))
    //     {
    //         cout << "Failed!" << endl;
    //     }
    // }

    cout << o << endl;

    return 0;
}