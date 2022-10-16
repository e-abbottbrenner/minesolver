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

public:
    AppState();

    MinefieldTableModel *getMinefieldModel() const;

    Q_INVOKABLE void createNewMinefield(int mineCount, int width, int height);

signals:
    void minefieldChanged();

private:
    QSharedPointer<Minefield> minefield;

    // exposed to QML so we use Qt's object ownership system instead of smart pointers
    MinefieldTableModel *minefieldModel = nullptr;
};

#endif // APPSTATE_H
