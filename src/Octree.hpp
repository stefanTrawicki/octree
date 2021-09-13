#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>

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
    unsigned layer;

    Octree<S> *master;

    OctreeInternal<S> *parent = {0};
    OctreeInternal<S> *children = {0};

public:
    size_t size()
    {
        return n_items;
    }

    S* GetItems()
    {
        return items;
    }

    bool BoundsCheck(OVector3 point)
    {
        return point >= (origin + bounds) || point < origin;
    }

    friend ostream &operator<<(ostream &os, const OctreeInternal &o)
    {
        os << "origin: " << o.origin << " bounds: " << o.bounds << " layer: " << o.layer << " parent: " << o.parent;
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

            master->n_containers_total += 8;
        }
    }

    OctreeInternal *FindContainer(OVector3 target)
    {
        if (target >= (children[7].origin + children[7].bounds))
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

    size_t insert(S data, OVector3 position, unsigned subdivision_amount)
    {
        if (size() >= subdivision_amount)
        {
            this->Subdivide();
            OctreeInternal<S> *new_container;
            for (size_t i = 0; i < this->size(); i++)
            {
                new_container = this->FindContainer(position);
                new_container->insert(this->items[i], position, subdivision_amount);
            }
            free(this->items);
            this->n_items = 0;
            master->n_items_total -= subdivision_amount;
        }

        this->items[this->n_items++] == data;
        master->n_items_total++;

        return this->n_items;
    }

    OctreeInternal<S>() {}

    OctreeInternal<S>(Octree<S> *master, OctreeInternal<S> *parent, OVector3 bounds, OVector3 origin) : master(master), parent(parent), bounds(bounds), origin(origin)
    {
        items = (S *)malloc(sizeof(S) * master->subdivision_amount);
        n_items = 0;
        if (!parent) layer = 0;
        else layer = parent->layer + 1;
    }
};

template <class T>
class Octree
{

private:
    OctreeInternal<T> root;

public:
    size_t subdivision_amount;
    size_t n_items_total;
    size_t n_containers_total;

    Octree<T>(size_t subdivision_amount, OVector3 bounds, OVector3 origin) : subdivision_amount(subdivision_amount)
    {
        n_items_total = 0;
        n_containers_total = 0;
        root = OctreeInternal<T>(this, NULL, bounds, origin);
        root.Subdivide();
    }

    size_t put(T data, OVector3 position)
    {
       
        if (root.BoundsCheck(position))
            return 0;

        OctreeInternal<T> *container = root.FindContainer(position);

        if (container)
            return container->insert(data, position, subdivision_amount);
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
        os << "Total containers: " << o.n_containers_total;
        return os;
    }
};

#endif //OCTREE_HPP