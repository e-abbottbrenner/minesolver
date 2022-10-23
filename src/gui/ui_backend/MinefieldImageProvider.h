#ifndef MINEFIELDIMAGEPROVIDER_H
#define MINEFIELDIMAGEPROVIDER_H

#include <QQuickImageProvider>

class Minefield;

class MinefieldImageProvider : public QQuickImageProvider
{
public:
    static MinefieldImageProvider *instance();

    void setMinefield(QSharedPointer<Minefield> minefield);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    MinefieldImageProvider();

    QSharedPointer<Minefield> minefield;
};

#endif // MINEFIELDIMAGEPROVIDER_H
