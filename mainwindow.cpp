#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <set>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>

#include <QRandomGenerator>

MainWindow::MainWindow(QVector<ReportUnit>& h, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_communicator(new SerialCommunicator())
    // , m_communicatorThread(new QThread(this))
    , m_graph_tab(std::make_unique<GraphsTab>())
    , history(h)
    , workerThread(std::make_unique<QThread>())
    , worker(std::make_unique<SaveDataWorker>())
{
    ui->setupUi(this);

    ui->cb_kidney_selector->addItem("Правая");
    ui->cb_kidney_selector->addItem("Левая");

    ui->cb_organ_code->addItem("00: Человек");
    ui->cb_organ_code->addItem("01: Свинья");
    ui->cb_organ_code->addItem("02: Баран");
    ui->cb_organ_code->addItem("03: Кролик");
    ui->cb_organ_code->addItem("04: Другое");

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
    connect(ui->pb_pump_rpm, &QPushButton::clicked,
            this, &MainWindow::onPumpRpmButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_tare_pressure, &QPushButton::clicked,
            this, &MainWindow::onTarePressureButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_perfussion_rate, &QPushButton::clicked,
            this, &MainWindow::onPerfussionSpeedRatioButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_pressure_target, &QPushButton::clicked,
            this, &MainWindow::onPressureTargetValueButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_enulate_bubble, &QPushButton::clicked,
            this, &MainWindow::onEmulateBubbleButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_temp_low_limit, &QPushButton::clicked,
            this, &MainWindow::onLowTempLimitButtonClicked, Qt::QueuedConnection);
    connect(ui->pb_temp_high_limit, &QPushButton::clicked,
            this, &MainWindow::onHighTempLimitButtonClicked, Qt::QueuedConnection);

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

    connect(this, &MainWindow::drawDataToChart,
            m_graph_tab.get(), &GraphsTab::addDataToChart, Qt::QueuedConnection);

    connectCommunicator();

    // m_communicator->moveToThread(m_communicatorThread);
    // m_communicatorThread->start();

    onSyncDateTime();

    /* Add tab with charts */
    ui->tabWidget->addTab(m_graph_tab.get(), "Визуализация");

    /* Fill the combo box with COM ports */
    updateDeviceList();

    test_timer = std::make_unique<QTimer>(this);
    connect(test_timer.get(), &QTimer::timeout, this, &MainWindow::onTestTimerTimeOut, Qt::QueuedConnection);

    backup_timer = std::make_unique<QTimer>(this);
    connect(backup_timer.get(), &QTimer::timeout, this, &MainWindow::onBackupTimerTimeout, Qt::QueuedConnection);

    workerThread->start();
    worker->moveToThread(workerThread.get());

    connect(worker.get(), &SaveDataWorker::finished, workerThread.get(), &QThread::quit);
    connect(this, &MainWindow::sendDataForSaving, worker.get(), &SaveDataWorker::saveData, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    // m_communicatorThread->quit();
    // m_communicatorThread->wait();
    // delete m_communicatorThread;

    workerThread->quit();
    workerThread->wait();

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

void MainWindow::paintAlertLabels(const std::array<bool, 8> &alert)
{
    ui->alert_attention->setStyleSheet("background-color: none;");
    ui->alert_air->setStyleSheet("background-color: none;");
    ui->alert_pressure->setStyleSheet("background-color: none;");
    ui->alert_resistance->setStyleSheet("background-color: none;");
    ui->alert_temp1->setStyleSheet("background-color: none;");
    ui->alert_temp2->setStyleSheet("background-color: none;");

    if (alert[AlertType::PRESSURE_HIGH] or
        alert[AlertType::PRESSURE_LOW]){
        ui->alert_pressure->setStyleSheet("background-color: red;");
        ui->alert_attention->setStyleSheet("background-color: red;");
    }

    if (alert[AlertType::PRESSURE_UP]){
        ui->alert_pressure->setStyleSheet("background-color: yellow;");
        ui->alert_attention->setStyleSheet("background-color: red;");
    }

    if (alert[AlertType::TEMP1_HIGH] or alert[AlertType::TEMP1_LOW]){
        ui->alert_temp1->setStyleSheet("background-color: red;");
        ui->alert_attention->setStyleSheet("background-color: red;");
    }

    if (alert[AlertType::TEMP2_HIGH] or alert[AlertType::TEMP2_LOW]){
        ui->alert_temp2->setStyleSheet("background-color: red;");
        ui->alert_attention->setStyleSheet("background-color: red;");
    }

    if (alert[AlertType::RESISTANCE]){
        ui->alert_resistance->setStyleSheet("background-color: red;");
        ui->alert_attention->setStyleSheet("background-color: red;");
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

    /* Validate incoming report */
    if (report.fill_value < -100 or report.fill_value > 200) {
        qDebug() << "ERROR: MainWindow::onNewReport() - Wront incoming report message\n";
        return;
    }

    /* Update UI fields */
    ui->le_pressure->setText(QString::number(report.fill_value));
    ui->le_resistance->setText(QString::number(report.resistance));
    ui->le_perfussion_speed->setText(QString::number(report.flow));
    ui->le_temp1->setText(QString::number(report.temp1));
    ui->le_temp2->setText(QString::number(report.temp2));
    ui->le_working_mode->setText(RegimeMap.at(report.regime));
    ui->le_duration->setText(report.time.toString());

    paintAlertLabels(report.alert);

    /* Update graph */
    emit drawDataToChart(report);

    /* Store report in memory */
    history.emplace_back(report);
}

void MainWindow::onStartButtonClicked() {
    // qDebug() << "onStartButtonClicked";
    if (is_new_experiment) {
        history.clear();
        m_graph_tab->clear();
        is_new_experiment = false;
    }

    m_communicator->onSendToPort("start\n");
    // test_timer->start(1000);
    backup_timer->start(1000*60*40);
    // backup_timer->start(1000*10);
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
    is_new_experiment = true;
    // test_timer->stop();
    backup_timer->stop();
}

void MainWindow::onExportResultButtonClicked() {
    // Получаем имя файла, введенное пользователем
    QString file_name = QFileDialog::getSaveFileName(nullptr,
                                                    "Введите имя файла",
                                                    "№" + ui->le_experiment_number_control->text() + '_' +
                                                    synced_date.toString("hh-mm-ss_dd.MM.yyyy") + ".xlsx",
                                                    "Все файлы (*.*)");

    emit sendDataForSaving(file_name, history);
}

void MainWindow::onSyncDateTime() {
    synced_date = QDateTime::currentDateTime();
    ui->le_date_control->setText(synced_date.toString("dd.MM.yyyy hh:mm:ss"));
    emit ui->le_date_control->textChanged(ui->le_date_control->text());
}

void MainWindow::onPumpRpmButtonClicked() {
    m_communicator->onSendToPort("set_speed " + ui->le_pump_rpm->text() + '\n');
}

void MainWindow::onPerfussionSpeedRatioButtonClicked() {
    m_communicator->onSendToPort("set_perfusion_speed_ratio " + ui->le_perfussion_rate->text() + '\n');
}

void MainWindow::onPressureTargetValueButtonClicked() {
    m_communicator->onSendToPort("set_tv " + ui->le_pressure_target->text() + '\n');
}

void MainWindow::onTarePressureButtonClicked() {
    m_communicator->onSendToPort("tare_pressure\n");
}

void MainWindow::onEmulateBubbleButtonClicked()
{
    m_communicator->onSendToPort("emulate_bubble\n");
}

void MainWindow::onBackupTimerTimeout()
{
    qDebug() << "ON BACKUP TIMER";

    QDir dir = QDir::current();

    if (!dir.exists("backups"))
        dir.mkdir("backups");

    QString file_name = dir.currentPath() + "/backups/" + "№" + ui->le_experiment_number_control->text() + '_' +
                        QDateTime::currentDateTime().toString("hh-mm-ss_dd.MM.yyyy")
                        + ".xlsx";

    emit sendDataForSaving(file_name, history);
}

void MainWindow::onLowTempLimitButtonClicked()
{
    m_communicator->onSendToPort("temp_low_limit " + ui->le_temp_low_limit->text() + '\n');
}

void MainWindow::onHighTempLimitButtonClicked()
{
    m_communicator->onSendToPort("temp_high_limit " + ui->le_temp_high_limit->text() + '\n');
}

void MainWindow::onTestTimerTimeOut() {
    ReportUnit report;

    report.fill_value = QRandomGenerator::global()->bounded(20, 40);
    report.flow = QRandomGenerator::global()->bounded(20, 25);
    report.resistance = report.fill_value / report.flow;
    report.temp1 = QRandomGenerator::global()->bounded(3, 10);
    report.temp2 = QRandomGenerator::global()->bounded(3, 10);
    report.time = QTime::currentTime();
    report.regime = Regime::REGIME1;
    report.kidney_selector = true;
    report.is_blocked = false;

    for (bool& error : report.alert)
        error = false;

    for (bool& status : report.peripheral_status)
        status = true;

    onNewReport(report);
}
