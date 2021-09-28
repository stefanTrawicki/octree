#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>

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
    OctreeCell *parent;
    OctreeCell *children[8];
    unsigned short index;
    size_t layer;

    size_t GetLayer();
    void SetNeighbour(unsigned short direction, OctreeCell *cell);

public:
    OctreeCell(bool isLeaf, OctreeCell *parent, unsigned short index);
    void Subdivide(size_t layer);
    void Link(size_t layer);
    bool IsLeaf();
    size_t Size();
    OctreeCell *GetChildren(unsigned short index);
    unsigned short GetIndex();
    OctreeCell* GetNeighbour(unsigned short index);
    std::vector<size_t> *GetIndexContainer();

    friend std::ostream &operator<<(std::ostream &os, const OctreeCell &o)
    {
        os << "This: " << &o << std::endl;
        os << "Index: " << o.index << std::endl;
        os << "Neighbours: " << std::endl;
        for (unsigned short i = 0; i < 6; i++)
        {
            if (o.neighbours)
            {
                std::cout << "\t[" << i << "] " << o.neighbours[i];
                if (o.neighbours[i])
                    std::cout << " ind: " << o.neighbours[i]->index;
                std::cout << std::endl;
            }
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
    size_t GetFromArea(OVector3 lower_bound, OVector3 upper_bound, std::vector<struct Storable<T>> *results);

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
    root = new OctreeCell(n_layers == 0, root, 0);
    root->Subdivide(n_layers);
    root->Link(n_layers);
    n_containers = 8;
    for (size_t i = 0; i < n_layers; i++)
        n_containers *= 8;
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

template <class T>
size_t Octree<T>::GetFromArea(OVector3 lower_bound, OVector3 upper_bound, std::vector<struct Storable<T>> *results)
{
    if (!(IsStorable(lower_bound) && IsStorable(upper_bound))) return 0;
    
    if (upper_bound == lower_bound) return 0;
    if (lower_bound > upper_bound)
    {
        OVector3 temp = lower_bound;
        lower_bound = upper_bound;
        upper_bound = temp;
    }

    OctreeCell *points[4] = {NULL, NULL, NULL, NULL};

    points[0] = FindContainer(lower_bound);
    if (!points[0]) return 0;

    // in x dim
    points[1] = FindContainer(OVector3{lower_bound.x + upper_bound.x, lower_bound.y, lower_bound.z});
    // in y dim
    points[2] = FindContainer(OVector3{lower_bound.x, lower_bound.y + upper_bound.y, lower_bound.z});
    // in z dim
    points[3] = FindContainer(OVector3{lower_bound.x, lower_bound.y, lower_bound.z + upper_bound.z});

    if (!points[1] || !points[2] || !points[3]) return 0;

    OctreeCell *windows[3] = {points[0], points[0], points[0]};

    size_t count = 0;

    while(windows[2] != points[3]->GetNeighbour(2))
    {
        while(windows[1] != points[2]->GetNeighbour(1))
        {
            while(windows[0] != points[1]->GetNeighbour(0))
            {
                // std::cout << "\t\tx" << std::endl;
                for (auto const& i : *(windows[0]->GetIndexContainer()))
                {
                    results->push_back(items.at(i));                    
                }
                count++;
                windows[0] = windows[0]->GetNeighbour(0);
            }
            // std::cout << "\ty" << std::endl;
            windows[0] = points[0];
            windows[1] = windows[1]->GetNeighbour(1);
        }
        // std::cout << "z" << std::endl;
        windows[1] = points[0];
        windows[2] = windows[2]->GetNeighbour(2);
    }

    return count;

}

#endif //OCTREE_HPP