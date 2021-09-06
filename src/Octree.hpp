#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <iostream>
#include <vector>

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

    double operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            cout << "Invalid index (0-3 accepted)" << endl;
            return __DBL_MAX__;
        }
    }

    OVector3(double x, double y, double z) : x(x), y(y), z(z){};
};

template <class T>
class Octree
{

private:
    T* items;
    unsigned long n_items;

public:
    Octree<T>() {
        n_items = 0;
        items = (T*)(malloc(sizeof(T) * 24));
    }

    bool put(T data, OVector3 position);
    T get(OVector3 base, double radius);
    unsigned long size();
};

template<class T>
unsigned long Octree<T>::size()
{
    return n_items;
}

#endif //OCTREE_HPP