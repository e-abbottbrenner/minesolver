#include "TraversableGrid.h"

TraversableGrid::TraversableGrid(int width, int height)
    : width(width), height(height)
{

}

bool TraversableGrid::checkBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

void TraversableGrid::traverseAdacentCells(int x, int y, std::function<void (int, int)> func)
{
    for(int i = -1; i <= 1; ++i)
    {
        for(int j = -1; j <= 1; ++j)
        {
            if(checkBounds(x + i, y + j))
            {
                func(x + i, y + j);
            }
        }
    }
}

void TraversableGrid::traverseCells(std::function<void (int, int)> func)
{
    for(int x = 0; x < width; ++x)
    {
        for(int y = 0; y < height; ++y)
        {
            func(x, y);
        }
    }
}

int TraversableGrid::getWidth() const
{
    return width;
}

int TraversableGrid::getHeight() const
{
    return height;
}
