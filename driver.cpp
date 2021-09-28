#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 10000
#define TEST_SET_SIZE 9200
#define BOX_SIZE 3
#define RAND_MAC static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/CONT_SIZE))

int main(int argc, char const *argv[])
{

    srand(time(0));

    OVector3 origin = OVector3{0, 0, 0};
    OVector3 bounds = OVector3{CONT_SIZE, CONT_SIZE, CONT_SIZE};

    Octree<int> o = Octree<int>(origin, bounds, 5);

    std::vector<OVector3> item_list_test = std::vector<OVector3>(0);

    for (size_t i = 0; i < TEST_SET_SIZE; i++)
    {
        OVector3 pos = {RAND_MAC, RAND_MAC, RAND_MAC};
        o.Put(1, pos);
        item_list_test.push_back(pos);
    }

    std::vector<struct Storable<int>> results = std::vector<struct Storable<int>>(0);

    OVector3 ret = {RAND_MAC, RAND_MAC, RAND_MAC};
    OVector3 ret_end = {ret.x+BOX_SIZE, ret.y+BOX_SIZE, ret.z+BOX_SIZE};

    std::vector<OVector3> results_2 = std::vector<OVector3>(0);
    std::vector<OVector3> results_3 = std::vector<OVector3>(0);

    auto start_manual = std::chrono::high_resolution_clock::now();

    for (auto const& i : item_list_test)
    {
        // std::cout << "i: " << i << " ret: " << ret << " i > ret " << (i > ret) << std::endl;
        if (i > ret && i < ret_end)
        {
            results_2.push_back(i);
        }
    }

    auto finish_manual = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish_manual-start_manual).count() << "ns\n";

    auto start_octree = std::chrono::high_resolution_clock::now();

    size_t drumroll_pls = o.GetFromArea(
        ret,
        ret_end,
        &results
    );


    for (auto const& i : results)
    {
        // std::cout << "i: " << i << " ret: " << ret << " i > ret " << (i > ret) << std::endl;
        if (i.position > ret && i.position < ret_end)
        {
            results_3.push_back(i.position);
        }
    }
    auto finish_octree = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish_octree-start_manual).count() << "ns\n";

    // std::cout << o << std::endl;

    std::cout << drumroll_pls << std::endl;
    std::cout << results.size() << std::endl;
    std::cout << results_2.size() << std::endl;
    std::cout << results_3.size() << std::endl;

    return 0;
}