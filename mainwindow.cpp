#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <set>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_communicator(new SerialCommunicator())
    // , m_communicatorThread(new QThread(this))
    , m_graph_tab(std::make_unique<GraphsTab>())
{
    ui->setupUi(this);

    ui->cb_kidney_selector->addItem("Правая");
    ui->cb_kidney_selector->addItem("Левая");

    ui->cb_organ_code->addItem("01 (Человек)");
    ui->cb_organ_code->addItem("02 (Мыш))");

    onSyncDateTime();


    connect(this, &MainWindow::setConnectionTarget, this->m_communicator,
            &SerialCommunicator::onSetConnectionTarget, Qt::QueuedConnection);


    connect(ui->btn_connect, &QPushButton::clicked,
            this, &MainWindow::onBtnConnectClicked, Qt::QueuedConnection);
    connect(ui->pb_start, &QPushButton::clicked,
            this, &MainWindow::onStartButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_pause, &QPushButton::clicked,
            this, &MainWindow::onPauseButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_regime_antibubble, &QPushButton::clicked,
            this, &MainWindow::onRegimeAntibubbleButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_regime_perfusion, &QPushButton::clicked,
            this, &MainWindow::onRegimePerfusionButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_stop, &QPushButton::clicked,
            this, &MainWindow::onStopButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_export_result, &QPushButton::clicked,
            this, &MainWindow::onExportResultButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_sync_date, &QPushButton::clicked,
            this, &MainWindow::onSyncDateTime, Qt::QueuedConnection);

    connect(ui->cb_device, &QComboBox::currentTextChanged,
            this, &MainWindow::onDeviceComboCurrentTextChanged, Qt::QueuedConnection);
    connect(ui->cb_kidney_selector, &QComboBox::currentTextChanged,
            ui->le_kindey_selector, &QLineEdit::setText, Qt::QueuedConnection);
    connect(ui->cb_organ_code, &QComboBox::currentTextChanged,
            ui->le_organ_code, &QLineEdit::setText, Qt::QueuedConnection);

    connect(ui->le_date_control, &QLineEdit::textChanged,
            ui->le_date_param, &QLineEdit::setText, Qt::QueuedConnection);
    connect(ui->le_experiment_number_control, &QLineEdit::textChanged,
            ui->le_experiment_number_param, &QLineEdit::setText, Qt::QueuedConnection);

    connectCommunicator();

    // m_communicator->moveToThread(m_communicatorThread);
    // m_communicatorThread->start();

    /* Add tab with charts */
    ui->tabWidget->addTab(m_graph_tab.get(), "Визуализация");

    /* Fill the combo box with COM ports */
    updateDeviceList();
}

MainWindow::~MainWindow()
{
    // m_communicatorThread->quit();
    // m_communicatorThread->wait();
    // delete m_communicatorThread;
    delete ui;
}

void MainWindow::updateDeviceList()
{
    std::set<QString> devices;
    for (const auto& info : QSerialPortInfo::availablePorts())
    {
        if (info.hasVendorIdentifier()) {
            devices.insert(info.portName());
        }
    }

    ui->cb_device->clear();

    if (devices.empty()) {
        ui->cb_device->addItem("");
        ui->cb_device->addItem("Refresh");
        ui->cb_device->setCurrentText("");
    }
    else {
        for (const auto& device : devices)
        {
            ui->cb_device->addItem(device);
        }
        ui->cb_device->addItem("Refresh");
        ui->cb_device->setCurrentIndex(0);
    }
}

void MainWindow::onDeviceComboCurrentTextChanged(const QString &text)
{
    if (text == "Refresh") {
        updateDeviceList();
    }
    else if (!text.isEmpty()) {
        Q_EMIT setConnectionTarget(text);
    }
}

void MainWindow::onBtnConnectClicked()
{
    if (deviceConnected) {
        Q_EMIT setPortOpen(false);
    } else {
        Q_EMIT setPortOpen(true);
    }
}

void MainWindow::connectCommunicator()
{
    if (m_communicator) {
//        connect(m_communicatorThread, &QThread::started,
//                m_communicator, &MKIO_CommunicatorBase::process, Qt::QueuedConnection);
        // connect(m_communicatorThread, &QThread::finished,
        //         m_communicator, &QObject::deleteLater, Qt::DirectConnection);
//        connect(m_communicator, &MKIO_CommunicatorBase::initialized,
//                this, &InterfaceModuleBase::onCommunicatorInitialized, Qt::QueuedConnection);

        connect(this, &MainWindow::setPortOpen,
                m_communicator, &SerialCommunicator::onSetPortOpen,
                Qt::QueuedConnection);
        connect(this, &MainWindow::setConnectionTarget,
                m_communicator, &SerialCommunicator::onSetConnectionTarget,
                Qt::QueuedConnection);

        connect(m_communicator, &SerialCommunicator::connected,
                this, &MainWindow::onDeviceConnected, Qt::QueuedConnection);
        connect(m_communicator, &SerialCommunicator::disconnected,
                this, &MainWindow::onDeviceDisconnected, Qt::QueuedConnection);
        connect(m_communicator, &SerialCommunicator::ledColorChanged,
                this, &MainWindow::onLedColorChanged, Qt::QueuedConnection);
        connect(m_communicator, &SerialCommunicator::newReport,
                this, &MainWindow::onNewReport, Qt::QueuedConnection);
//        connect(m_communicator, &MKIO_CommunicatorBase::errorOccured,
//                this, &InterfaceModuleBase::onCommunicationErrorOccured, Qt::QueuedConnection);
    }
}

