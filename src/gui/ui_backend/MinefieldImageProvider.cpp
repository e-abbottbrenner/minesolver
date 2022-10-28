#include "MinefieldImageProvider.h"

#include "Minefield.h"

void cleanupImageBytes(void *bytes)
{
    delete[] reinterpret_cast<uchar*>(bytes);
}

MinefieldImageProvider::MinefieldImageProvider()
    : QQuickImageProvider(ImageType::Image)
{

}

MinefieldImageProvider *MinefieldImageProvider::instance()
{
    // TODO: Qt takes ownership, this isn't really "safe" as a singleton
    static MinefieldImageProvider* inst = new MinefieldImageProvider;

    return inst;
}

void MinefieldImageProvider::setMinefield(QSharedPointer<Minefield> minefield)
{
    this->minefield = minefield;
}

QImage MinefieldImageProvider::requestImage(const QString &/*id*/, QSize *size, const QSize &/*requestedSize*/)
{
    if(minefield)
    {
        QImage image(minefield->getWidth(), minefield->getHeight(), QImage::Format_Indexed8);

        image.setColor(static_cast<uchar>(SpecialStatus::Unknown), qRgb(128, 128, 128));
        image.setColor(static_cast<uchar>(SpecialStatus::GuessMine), qRgb(255, 0, 255));
        image.setColor(static_cast<uchar>(SpecialStatus::Mine), qRgb(255, 0, 0));
        image.setColor(static_cast<uchar>(SpecialStatus::UnexplodedMine), qRgb(255, 255, 0));

        for(int i = 0; i <= 8; ++i)
        {
            image.setColor(i, qRgb(0, 255 - 128 / (i + 1), 0));
        }

        *size = image.size();

        for(int x = 0; x < minefield->getWidth(); ++x)
        {
            for(int y = 0; y < minefield->getHeight(); ++y)
            {
                image.setPixel(x, y, static_cast<uchar>(minefield->getCell(x, y)));
            }
        }

        return image;
    }

    return {};
}
