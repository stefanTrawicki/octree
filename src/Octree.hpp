#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>
#include <math.h>

// given an index, these are the children that neighbour you
#define INDEX_MAPPING { {4, 2, 1}, {5, 3, 0}, {6, 0, 3}, {7, 1, 2}, {0, 6, 5}, {1, 7, 4}, {2, 4, 7}, {3, 5, 6} }
// if you have to retrieve a child from a neighbour, they can be found in this direction
#define DIRECTION_MAPPINGS { {0, 1, 2}, {0, 1, 5}, {0, 4, 2}, {0, 4, 5}, {3, 1, 2}, {3, 1, 5}, {3, 4, 2}, {3, 4, 5} }

class OVector3
{
public:
    double x;
    double y;
    double z;

    friend bool operator==(const OVector3 &l, const OVector3 &r);
    friend bool operator<(const OVector3 &l, const OVector3 &r);
    friend bool operator<=(const OVector3 &l, const OVector3 &r);
    friend bool operator>(const OVector3 &l, const OVector3 &r);
    friend bool operator>=(const OVector3 &l, const OVector3 &r);
    friend bool operator!=(const OVector3 &l, const OVector3 &r);
    friend std::ostream &operator<<(std::ostream &os, const OVector3 &o);
    OVector3 &operator+=(const OVector3 &rhs);
    friend OVector3 operator+(OVector3 lhs, const OVector3 &rhs);
    void OVec3Mult(float factor);
};

bool operator==(const OVector3 &l, const OVector3 &r);
bool operator<(const OVector3 &l, const OVector3 &r);
bool operator<=(const OVector3 &l, const OVector3 &r);
bool operator>(const OVector3 &l, const OVector3 &r);
bool operator>=(const OVector3 &l, const OVector3 &r);
bool operator!=(const OVector3 &l, const OVector3 &r);
std::ostream &operator<<(std::ostream &os, const OVector3 &o);
OVector3 operator+(OVector3 lhs, const OVector3 &rhs);
void OVec3Mult(float factor);

template <class T>
struct Storable
{
    T data;
    OVector3 position;
};

template <class T>
class Octree;

class OctreeCell
{
private:
    std::vector<size_t> *items_index;
    OctreeCell **neighbours;
    OctreeCell *children[8];
    OctreeCell *parent;

    size_t GetLayer();
    void SetNeighbour(unsigned short direction, OctreeCell *cell);

public:
    OctreeCell(bool isLeaf, OctreeCell *parent);
    void Subdivide(size_t layer);
    void Link(size_t layer);
    bool IsLeaf();
    OctreeCell *GetChildren(unsigned short index);
    std::vector<size_t> *GetIndexContainer();

    friend std::ostream &operator<<(std::ostream &os, const OctreeCell &o)
    {
        os << "Neighbours: " << std::endl;
        for (unsigned short i = 0; i < 6; i++)
        {
            std::cout << "\t[" << i << "] " << o.neighbours[i] << std::endl;
        }
        std::cout << "Parent: " << o.parent << std::endl;
        std::cout << "Item index: " << o.items_index->size() << std::endl;
        return os;
    }
};

// generics can be a bit funny about having separate defintion and implementations

template <class T>
class Octree
{
private:
    OVector3 origin;
    OVector3 bounds;
    size_t n_layers;
    size_t n_containers;
    std::vector<struct Storable<T>> items;
    OctreeCell *root;

    inline void SpaceReduce(OVector3 &origin, OVector3 &bounds, unsigned short index);

public:
    Octree<T>(OVector3 origin, OVector3 bounds, size_t n_layers);
    ~Octree<T>();
    OctreeCell *FindContainer(OVector3 target);
    std::vector<T *> GetFromPosition(OVector3 target);
    bool Put(T data, OVector3 position);
    size_t Size();
    bool IsStorable(OVector3 target);

    friend std::ostream &operator<<(std::ostream &os, const Octree<T> &o)
    {
        os << "origin: " << o.origin << std::endl;
        os << "bounds: " << o.bounds << std::endl;
        os << "n_layers: " << o.n_layers << std::endl;
        os << "n_containers: " << o.n_containers << std::endl;
        os << "n_items: " << o.items.size();
        return os;
    }
};

template <class T>
bool Octree<T>::IsStorable(OVector3 target)
{
    return !(target < origin || target >= (origin + bounds));
}

template <class T>
Octree<T>::Octree(OVector3 origin, OVector3 bounds, size_t n_layers) : origin(origin), bounds(bounds), n_layers(n_layers)
{
    root = new OctreeCell(n_layers == 0, NULL);
    root->Subdivide(n_layers);
    root->Link(n_layers);
    n_containers = pow(8, n_layers);
    items = std::vector<struct Storable<T>>();
}

template <class T>
Octree<T>::~Octree()
{
    delete (root);
}

template <class T>
inline void Octree<T>::SpaceReduce(OVector3 &origin, OVector3 &bounds, unsigned short index)
{
    bounds.OVec3Mult(0.5);
    origin.x += ((index >> 2) % 2 != 0) ? bounds.x : 0;
    origin.y += ((index >> 1) % 2 != 0) ? bounds.y : 0;
    origin.z += ((index >> 0) % 2 != 0) ? bounds.z : 0;
}

template <class T>
OctreeCell *Octree<T>::FindContainer(OVector3 target)
{
    if (!IsStorable(target))
    {
        return NULL;
    }

    OVector3 cons_origin = origin;
    OVector3 cons_bounds = bounds;

    OctreeCell *cell = root;
    unsigned short index = 0;

    while (!cell->IsLeaf())
    {
        index = (target.x > (cons_origin.x + (cons_bounds.x / 2.0f)));
        index = index << 1;
        index += (target.y > (cons_origin.y + (cons_bounds.y / 2.0f)));
        index = index << 1;
        index += (target.z > (cons_origin.z + (cons_bounds.z / 2.0f)));

        SpaceReduce(cons_origin, cons_bounds, index);
        cell = cell->GetChildren(index);
    }

    return cell;
}

template <class T>
std::vector<T *> Octree<T>::GetFromPosition(OVector3 target)
{
    std::vector<T *> found_items = std::vector<T *>(0);

    OctreeCell *cell = FindContainer(target);
    if (cell)
    {
        if (cell->IsLeaf())
        {
            for (size_t i = 0; i < cell->GetIndexContainer()->size(); i++)
            {
                size_t index = cell->GetIndexContainer()->at(i);
                if (items.at(index).position == target)
                    found_items.push_back(&(items.at(index).data));
            }
        }
    }
    return found_items;
}

template <class T>
size_t Octree<T>::Size()
{
    return items.size();
}

template <class T>
bool Octree<T>::Put(T data, OVector3 position)
{
    OctreeCell *cell = FindContainer(position);
    if (cell)
    {
        struct Storable<T> stored =
        {
            data, position
        };
        items.push_back(stored);
        cell->GetIndexContainer()->push_back(Size() - 1);
        return true;
    }
    return false;
}

#endif //OCTREE_HPP