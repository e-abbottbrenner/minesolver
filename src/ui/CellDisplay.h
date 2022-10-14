#ifndef CELLDISPLAY_H
#define CELLDISPLAY_H

#include <QGraphicsObject>

class Minefield;

class CellDisplay : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CellDisplay(Minefield *mineData, int x, int y, QGraphicsItem *parent = 0);

    static const int CellSize;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:
    Minefield *minefield = nullptr;
    int x = 0;
    int y = 0;
    

    // QGraphicsItem interface
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
};

#endif // CELLDISPLAY_H
