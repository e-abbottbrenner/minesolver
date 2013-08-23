#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MinefieldData;
class MinefieldDisplay;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    MinefieldData *fieldData;
    MinefieldDisplay *fieldDisplay;
};

#endif // MAINWINDOW_H
