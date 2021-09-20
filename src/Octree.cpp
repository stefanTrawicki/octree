#include "Octree.hpp"

int index_mappings[8][3] = INDEX_MAPPING;
int direction_mappings[8][3] = DIRECTION_MAPPINGS;

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

OctreeCell::OctreeCell(bool isLeaf, OctreeCell *parent) : parent(parent)
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
            children[i] = new OctreeCell(layer - 1 == 0, this);
            children[i]->Subdivide(layer - 1);
        }
    }
}

void OctreeCell::Link(size_t layer)
{
    for (unsigned i = 0; i < 8; i++)
    {
        // linking neighbours
        unsigned short neighbour_index = 0;
        unsigned short neighbour = 0;
        unsigned short direction = 0;

        std::cout << "layer: " << layer << std::endl;
        std::cout << "index: " << i << std::endl;

        for (unsigned short j = 0; j < 2; j++)
        {
            for (unsigned short k = 0; k < 3; k++)
            {
                neighbour = index_mappings[i][k];
                direction = direction_mappings[i][k];

                std::cout << "n_i: " << neighbour_index <<
                    " n: " << neighbour <<
                    " d: " << direction << std::endl;

                if (j == 1)
                {
                    std::cout << "negative" << std::endl;
                    if (parent)
                    {
                        std::cout << "parent found!" << std::endl;
                        if (parent->neighbours[direction])
                        {
                            std::cout << "found relevant neighbour!" << std::endl;
                            SetNeighbour(neighbour_index++, parent->neighbours[direction]->children[neighbour]);
                        }
                        else
                            SetNeighbour(neighbour_index++, NULL);
                    }
                    else
                    {
                        std::cout << "no parents :(" << std::endl;
                    }
                }
                else if (j == 0)
                {
                    std::cout << "positive" << std::endl;
                    SetNeighbour(neighbour_index++, children[neighbour]);
                    std::cout << "(" << children[neighbour] << ")" << std::endl;
                }
            }
        }

        std::cout << std::endl;
    }

    if (layer > 1)
    {
        for (unsigned i = 0; i < 8; i++)
            children[i]->Link(layer-1);
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