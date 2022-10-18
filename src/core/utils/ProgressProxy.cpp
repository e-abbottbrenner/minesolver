#include "ProgressProxy.h"

ProgressProxy::ProgressProxy()
{

}

void ProgressProxy::emitProgressMaximum(int maximum)
{
    emit progressMaximum(maximum);
}

void ProgressProxy::emitProgressStep(const QString &step)
{
    emit progressStep(step);
}

void ProgressProxy::incrementProgress()
{
    emit progressMade(progress++);
}

void ProgressProxy::reset()
{
    progress = 0;
}
