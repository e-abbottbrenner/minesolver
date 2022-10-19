#include "AppState.h"

#include "Minefield.h"

#include <QRandomGenerator>

AppState::AppState()
{
    minefieldModel = new MinefieldTableModel(this);

    createNewMinefield(300, 50, 50);
}

MinefieldTableModel *AppState::getMinefieldModel() const
{
    return minefieldModel;
}

void AppState::createNewMinefield(int mineCount, int width, int height)
{
    minefield = minefield.create(mineCount, width, height, QRandomGenerator::global()->generate());

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
