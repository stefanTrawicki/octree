#include "src/Octree.hpp"
#include <time.h>
#include <unistd.h>
#include <chrono>

#define CONT_SIZE 1000
#define TEST_SET_SIZE 1000000
#define LAYERS 4
#define RAND_MAC static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/CONT_SIZE))

int main(int argc, char const *argv[])
{

    srand(time(0));

    QVector2 origin = QVector2{0, 0};
    QVector2 bounds = QVector2{CONT_SIZE, CONT_SIZE};

    Quadtree<int> o = Quadtree<int>(origin, bounds, LAYERS);

    std::cout << o << std::endl;

    std::vector<struct Storable<int>> linear_test = std::vector<struct Storable<int>>(0);

    for (int i = 0; i < TEST_SET_SIZE; i++)
    {
        QVector2 v = {RAND_MAC, RAND_MAC};
        if (!o.Put(i, v))
            std::cout << "I failed somewhere! (" << v << ")" << std::endl; 
        struct Storable<int> s = {i, v};
        linear_test.push_back(s);
    }

    QVector2 lower = {0,0};
    QVector2 upper = {10,10};

    auto start_tree = std::chrono::high_resolution_clock::now();
    std::vector<struct Storable<int> *> results = o.Get(lower, upper);
    auto end_tree = std::chrono::high_resolution_clock::now();

    std::vector<struct Storable<int>> results_linear = std::vector<struct Storable<int>>(0);

    auto start_linear = std::chrono::high_resolution_clock::now();
    for (size_t j = 0; j < linear_test.size(); j++)
    {
        struct Storable<int> i = linear_test.at(j);
        if (i.position >= lower && i.position < upper)
            results_linear.push_back(i);
    }
    auto end_linear = std::chrono::high_resolution_clock::now();

    std::cout << "Tree: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_tree-start_tree).count() << "ns\n";
    std::cout << "Linear: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_linear-start_linear).count() << "ns\n";
    std::cout << "Delta: " << std::chrono::duration_cast<std::chrono::nanoseconds>((end_tree-start_tree)-(end_linear-start_linear)).count() << "ns\n";

    std::cout << "Tree n_results: " << results.size() << std::endl;
    std::cout << "Linear n_results: " << results_linear.size() << std::endl;

    return 0;
}