#include "MainWindow.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDateTime>

#include "Minefield.h"
#include "MinefieldDisplay.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    fieldData = new Minefield(10, 10, 10, QDateTime::currentMSecsSinceEpoch(), this);
    fieldDisplay = new MinefieldDisplay(fieldData);

    QGraphicsView *view = new QGraphicsView(this);
    setCentralWidget(view);

    QGraphicsScene *scene = new QGraphicsScene(this);
    view->setScene(scene);

    scene->addItem(fieldDisplay);

    show();

    view->fitInView(scene->itemsBoundingRect());
}

MainWindow::~MainWindow()
{
    
}
