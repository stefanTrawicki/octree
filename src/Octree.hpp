#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>

#define XOR(a, b) ((a + b) % 2)

using namespace std;

#define MAPPINGS { {4, 2, 1}, {5, 3, 0}, {6, 0, 3}, {7, 1, 2}, {0, 6, 5}, {1, 7, 4}, {2, 4, 7}, {3, 5, 6} }
#define DIRECTION_MAPPINGS { {0, 1, 2}, {0, 1, 5}, {0, 4, 2}, {0, 4, 5}, {3, 1, 2}, {3, 1, 5}, {3, 4, 2}, {3, 4, 5} }

inline void FindNeighbourIndexes(unsigned short index, unsigned short *neighbours)
{
    for (unsigned short i = 0; i < 3; i++)
    {
        neighbours[2-i] = (index + (!((index >> i) % 2) ? 1 << i : -1 << i)) % 8;
    }
}

class OVector3
{
public:
    double x;
    double y;
    double z;

    friend bool operator==(const OVector3 &l, const OVector3 &r)
    {
        return (l.x == r.x && l.y == r.y && l.z == r.z);
    }

    friend bool operator<(const OVector3 &l, const OVector3 &r)
    {
        return (l.x < r.x && l.y < r.y && l.z < r.z);
    }

    friend bool operator<=(const OVector3 &l, const OVector3 &r)
    {
        return (l < r || l == r);
    }

    friend bool operator>(const OVector3 &l, const OVector3 &r)
    {
        return (l.x > r.x && l.y > r.y && l.z > r.z);
    }

    friend bool operator>=(const OVector3 &l, const OVector3 &r)
    {
        return (l > r || l == r);
    }

    friend bool operator!=(const OVector3 &l, const OVector3 &r)
    {
        return !(l == r);
    }

    friend ostream &operator<<(ostream &os, const OVector3 &o)
    {
        os << "{" << o.x << ", " << o.y << ", " << o.z << "}";
        return os;
    }
    OVector3 &operator+=(const OVector3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    friend OVector3 operator+(OVector3 lhs, const OVector3 &rhs)
    {
        lhs += rhs;
        return lhs;
    }
};

template <class V>
class Octree;

template <class S>
class OctreeCell
{
private:
    OVector3 origin;
    OVector3 bounds;

    unsigned short index = 0;

    S *items;
    size_t n_items;
    size_t layer;

    OctreeCell<S> *children = 0;
    OctreeCell<S> *neighbours[6];

public:
    OctreeCell<S> *parent = 0;

    size_t size()
    {
        return n_items;
    }

    S *GetItems()
    {
        return items;
    }

    bool BoundsCheck(OVector3 point)
    {
        return point < (origin + bounds) || point >= origin;
    }

    friend ostream &operator<<(ostream &os, const OctreeCell &o)
    {
        os << "origin: " << o.origin << " bounds: " << o.bounds << " layer: " << o.layer << " parent: " << o.parent;
        return os;
    }

    void Subdivide(Octree<S> *master)
    {
        if (!children)
        {
            children = (OctreeCell<S> *)malloc(sizeof(OctreeCell<S>) * 8);

            OVector3 new_bounds = OVector3{bounds.x / 2, bounds.y / 2, bounds.z / 2};

            // a
            children[0] = OctreeCell<S>(this, new_bounds, origin, master->subdivision_amount, 0);
            // b
            children[1] = OctreeCell<S>(this, new_bounds, origin + OVector3{0, 0, new_bounds.z}, master->subdivision_amount, 1);
            // c
            children[2] = OctreeCell<S>(this, new_bounds, origin + OVector3{0, new_bounds.y, 0}, master->subdivision_amount, 2);
            // d
            children[3] = OctreeCell<S>(this, new_bounds, origin + OVector3{0, new_bounds.y, new_bounds.z}, master->subdivision_amount, 3);
            // e
            children[4] = OctreeCell<S>(this, new_bounds, origin + OVector3{new_bounds.x, 0, 0}, master->subdivision_amount, 4);
            // f
            children[5] = OctreeCell<S>(this, new_bounds, origin + OVector3{new_bounds.x, 0, new_bounds.z}, master->subdivision_amount, 5);
            // g
            children[6] = OctreeCell<S>(this, new_bounds, origin + OVector3{new_bounds.x, new_bounds.y, 0}, master->subdivision_amount, 6);
            // h
            children[7] = OctreeCell<S>(this, new_bounds, origin + OVector3{new_bounds.x, new_bounds.y, new_bounds.z}, master->subdivision_amount, 7);

            if (this->layer + 1 > master->lowest_layer)
                master->lowest_layer = this->layer + 1;
            master->n_containers_total += 8;

            unsigned short temp[8][3] = MAPPINGS;
            unsigned short dirs[8][3] = DIRECTION_MAPPINGS;

            for (unsigned short i = 0; i < 8; i++)
            {
                for (unsigned short j = 0; j < 6; j++)
                {
                    if (i%2 == 0 && parent)
                    {
                        if (parent->neighbours)
                        {
                            unsigned short neighbour_index = dirs[i][j%3];
                            OctreeCell *neighbour_container = parent->neighbours[neighbour_index];
                            cout << neighbour_container->index << endl;
                            if (neighbour_container->children)
                            {
                                children[i].neighbours[j] = &neighbour_container->children[neighbour_index];
                            }
                            else
                                children[i].neighbours[j] = neighbour_container;
                        }
                        else
                            children[i].neighbours[j] = NULL;
                    }
                    else if (i%2 != 0)
                    {
                        children[i].neighbours[j] = &children[temp[i][j%3]];
                    }
                    else
                    {
                        children[i].neighbours[j] = NULL;
                    }
                }
            }

            DisplayChildrensNeighbours();

        }
    }