void MainWindow::disconnectCommunicator() {
    if (m_communicator) {
//        disconnect(m_communicatorThread, &QThread::started,
//                   m_communicator, &MKIO_CommunicatorBase::process);
        // disconnect(m_communicatorThread, &QThread::finished,
        //            m_communicator, &QObject::deleteLater);
//        disconnect(m_communicator, &MKIO_CommunicatorBase::initialized,
//                   this, &InterfaceModuleBase::onCommunicatorInitialized);

        disconnect(this, &MainWindow::setPortOpen,
                   m_communicator, &SerialCommunicator::onSetPortOpen);
        disconnect(this, &MainWindow::setConnectionTarget,
                   m_communicator, &SerialCommunicator::onSetConnectionTarget);

        disconnect(m_communicator, &SerialCommunicator::connected,
                   this, &MainWindow::onDeviceConnected);
        disconnect(m_communicator, &SerialCommunicator::disconnected,
                   this, &MainWindow::onDeviceDisconnected);
        disconnect(m_communicator, &SerialCommunicator::ledColorChanged,
                   this, &MainWindow::onLedColorChanged);
        disconnect(m_communicator, &SerialCommunicator::newReport,
                    this, &MainWindow::onNewReport);
//        disconnect(m_communicator, &MKIO_CommunicatorBase::errorOccured,
//                   this, &InterfaceModuleBase::onCommunicationErrorOccured);
    }
}

void MainWindow::onDeviceConnected() {
    if (ui->cb_device->currentText() != "" && ui->cb_device->currentText() != "Refresh") {
        ui->btn_connect->setText("Отключиться");
        ui->cb_device->setEnabled(false);
        deviceConnected = true;
    }
}

void MainWindow::onDeviceDisconnected() {
    ui->btn_connect->setText("Подключиться");
    onLedColorChanged(LedColor::GRAY);
    ui->cb_device->setEnabled(true);
    deviceConnected = false;
}

void MainWindow::onLedColorChanged(LedColor color)
{
    if (color == m_ledColor) {
        return;
    }

    if (color == GREEN)
    {
        ui->LED_connect->setStyleSheet("background-color : #30F030;"
                                      "border-radius : 12px;"
                                      "border-width: 3px;"
                                      "border-color: #808080;"
                                      "border-style: outset;");
    }
    else if (color == RED)
    {
        ui->LED_connect->setStyleSheet("background-color : #F03030;"
                                      "border-radius : 12px;"
                                      "border-width: 3px;"
                                      "border-color: #808080;"
                                      "border-style: outset;");
    }
    else
    {
        ui->LED_connect->setStyleSheet("background-color : gray;"
                                      "border-radius : 12px;"
                                      "border-width: 3px;"
                                      "border-color: #808080;"
                                      "border-style: outset;");
    }
    m_ledColor = color;
}

void MainWindow::onNewReport(const ReportUnit& report) {
    qDebug() << report;

    ui->le_pressure->setText(QString::number(report.fill_value));
    ui->le_resistance->setText(QString::number(report.resistance));
    ui->le_perfussion_speed->setText(QString::number(report.flow));
    ui->le_temp1->setText(QString::number(report.temp1));
    ui->le_temp2->setText(QString::number(report.temp2));

    ui->le_working_mode->setText(RegimeMap.at(report.regime));

    ui->le_duration->setText(report.time.toString());

    history.emplace_back(report);
}

void MainWindow::onStartButtonClicked() {
    // qDebug() << "onStartButtonClicked";
    m_communicator->onSendToPort("start\n");
}

void MainWindow::onPauseButtonClicked() {
    // qDebug() << "onPauseButtonClicked";
    m_communicator->onSendToPort("pause\n");
}

void MainWindow::onRegimeAntibubbleButtonClicked() {
    // qDebug() << "onRegimeAntibubbleButtonClicked";
    m_communicator->onSendToPort("regime 2\n");
}

void MainWindow::onRegimePerfusionButtonClicked() {
    // qDebug() << "onRegimePerfusionButtonClicked";
    m_communicator->onSendToPort("regime 1\n");
}

void MainWindow::onStopButtonClicked() {
    // qDebug() << "onStopButtonClicked";
    m_communicator->onSendToPort("stop\n");
}

void MainWindow::onExportResultButtonClicked() {
    // qDebug() << "onExportResultButtonClicked";

    for (const ReportUnit& unit : history)
        qDebug() << unit;
}

void MainWindow::onSyncDateTime() {
    const QDateTime&& date = QDateTime::currentDateTime();
    ui->le_date_control->setText(date.toString("dd.MM.yyyy hh:mm:ss"));
}
