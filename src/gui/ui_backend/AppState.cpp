#include "AppState.h"

#include "Minefield.h"

#include <QRandomGenerator>

AppState::AppState()
{
    minefieldModel = new MinefieldTableModel(this);

    createNewMinefield(99, 30, 16);
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
