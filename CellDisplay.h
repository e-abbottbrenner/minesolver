#ifndef CELLDISPLAY_H
#define CELLDISPLAY_H

#include <QGraphicsObject>

class MinefieldData;

class CellDisplay : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CellDisplay(MinefieldData *mineData, int x, int y, QGraphicsItem *parent = 0);

    static const int CellSize;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:
    MinefieldData *mineData;
    int x;
    int y;
    
};

#endif // CELLDISPLAY_H
