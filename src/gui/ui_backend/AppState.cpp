#include "AppState.h"

#include "Minefield.h"

#include <QRandomGenerator>

AppState::AppState()
{
    minefieldModel = new MinefieldTableModel(this);

    createNewMinefield();
}

MinefieldTableModel *AppState::getMinefieldModel() const
{
    return minefieldModel;
}

void AppState::createNewMinefield()
{
    minefield = minefield.create(mineCount, minefieldWidth, minefieldHeight, QRandomGenerator::global()->generate());

    minefieldModel->setMinefield(minefield);

    emit minefieldChanged();
}

bool AppState::getShowSolution() const
{
    return showSolution;
}

void AppState::setShowSolution(bool show)
{
    if(showSolution != show)
    {
        showSolution = show;
        emit showSolutionChanged(show);
    }
}

int AppState::getMineCount() const
{
    return mineCount;
}

void AppState::setMineCount(int newMineCount)
{
    if(mineCount != newMineCount)
    {
        mineCount = newMineCount;

        emit mineCountChanged(newMineCount);
    }
}

int AppState::getMinefieldWidth() const
{
    return minefieldWidth;
}

void AppState::setMinefieldWidth(int newMinefieldWidth)
{
    if(minefieldWidth != newMinefieldWidth)
    {
        minefieldWidth = newMinefieldWidth;

        emit minefieldWidthChanged(newMinefieldWidth);
    }
}

int AppState::getMinefieldHeight() const
{
    return minefieldHeight;
}

void AppState::setMinefieldHeight(int newMinefieldHeight)
{
    if(minefieldHeight != newMinefieldHeight)
    {
        minefieldHeight = newMinefieldHeight;

        emit minefieldHeightChanged(newMinefieldHeight);
    }
}
