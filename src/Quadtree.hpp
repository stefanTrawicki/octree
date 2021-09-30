#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <iostream>
#include <vector>
#include <math.h>

class QVector2
{
public:
    double x;
    double z;

    friend bool operator==(const QVector2 &l, const QVector2 &r)
    {
        return (l.x == r.x && l.z == r.z);
    }

    friend bool operator<(const QVector2 &l, const QVector2 &r)
    {
        return (l.x < r.x && l.z < r.z);
    }
    friend bool operator<=(const QVector2 &l, const QVector2 &r)
    {
        return (l < r || l == r);
    }
    friend bool operator>(const QVector2 &l, const QVector2 &r)
    {
        return (l.x > r.x && l.z > r.z);
    }
    friend bool operator>=(const QVector2 &l, const QVector2 &r)
    {
        return (l > r || l == r);
    }
    friend bool operator!=(const QVector2 &l, const QVector2 &r)
    {
        return !(l == r);
    }
    friend std::ostream &operator<<(std::ostream &os, const QVector2 &o)
    {
        os << "{" << o.x << ", " << o.z << "}";
        return os;
    }
    friend QVector2 operator+(QVector2 lhs, const QVector2 &rhs)
    {
        lhs += rhs;
        return lhs;
    }
    QVector2 &operator+=(const QVector2 &rhs)
    {
        x += rhs.x;
        z += rhs.z;
        return *this;
    }
    QVector2 &operator*=(const QVector2 &rhs)
    {
        x *= rhs.x;
        z *= rhs.z;
        return *this;
    }
};

template <class T>
struct Storable
{
    T data;
    QVector2 position;
};

template <class T>
class Quadtree;

template <class T>
class QuadtreeCell
{
private:
    QuadtreeCell *neighbours[2];
    QuadtreeCell *parent;
    QuadtreeCell *children[4];
    unsigned short index;

public:
    QuadtreeCell(bool isLeaf, QuadtreeCell *parent, unsigned short index);
    void Subdivide(size_t layer);
    void Link(size_t layer);
    size_t Size();
    bool IsLeaf();
    QuadtreeCell *GetChildren(unsigned short index);
    QuadtreeCell* GetNeighbour(unsigned short index);
    std::vector<struct Storable<T> *> *items;
    void SetNeighbour(unsigned short direction, QuadtreeCell *cell);

    friend std::ostream &operator<<(std::ostream &os, const QuadtreeCell &o)
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
        return os;
    }
};

// generics can be a bit funny about having separate defintion and implementations

template <class T>
class Quadtree
{
private:
    QVector2 origin;
    QVector2 bounds;
    size_t n_layers;
    size_t n_containers;
    QuadtreeCell<T> *root;

public:
    Quadtree<T>(QVector2 origin, QVector2 bounds, size_t n_layers);
    QuadtreeCell<T> *FindContainer(QVector2 target);
    bool Put(T data, QVector2 position);
    inline bool IsStorable(QVector2 target);
    std::vector<struct Storable<T> *> Get(QVector2 lower_bound, QVector2 upper_bound);

    friend std::ostream &operator<<(std::ostream &os, const Quadtree<T> &o)
    {
        os << "origin: " << o.origin << std::endl;
        os << "bounds: " << o.bounds << std::endl;
        os << "n_layers: " << o.n_layers << std::endl;
        os << "n_containers: " << o.n_containers << std::endl;
        return os;
    }
};

template <class T>
inline bool Quadtree<T>::IsStorable(QVector2 target)
{
    return !(target < origin || target >= (origin + bounds));
}

template <class T>
Quadtree<T>::Quadtree(QVector2 origin, QVector2 bounds, size_t n_layers) : origin(origin), bounds(bounds), n_layers(n_layers)
{
    root = new QuadtreeCell<T>(n_layers == 0, root, 0);
    root->Subdivide(n_layers);
    root->Link(n_layers);
    n_containers = pow(4, n_layers);
}

template <class T>
QuadtreeCell<T> *Quadtree<T>::FindContainer(QVector2 target)
{
    if (!IsStorable(target))
    {
        return NULL;
    }

    QVector2 cons_origin = origin;
    QVector2 cons_bounds = bounds;

    QuadtreeCell<T> *cell = root;
    unsigned short index = 0;

    while (!cell->IsLeaf())
    {
        cons_bounds *= QVector2{0.5, 0.5};
        index = (target.x > (cons_origin.x + cons_bounds.x));
        index = index << 1;
        index += (target.z > (cons_origin.z + cons_bounds.z));

        cons_origin.x += ((index >> 1) % 2 != 0) ? cons_bounds.x : 0;
        cons_origin.z += ((index >> 0) % 2 != 0) ? cons_bounds.z : 0;

        cell = cell->GetChildren(index);
    }

    return cell;
}

