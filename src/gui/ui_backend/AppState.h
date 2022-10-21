#ifndef APPSTATE_H
#define APPSTATE_H

#include <QObject>
#include <QtQml/QtQml>

// needed for Q_PROPERTY DECLARATION BELOW
#include "MinefieldTableModel.h"

class Minefield;

class AppState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(MinefieldTableModel* minefieldModel READ getMinefieldModel CONSTANT)
    Q_PROPERTY(bool showSolution READ getShowSolution WRITE setShowSolution NOTIFY showSolutionChanged)
    Q_PROPERTY(int mineCount READ getMineCount WRITE setMineCount NOTIFY mineCountChanged)
    Q_PROPERTY(int minefieldWidth READ getMinefieldWidth WRITE setMinefieldWidth NOTIFY minefieldWidthChanged)
    Q_PROPERTY(int minefieldHeight READ getMinefieldHeight WRITE setMinefieldHeight NOTIFY minefieldHeightChanged)
    Q_PROPERTY(bool fieldConfigValid READ isFieldConfigValid NOTIFY fieldConfigValidChanged)

public:
    AppState();

    MinefieldTableModel *getMinefieldModel() const;

    Q_INVOKABLE void createNewMinefield();

    bool getShowSolution() const;
    void setShowSolution(bool show);

    int getMineCount() const;
    void setMineCount(int newMineCount);

    int getMinefieldWidth() const;
    void setMinefieldWidth(int newMinefieldWidth);

    int getMinefieldHeight() const;
    void setMinefieldHeight(int newMinefieldHeight);

    bool isFieldConfigValid() const;

signals:
    void minefieldChanged();
    void showSolutionChanged(bool show);

    void mineCountChanged(int count);
    void minefieldWidthChanged(int width);
    void minefieldHeightChanged(int height);

    void fieldConfigValidChanged(bool valid);

private:
    int mineCount = 99;
    int minefieldWidth = 30;
    int minefieldHeight = 16;

    bool fieldConfigValid = true;

    QSharedPointer<Minefield> minefield;

    bool showSolution = true;

    // exposed to QML so we use Qt's object ownership system instead of smart pointers
    MinefieldTableModel *minefieldModel = nullptr;

    void checkFieldConfigValid();
    void setFieldConfigValid(bool valid);
};

#endif // APPSTATE_H
