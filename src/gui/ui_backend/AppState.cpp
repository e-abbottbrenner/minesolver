#include "AppState.h"

#include "Minefield.h"
#include "MinefieldImageProvider.h"

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
    MinefieldImageProvider::instance()->setMinefield(minefield);

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

        checkFieldConfigValid();

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

        checkFieldConfigValid();

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

        checkFieldConfigValid();

        emit minefieldHeightChanged(newMinefieldHeight);
    }
}

bool AppState::isFieldConfigValid() const
{
    return fieldConfigValid;
}

QImage AppState::getMinefieldThumbnail() const
{
    return minefieldThumbnail;
}

bool AppState::getShowInteractiveGameBoard() const
{
    return showInteractiveGameBoard;
}

void AppState::setShowInteractiveGameBoard(bool newShowInteractiveGameBoard)
{
    if (showInteractiveGameBoard == newShowInteractiveGameBoard)
        return;
    showInteractiveGameBoard = newShowInteractiveGameBoard;
    emit showInteractiveGameBoardChanged();
}

void AppState::checkFieldConfigValid()
{
    setFieldConfigValid(mineCount > 0 && mineCount < minefieldWidth * minefieldHeight - 9 && minefieldWidth > 3 && minefieldHeight > 3);
}

void AppState::setFieldConfigValid(bool valid)
{
    if(fieldConfigValid != valid)
    {
        fieldConfigValid = valid;

        emit fieldConfigValidChanged(valid);
    }
}

void AppState::setMinefieldThumbnail(QImage thumbnail)
{
    // don't bother comparing for this one
    minefieldThumbnail = thumbnail;

    emit minefieldThumbnailChanged();
}