template <class T>
bool Quadtree<T>::Put(T data, QVector2 position)
{
    QuadtreeCell<T> *cell = FindContainer(position);
    if (cell)
    {
        struct Storable<T> *store_this = (struct Storable<T> *)malloc(sizeof(struct Storable<T>));
        store_this->data = data;
        store_this->position = position;

        if (!cell->items)
            cell->items = new std::vector<struct Storable<T> *>;

        cell->items->push_back(store_this);

        return true;
    }
    return false;
}

template <class T>
std::vector<struct Storable<T> *> Quadtree<T>::Get(QVector2 lower_bound, QVector2 upper_bound)
{
    if (!(IsStorable(lower_bound) || IsStorable(upper_bound))) return std::vector<struct Storable<T> *>(0);
    
    // will be a specific case to add later
    // if (upper_bound == lower_bound) return 0;

    if (lower_bound > upper_bound)
    {
        QVector2 temp = lower_bound;
        lower_bound = upper_bound;
        upper_bound = temp;
    }

    QuadtreeCell<T> *points[3] = {NULL, NULL, NULL};

    points[0] = FindContainer(lower_bound);
    if (!points[0]) return std::vector<struct Storable<T> *>(0);

    // in x dim
    points[1] = FindContainer(QVector2{lower_bound.x + upper_bound.x, lower_bound.z});
    // in z dim
    points[2] = FindContainer(QVector2{lower_bound.x, lower_bound.z + upper_bound.z});

    if (!points[1] || !points[2]) return std::vector<struct Storable<T> *>(0);

    QuadtreeCell<T> *windows[2] = {points[0], points[0]};

    std::vector<struct Storable<T> *> results = std::vector<struct Storable<T> *>(0);

    size_t count = 0;

    while(windows[1] != points[2]->GetNeighbour(1))
    {
        while(windows[0] != points[1]->GetNeighbour(0))
        {
            count++;
            if (windows[0]->items)
            {
                for (struct Storable<T> *i : *(windows[0]->items))
                {
                    if (i->position >= lower_bound && i->position < upper_bound)
                    {
                        results.push_back(i);
                    }
                }
            }
            windows[0] = windows[0]->GetNeighbour(0);
        }
        // std::cout << "\ty" << std::endl;
        windows[0] = points[0];
        windows[1] = windows[1]->GetNeighbour(1);
    }

    return results;

}

template <class T>
QuadtreeCell<T>::QuadtreeCell(bool isLeaf, QuadtreeCell<T> *parent, unsigned short index) : parent(parent), index(index)
{
    children[0] = NULL;
    neighbours[0] = NULL;
}

template <class T>
bool QuadtreeCell<T>::IsLeaf()
{
    return children[0] == NULL;
}

template <class T>
void QuadtreeCell<T>::Subdivide(size_t layer)
{

    if (layer > 0)
    {
        // creating children
        for (unsigned short i = 0; i < 4; i++)
        {
            children[i] = new QuadtreeCell(layer - 1 == 0, this, i);
            children[i]->Subdivide(layer - 1);
        }
    }
}

template <class T>
void QuadtreeCell<T>::Link(size_t layer)
{
    for (unsigned short child_i = 0; child_i < 4; child_i++)
    {
        QuadtreeCell<T> *child = children[child_i];

        for (unsigned short i = 0; i < 2; i++)
        {
            // given dimensions 0 = x, 1 = y, 2 = z
            unsigned short neighbour = child->index & (2 >> i) ? child->index - (2 >> i) : child->index + (2 >> i);

            // if the index cannot be increased in this dimension without expanding into other container
            if ((child->index & (2 >> i)) > 0)
            {
                // std::cout << "will be remote!" << std::endl;
                // is remote
                if (neighbours[i])
                {            
                    child->neighbours[i] = neighbours[i]->children[neighbour%4];
                }
                else
                {
                    child->neighbours[i] = NULL;
                }
            }
            else
            {        
                // is local
                child->neighbours[i] = children[neighbour%4];
            }
            
        }
    }

    if (layer > 1)
    {
        for (unsigned short child_i = 0; child_i < 4; child_i++)
        {
            children[child_i]->Link(layer-1);
        }
    }
}

template <class T>
QuadtreeCell<T> *QuadtreeCell<T>::GetChildren(unsigned short index)
{
    return children[index];
}

template <class T>
QuadtreeCell<T> *QuadtreeCell<T>::GetNeighbour(unsigned short index)
{
    if (index < 0 || index > 1) return NULL;

    return neighbours[index];
}

#endif //Quadtree_HPP