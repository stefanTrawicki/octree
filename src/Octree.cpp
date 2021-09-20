#include "Octree.hpp"

bool operator==(const OVector3 &l, const OVector3 &r)
{
    return (l.x == r.x && l.y == r.y && l.z == r.z);
}

bool operator<(const OVector3 &l, const OVector3 &r)
{
    return (l.x < r.x && l.y < r.y && l.z < r.z);
}

bool operator<=(const OVector3 &l, const OVector3 &r)
{
    return (l < r || l == r);
}

bool operator>(const OVector3 &l, const OVector3 &r)
{
    return (l.x > r.x && l.y > r.y && l.z > r.z);
}

bool operator>=(const OVector3 &l, const OVector3 &r)
{
    return (l > r || l == r);
}

bool operator!=(const OVector3 &l, const OVector3 &r)
{
    return !(l == r);
}

std::ostream &operator<<(std::ostream &os, const OVector3 &o)
{
    os << "{" << o.x << ", " << o.y << ", " << o.z << "}";
    return os;
}

OVector3 operator+(OVector3 lhs, const OVector3 &rhs)
{
    lhs += rhs;
    return lhs;
}

OVector3 &OVector3::operator+=(const OVector3 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

void OVector3::OVec3Mult(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
}

// octree cell stuff

OctreeCell::OctreeCell(bool isLeaf, OctreeCell *parent, unsigned short index) : parent(parent), index(index)
{
    children[0] = NULL;

    neighbours = (OctreeCell **)malloc(sizeof(OctreeCell *) * 6);

    if (isLeaf)
        items_index = new std::vector<size_t>(0);
}

bool OctreeCell::IsLeaf()
{
    return children[0] == NULL;
}

void OctreeCell::SetNeighbour(unsigned short direction, OctreeCell *cell)
{
    neighbours[direction] = cell;
}

void OctreeCell::Subdivide(size_t layer)
{

    if (layer > 0)
    {
        // creating children
        for (unsigned short i = 0; i < 8; i++)
        {
            children[i] = new OctreeCell(layer - 1 == 0, this, i);
            children[i]->Subdivide(layer - 1);
        }
    }
}

void OctreeCell::Link(size_t layer)
{
    for (unsigned short child_i = 0; child_i < 8; child_i++)
    {
        OctreeCell *child = children[child_i];

        for (unsigned short i = 0; i < 3; i++)
        {
            // given dimensions 0 = x, 1 = y, 2 = z
            unsigned short neighbour = child->index + (4 >> i);
            
            if ((child->index & (4 >> i)) > 0)
            {
                // is remote
                if (neighbours[i])
                {            
                    child->neighbours[i] = neighbours[i]->children[neighbour%8];
                }
                else
                {
                    child->neighbours[i] = NULL;
                }
                // is local
                child->neighbours[i+3] = children[neighbour%8];
            }
            else
            {        
                // is remote
                if (neighbours[i+3])
                {            
                    child->neighbours[i+3] = neighbours[i+3]->children[neighbour%8];
                }
                else
                {
                    child->neighbours[i+3] = NULL;
                }
                // is local
                child->neighbours[i] = children[neighbour%8];
            }
            
        }
    }

    if (layer > 1)
    {
        for (unsigned short child_i = 0; child_i < 8; child_i++)
        {
            children[child_i]->Link(layer-1);
        }
    }
}

OctreeCell *OctreeCell::GetChildren(unsigned short index)
{
    return children[index];
}

std::vector<size_t> *OctreeCell::GetIndexContainer()
{
    return items_index;
}