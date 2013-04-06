#ifndef MINEFIELDDISPLAY_H
#define MINEFIELDDISPLAY_H

#include <QGraphicsObject>

class MinefieldData;
class CellDisplay;

class MinefieldDisplay : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MinefieldDisplay(MinefieldData *fieldData, QGraphicsItem *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;

    ~MinefieldDisplay();

private slots:
    void onCellRevealed(int x, int y);

private:
    MinefieldData *mineData;

    int pixelWidth;
    int pixelHeight;

    int fieldWidth;
    int fieldHeight;

    CellDisplay*** cellDisplays;
};

#endif // MINEFIELDDISPLAY_H
