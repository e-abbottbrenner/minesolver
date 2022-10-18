#ifndef PROGRESSPROXY_H
#define PROGRESSPROXY_H

#include <QObject>

class ProgressProxy : public QObject
{
    Q_OBJECT
public:
    explicit ProgressProxy();

    void emitProgressMaximum(int maximum);
    void emitProgressStep(const QString& step);
    void incrementProgress();

    void reset();

signals:
    void progressMaximum(int max);
    void progressMade(int progress);
    void progressStep(const QString& step);

private:
    int progress = 0;
};

#endif // PROGRESSPROXY_H
