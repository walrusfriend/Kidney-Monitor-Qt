#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>

#include "SerialCommunicator.h"
#include "GraphsTab.h"
#include "SaveDataWorker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    Q_SIGNAL void setConnectionTarget(const QString& target);
    Q_SIGNAL void setPortOpen(const bool opened);

private:
    Ui::MainWindow *ui;

    SerialCommunicator* m_communicator;
    // QThread* m_communicatorThread;
    std::unique_ptr<GraphsTab> m_graph_tab;

    bool deviceConnected = false;

    LedColor m_ledColor { GRAY };

    bool is_new_experiment = true;
    QVector<ReportUnit> history;

    std::unique_ptr<QTimer> test_timer;

    std::unique_ptr<QTimer> backup_timer;
    QDateTime synced_date;

    std::unique_ptr<QThread> workerThread;
    std::unique_ptr<SaveDataWorker> worker;

private:
    void connectCommunicator();
    void disconnectCommunicator();
    void paintAlertLabels(const std::array<bool, 8>& alert);

    Q_SIGNAL void drawDataToChart(const ReportUnit& unit);
    Q_SIGNAL void sendDataForSaving(const QString& file_name,
                                    const QVector<ReportUnit> &history);

    Q_SLOT void updateDeviceList();
    Q_SLOT void onDeviceComboCurrentTextChanged(const QString &text);
    Q_SLOT void onBtnConnectClicked();
    Q_SLOT void onDeviceConnected();
    Q_SLOT void onDeviceDisconnected();
    Q_SLOT void onLedColorChanged(LedColor color);
    Q_SLOT void onNewReport(const ReportUnit& report);

    Q_SLOT void onStartButtonClicked();
    Q_SLOT void onPauseButtonClicked();
    Q_SLOT void onRegimeAntibubbleButtonClicked();
    Q_SLOT void onRegimePerfusionButtonClicked();
    Q_SLOT void onStopButtonClicked();
    Q_SLOT void onExportResultButtonClicked();
    Q_SLOT void onSyncDateTime();
    Q_SLOT void onPumpRpmButtonClicked();
    Q_SLOT void onPerfussionSpeedRatioButtonClicked();
    Q_SLOT void onPressureTargetValueButtonClicked();
    Q_SLOT void onTarePressureButtonClicked();
    Q_SLOT void onEmulateBubbleButtonClicked();
    Q_SLOT void onBackupTimerTimeout();

    /* Debug slots */
    Q_SLOT void onTestTimerTimeOut();
};
#endif // MAINWINDOW_H
