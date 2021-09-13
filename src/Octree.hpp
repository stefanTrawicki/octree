#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>

// libraries for testing
#include <unistd.h>

#define PREALLOCATION_SIZE 64

#define XOR(a, b) ((a + b) % 2)

using namespace std;

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
class OctreeInternal
{
private:
    OVector3 origin;
    OVector3 bounds;

    S *items;
    size_t n_items;

    Octree<S> *master;

    OctreeInternal<S> *parent;
    OctreeInternal<S> *children = {0};

public:
    friend ostream &operator<<(ostream &os, const OctreeInternal &o)
    {
        os << "origin: " << o.origin << " bounds: " << o.bounds;
        return os;
    }

    void Subdivide()
    {
        if (!children)
        {
            children = (OctreeInternal<S> *)malloc(sizeof(OctreeInternal<S>) * 8);

            OVector3 new_bounds = OVector3{bounds.x / 2, bounds.y / 2, bounds.z / 2};

            // a
            children[0] = OctreeInternal<S>(master, this, new_bounds, origin);
            // b
            children[1] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{0, 0, new_bounds.z});
            // c
            children[2] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{0, new_bounds.y, 0});
            // d
            children[3] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{0, new_bounds.y, new_bounds.z});
            // e
            children[4] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{new_bounds.x, 0, 0});
            // f
            children[5] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{new_bounds.x, 0, new_bounds.z});
            // g
            children[6] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{new_bounds.x, new_bounds.y, 0});
            // h
            children[7] = OctreeInternal<S>(master, this, new_bounds, origin + OVector3{new_bounds.x, new_bounds.y, new_bounds.z});
        }
    }

    OctreeInternal *FindContainer(OVector3 target)
    {
        if (target >= (children[7].bounds + children[7].origin) || target < children[0].bounds)
            return NULL;

        unsigned index = 0;
        index = (target.x > children[4].origin.x);
        index = index << 1;
        index += (target.y > children[2].origin.y);
        index = index << 1;
        index += (target.z > children[1].origin.z);

        if (children[index])
            return FindContainer(children[index]);

        return &children[index];
    }

    OctreeInternal<S>() {}

    OctreeInternal<S>(Octree<S> *master, OctreeInternal<S> *parent, OVector3 bounds, OVector3 origin) : master(master), parent(parent), bounds(bounds), origin(origin)
    {
        items = (S *)malloc(sizeof(S) * PREALLOCATION_SIZE);
        n_items = 0;
    }
};

template <class T>
class Octree
{

private:
    size_t subdivision_amount;
    size_t n_items_total;
    size_t n_containers_total;

    OctreeInternal<T> root;

public:
    Octree<T>(size_t subdivision_amount, OVector3 bounds, OVector3 origin) : subdivision_amount(subdivision_amount)
    {
        n_items_total = 0;
        n_containers_total = 8;
        root = OctreeInternal<T>(this, NULL, bounds, origin);
        root.Subdivide();
    }

    bool put(T data, OVector3 position)
    {
        return 0;
    }

    T get(OVector3 base, double radius);

    size_t size()
    {
        return n_items_total;
    }

    friend ostream &operator<<(ostream &os, const Octree &o)
    {
        os << "Prealloc Size: " << PREALLOCATION_SIZE << endl;
        os << "Subdivision Value: " << o.subdivision_amount << endl;
        os << "Total items: " << o.n_items_total << endl;
        os << "Total containers: " << o.n_containers_total;
        return os;
    }
};

#endif //OCTREE_HPP