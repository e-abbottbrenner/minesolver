#ifndef MINESTATUS_H
#define MINESTATUS_H

#include <QtGlobal>

typedef qint8 MineStatus;

namespace SpecialStatus
{
    const MineStatus Invalid = -1;
    const MineStatus Mine = -2;
    const MineStatus Unknown = -3;
    const MineStatus GuessMine = -4;
    const MineStatus Visited = -5;
    const MineStatus UnexplodedMine = -6;
    const MineStatus ExplodedMine = -7;
}

#endif // MINESTATUS_H
