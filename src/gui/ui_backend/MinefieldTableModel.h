#ifndef MINEFIELDTABLEMODEL_H
#define MINEFIELDTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFutureWatcher>
#include <QPair>
#include <QSharedPointer>
#include <QtQml/QtQml>

class Minefield;
class Solver;

typedef QPair<int, int> Coordinate;

class MinefieldTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("QML Cannot invoke constructor")

    Q_PROPERTY(bool recalculationInProgress READ isRecalculationInProgress NOTIFY recalculationInProgressChanged)
    Q_PROPERTY(int maxRecalculationProgress READ getMaxRecalculationProgress NOTIFY maxRecalculationProgressChanged)
    Q_PROPERTY(int currentRecalculationProgress READ getCurrentRecalculationProgress NOTIFY currentRecalculationProgressChanged)
    Q_PROPERTY(QString recalculationStep READ getRecalculationStep NOTIFY recalculationStepChanged)
    Q_PROPERTY(int logLegalFieldCount READ getLogLegalFieldCount NOTIFY logLegalFieldCountChanged)

public:
    enum DataRoles
    {
        CountRole = Qt::UserRole,
        GuessMineRole,
        MineRole,
        ChanceToBeMineRole,
        ChoiceColumnCountRole,
        SolverPathIndexRole,
    };

    explicit MinefieldTableModel(QObject *parent = nullptr);

    void setMinefield(QSharedPointer<Minefield> minefield);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    bool isRecalculationInProgress() const;

    int getMaxRecalculationProgress() const;

    int getCurrentRecalculationProgress() const;

    const QString &getRecalculationStep() const;

    int getLogLegalFieldCount() const;

signals:
    void recalculationInProgressChanged(bool recalculating);
    void maxRecalculationProgressChanged(int max);
    void currentRecalculationProgressChanged(int progress);
    void recalculationStepChanged(const QString& step);
    void logLegalFieldCountChanged(int logFieldCount);

public slots:
    void reveal(int row, int col);
    void revealAdjacent(int row, int col);
    void toggleGuessMine(int row, int col);

    void revealOptimalCell();

private:
    QSharedPointer<Minefield> minefield;

    QSharedPointer<QFutureWatcher<void>> mineChancesCalculationWatcher;
    QSharedPointer<Solver> activeSolver;
    QSharedPointer<Solver> finishedSolver;

    bool recalculationInProgress = false;

    int maxRecalculationProgress = 0;
    int currentRecalculationProgress = 0;
    QString recalculationStep;

    QList<QMetaObject::Connection> recalcProgressConnections;

    void calculateChances();

    void emitUpdateSignalForCoords(QList<Coordinate> coords);

    void applyCalculationResults();

    void setRecalculationInProgress(bool recalculation);
    void setMaxRecalculationProgress(int newMaxRecalculationProgress);
    void setCurrentRecalculationProgress(int newCurrentRecalculationProgress);
    void setRecalculationStep(const QString &newRecalculationStep);

    void setActiveSolver(QSharedPointer<Solver> solver);
};

Q_DECLARE_METATYPE(MinefieldTableModel)

#endif // MINEFIELDTABLEMODEL_H
