#include "CellDisplay.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

#include "Minefield.h"
#include "Solver.h"

static QSharedPointer<Solver> solver;

const int CellDisplay::CellSize = 20;

CellDisplay::CellDisplay(Minefield *mineData, int x, int y, QGraphicsItem *parent) :
    QGraphicsObject(parent), minefield(mineData), x(x), y(y)
{
    setX(x * CellSize);
    setY(y * CellSize);
    setToolTip(QString::number(x) + ", " + QString::number(y));

    if(!solver)
    {
        solver = solver.create(minefield);
    }

    setAcceptHoverEvents(true);
}

QRectF CellDisplay::boundingRect() const
{
    return QRectF(0, 0, CellSize, CellSize);
}

void CellDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();

    MineStatus status = minefield->getCell(x, y);

    if(status == SpecialStatus::Unknown)
    {
        painter->setBrush(Qt::gray);
    }
    else
    {
        painter->setBrush(Qt::lightGray);
    }
    painter->setPen(Qt::darkGray);

    painter->drawRect(boundingRect());

    painter->save();

    int margin = CellSize / 10;
    QRectF statusRect = boundingRect().adjusted(margin, margin, -margin, -margin);

    switch(status)
    {
    case SpecialStatus::Mine:
        painter->setBrush(Qt::red);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    case SpecialStatus::Unknown:
        break;
    case SpecialStatus::GuessMine:
        painter->setBrush(Qt::yellow);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    case SpecialStatus::GuessClear:
        painter->setBrush(Qt::green);
        painter->setPen(Qt::black);
        painter->drawEllipse(statusRect);
        break;
    case 0:
        break;
    default:
        painter->setBrush(Qt::black);
        painter->setPen(Qt::black);
        painter->drawText(statusRect, QString::number(status), QTextOption(Qt::AlignCenter));
        break;
    }

    painter->restore();

    painter->restore();
}

void CellDisplay::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        event->accept();
    }
}

void CellDisplay::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(minefield->getCell(x, y) != SpecialStatus::GuessMine)
        {
            minefield->revealCell(x, y);

            QTimer::singleShot(0, this, [&] ()
            {
                solver.reset(new Solver(minefield));

                solver->computeSolution();
            });
        }
    }
    else if(event->button() == Qt::RightButton)
    {
        minefield->toggleCellFlag(x, y);
    }
}

void CellDisplay::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    minefield->revealAdjacents(x, y);
}

void CellDisplay::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    double mineChance = solver->getChancesToBeMine().value({x, y}, 0);

    setToolTip(QString::number(x) + ", " + QString::number(y) + " mine chance: " + QString::number(mineChance));
}
