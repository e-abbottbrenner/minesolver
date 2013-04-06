#ifndef MINEDISPLAY_H
#define MINEDISPLAY_H

#include <QGraphicsObject>

class MineDisplay : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MineDisplay(QObject *parent = 0);

    static const int MineSize;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;
    
public slots:
    
};

#endif // MINEDISPLAY_H
