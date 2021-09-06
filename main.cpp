#include "src/Octree.hpp"

int main(int argc, char const *argv[])
{
    Octree<int> o = Octree<int>();
    std::cout << o.size() << std::endl;

    return 0;
}