    OctreeCell *FindContainer(OVector3 target)
    {
        if (target > (children[7].origin + children[7].bounds))
            return NULL;

        if (target < children[0].origin)
            return NULL;

        unsigned index = 0;
        index = (target.x > children[4].origin.x);
        index = index << 1;
        index += (target.y > children[2].origin.y);
        index = index << 1;
        index += (target.z > children[1].origin.z);

        if (children[index].children)
            return children[index].FindContainer(target);

        return &children[index];
    }

    size_t insert(S data, OVector3 position, unsigned subdivision_amount, Octree<S> *master)
    {
        if (size() >= subdivision_amount)
        {
            this->Subdivide(master);
            OctreeCell<S> *new_container;
            for (size_t i = 0; i < subdivision_amount; i++)
            {
                new_container = this->FindContainer(position);
                new_container->insert(this->items[i], position, subdivision_amount, master);
            }

            free(this->items);
            this->n_items = 0;
            master->n_items_total -= subdivision_amount;
        }

        this->items[this->n_items++] == data;
        master->n_items_total++;

        return this->n_items;
    }

    OctreeCell<S>() {}

    void DisplayChildrensNeighbours()
    {
        for (unsigned short i = 0; i < 8; i++)
        {
            printf("child[%u]: {%u, %u, %u, %u, %u, %u}\n",
                    i,
                    children[i].neighbours[0]->index,
                    children[i].neighbours[1]->index,
                    children[i].neighbours[2]->index,
                    children[i].neighbours[3]->index,
                    children[i].neighbours[4]->index,
                    children[i].neighbours[5]->index);
        }

    }

    OctreeCell<S>(OctreeCell<S> *parent, OVector3 bounds, OVector3 origin, size_t subdivision_amount, unsigned short index) : parent(parent), bounds(bounds), origin(origin), index(index)
    {
        items = (S *)malloc(sizeof(S) * subdivision_amount);
        n_items = 0;
        if (!parent)
            layer = 0;
        else
            layer = parent->layer + 1;
    }
};

template <class T>
class Octree
{

private:
    OctreeCell<T> root;

public:
    size_t subdivision_amount;
    size_t n_items_total;
    size_t n_containers_total;
    size_t lowest_layer;

    Octree<T>(size_t subdivision_amount, OVector3 bounds, OVector3 origin) : subdivision_amount(subdivision_amount)
    {
        n_items_total = 0;
        n_containers_total = 0;
        lowest_layer = 0;
        root = OctreeCell<T>(NULL, bounds, origin, subdivision_amount, 0);
        root.parent = &root;
        root.Subdivide(this);
    }

    size_t put(T data, OVector3 position)
    {

        if (!root.BoundsCheck(position))
            return 0;

        OctreeCell<T> *container = root.FindContainer(position);

        if (container)
            return container->insert(data, position, subdivision_amount, this);
        else
            return 0;
    }

    T get(OVector3 base, double radius);

    size_t size()
    {
        return n_items_total;
    }

    friend ostream &operator<<(ostream &os, const Octree &o)
    {
        os << "Subdivision Value: " << o.subdivision_amount << endl;
        os << "Total items: " << o.n_items_total << endl;
        os << "Total containers: " << o.n_containers_total << endl;
        os << "Lowest layer: " << o.lowest_layer;

        return os;
    }
};

#endif //OCTREE_HPP