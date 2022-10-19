#ifndef MINESTATUS_H
#define MINESTATUS_H

#include <QtGlobal>

typedef qint8 MineStatus;

namespace SpecialStatus
{
    const MineStatus Mine = -1;
    const MineStatus Unknown = -2;
    const MineStatus GuessMine = -3;
    const MineStatus Visited = -4;
}

#endif // MINESTATUS_H
