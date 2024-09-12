#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <set>
#include <QSerialPortInfo>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_communicator(new SerialCommunicator())
    , m_communicatorThread(new QThread(this))
{
    ui->setupUi(this);

    connect(this, &MainWindow::setConnectionTarget, this->m_communicator,
            &SerialCommunicator::onSetConnectionTarget, Qt::QueuedConnection);


    connect(ui->cb_device, &QComboBox::currentTextChanged,
            this, &MainWindow::onDeviceComboCurrentTextChanged, Qt::QueuedConnection);
    connect(ui->btn_connect, &QPushButton::clicked,
            this, &MainWindow::onBtnConnectClicked, Qt::QueuedConnection);


    connectCommunicator();

    m_communicator->moveToThread(m_communicatorThread);
    m_communicatorThread->start();


    /* Fill the combo box with COM ports */
    updateDeviceList();
}

MainWindow::~MainWindow()
{
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
    if (this->deviceConnected) {
        Q_EMIT this->setPortOpen(false);
    } else {
        Q_EMIT this->setPortOpen(true);
    }
}

void MainWindow::connectCommunicator()
{
    if (m_communicator) {
//        connect(m_communicatorThread, &QThread::started,
//                m_communicator, &MKIO_CommunicatorBase::process, Qt::QueuedConnection);
        connect(m_communicatorThread, &QThread::finished,
                m_communicator, &QObject::deleteLater, Qt::DirectConnection);
//        connect(m_communicator, &MKIO_CommunicatorBase::initialized,
//                this, &InterfaceModuleBase::onCommunicatorInitialized, Qt::QueuedConnection);

        connect(this, &MainWindow::setPortOpen,
                m_communicator, &SerialCommunicator::onSetPortOpen,
                Qt::QueuedConnection);
        connect(this, &MainWindow::setConnectionTarget,
                m_communicator, &SerialCommunicator::onSetConnectionTarget,
                Qt::QueuedConnection);

        connect(m_communicator, &SerialCommunicator::connected
                , this, &MainWindow::onDeviceConnected, Qt::QueuedConnection);
        connect(m_communicator, &SerialCommunicator::disconnected,
                this, &MainWindow::onDeviceDisconnected, Qt::QueuedConnection);
        connect(m_communicator, &SerialCommunicator::ledColorChanged,
                this, &::MainWindow::onLedColorChanged, Qt::QueuedConnection);
//        connect(m_communicator, &SerialCommunicator::newReport,
//                this, &InterfaceModuleBase::onNewReport, Qt::QueuedConnection);
//        connect(m_communicator, &MKIO_CommunicatorBase::errorOccured,
//                this, &InterfaceModuleBase::onCommunicationErrorOccured, Qt::QueuedConnection);
    }
}

void MainWindow::disconnectCommunicator() {
    if (m_communicator) {
//        disconnect(m_communicatorThread, &QThread::started,
//                   m_communicator, &MKIO_CommunicatorBase::process);
        disconnect(m_communicatorThread, &QThread::finished,
                   m_communicator, &QObject::deleteLater);
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
                   this, &::MainWindow::onLedColorChanged);
//        disconnect(m_communicator, &MKIO_CommunicatorBase::newReport,
//                   this, &InterfaceModuleBase::onNewReport);
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
