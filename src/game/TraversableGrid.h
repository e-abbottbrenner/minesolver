#ifndef TRAVERSABLEGRID_H
#define TRAVERSABLEGRID_H

#include <functional>

class TraversableGrid
{
public:
    TraversableGrid(int width, int height);

    bool checkBounds(int x, int y) const;

    // no const on these as we might want to make changes in the passed func
    void traverseAdacentCells(int x, int y, std::function<void (int, int)> func);
    void traverseCells(std::function<void (int, int)> func);

    int getWidth() const;
    int getHeight() const;

private:
    int width = 0;
    int height = 0;
};

#endif // TRAVERSABLEGRID_H
