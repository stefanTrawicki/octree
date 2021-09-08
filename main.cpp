#include "src/Octree.hpp"

int main(int argc, char const *argv[])
{

    OVector3 origin = OVector3{5, 5, 5};
    OVector3 bounds = OVector3{10, 10, 10};

    Octree<int> o = Octree<int>(32, bounds, origin);
    cout << o << endl;

    cout << o.put(5, OVector3{7, 7, 7}) << endl;

    return 0;
}