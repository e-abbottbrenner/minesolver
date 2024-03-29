#ifndef MINEFIELDTABLEMODEL_H
#define MINEFIELDTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFutureWatcher>
#include <QPair>
#include <QSharedPointer>
#include <QtQml/QtQml>

class AutoPlayer;
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
    Q_PROPERTY(double bestMineChance READ getBestMineChance NOTIFY bestMineChanceChanged)
    Q_PROPERTY(bool autoSolve WRITE setAutoSolve READ getAutoSolve NOTIFY autoSolveChanged)
    Q_PROPERTY(double cumulativeRiskOfLoss READ getCumulativeRiskOfLoss NOTIFY cumulativeRiskOfLossChanged)
    Q_PROPERTY(bool gameWon READ getGameWon NOTIFY gameWonChanged)
    Q_PROPERTY(bool gameLost READ getGameLost NOTIFY gameLostChanged)
    Q_PROPERTY(int flagsRemaining READ getFlagsRemaining WRITE setFlagsRemaining NOTIFY flagsRemainingChanged)

public:
    enum DataRoles
    {
        CountRole = Qt::UserRole,
        GuessMineRole,
        MineRole,
        ExplodedMineRole,
        UnexplodedMineRole,
        ChanceToBeMineRole,
        ChoiceColumnCountRole,
        SolverPathIndexRole,
    };

    explicit MinefieldTableModel(QObject *parent = nullptr);
    ~MinefieldTableModel() override;

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

    Q_INVOKABLE int roleForName(const QString& roleName) const;

    Q_INVOKABLE QModelIndex nullIndex() const;

    double getBestMineChance() const;

    void setAutoSolve(bool newAutoSolve);
    bool getAutoSolve() const;

    double getCumulativeRiskOfLoss() const;

    bool getGameWon() const;

    bool getGameLost() const;

    int getFlagsRemaining() const;
    void setFlagsRemaining(int newFlagsRemaining);

signals:
    void recalculationInProgressChanged(bool recalculating);
    void maxRecalculationProgressChanged(int max);
    void currentRecalculationProgressChanged(int progress);
    void recalculationStepChanged(const QString& step);
    void logLegalFieldCountChanged(int logFieldCount);
    void bestMineChanceChanged(double mineChance);
    void cumulativeRiskOfLossChanged(double risk);
    void gameWonChanged(bool won);
    void gameLostChanged(bool lost);
    void autoSolveChanged(bool autoSolve);

    void flagsRemainingChanged(int flagsRemaining);

    void newMinefieldStarted();

public slots:
    void reveal(int row, int col, bool force = false);
    void revealAdjacent(int row, int col);
    void toggleGuessMine(int row, int col);

    void revealLowestRiskCells();
    void flagGuaranteedMines();

private slots:
    void onMineHit();
    void onAllCountCellsRevealed();

    void onCellToggled(int x, int y);
    void onCellsRevealed(QList<Coordinate> revealed);

private:
    QSharedPointer<Minefield> minefield;

    QSharedPointer<Solver> finishedSolver;

    AutoPlayer* autoPlayer = nullptr;

    bool recalculationInProgress = false;

    int maxRecalculationProgress = 0;
    int currentRecalculationProgress = 0;
    QString recalculationStep;

    double bestMineChance = 0;

    double cumulativeRiskOfLoss = 0;

    bool gameWon = false;
    bool gameLost = false;

    int flagsRemaining;

    int dataChangedMinRow = std::numeric_limits<int>::max();
    int dataChangedMinCol = std::numeric_limits<int>::max();
    int dataChangedMaxRow = 0;
    int dataChangedMaxCol = 0;

    bool dataChangedPending = false;

    void onCalculationStarted();

    void emitUpdateSignalForCoords(QList<Coordinate> coords);
    void prepareDataChanged(int minRow, int minCol, int maxRow, int maxCol);
    void deliverDataChanged();

    void applyCalculationResults(QSharedPointer<Solver> solver);

    void updateRiskOfLoss(double additionalRisk);

    void setRecalculationInProgress(bool recalculation);
    void setMaxRecalculationProgress(int newMaxRecalculationProgress);
    void setCurrentRecalculationProgress(int newCurrentRecalculationProgress);
    void setRecalculationStep(const QString &newRecalculationStep);
    void setCumulativeRiskOfLoss(double risk);
    void setBestMineChance(double chance);
    void setGameWon(bool won);
    void setGameLost(bool lost);

    void setActiveSolver(QSharedPointer<Solver> solver);
};

Q_DECLARE_METATYPE(MinefieldTableModel)

#endif // MINEFIELDTABLEMODEL_H
