#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include "SerialCommunicator.h"

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

    bool deviceConnected = false;

    LedColor m_ledColor { GRAY };

    QVector<ReportUnit> history;

private:
    void connectCommunicator();
    void disconnectCommunicator();
    void drawTestChart();

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
};
#endif // MAINWINDOW_H
