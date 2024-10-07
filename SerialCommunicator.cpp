#include "SerialCommunicator.h"

#include <QDebug>
#include <QBitArray>
#include <QDataStream>
#include <ReportUnit.h>

SerialCommunicator::SerialCommunicator()
    : serialPort(std::make_unique<QSerialPort>())
{
    connect(this->serialPort.get(), &QSerialPort::readyRead, this, &SerialCommunicator::onReadyRead, Qt::QueuedConnection);
    connect(this->serialPort.get(), &QSerialPort::errorOccurred, this, &SerialCommunicator::onErrorOccured, Qt::QueuedConnection);
    Q_EMIT ledColorChanged(LedColor::GRAY);
}

bool SerialCommunicator::isConnected() const
{
    return this->serialPort->isOpen();
}

void SerialCommunicator::onSetPortOpen(const bool open)
{
    if (this->serialPort->isOpen() && !open) {
        this->serialPort->close();
        Q_EMIT ledColorChanged(LedColor::GRAY);
        Q_EMIT disconnected();
    }
    else if (!this->serialPort->isOpen() && open) {
        if (this->serialPort->setBaudRate(QSerialPort::Baud115200) &&
                this->serialPort->setDataBits(QSerialPort::Data8)      &&
                this->serialPort->setParity(QSerialPort::NoParity)     &&
                this->serialPort->setStopBits(QSerialPort::OneStop)    &&
                this->serialPort->setFlowControl(QSerialPort::NoFlowControl))
        {
            if (serialPort->open(QIODevice::ReadWrite) && serialPort->setDataTerminalReady(true)) {
                Q_EMIT ledColorChanged(LedColor::GREEN);
                Q_EMIT connected();
                Q_EMIT queryVersion();
            }
        }
    }
}

void SerialCommunicator::onSetConnectionTarget(const QString &target)
{
    if (this->serialPort->portName() != target) {
        this->serialPort->setPortName(target);
    }
}

void SerialCommunicator::onSendToPort(const QString &message)
{
    if (!this->serialPort->isOpen())
        return;

    this->serialPort->write(message.toStdString().c_str(), message.size());
}

void SerialCommunicator::onReadyRead()
{
    QByteArray data;
    while (this->serialPort->canReadLine()) {
        data = this->serialPort->readLine();

        // Magic number 26 is a compressed input array size
        // if (data.size() < 26) {
        if (data.size() < 22) {
            continue;
        }

        ReportUnit report;

        /* Extract float values */
        QByteArray&& ba_flow = data.left(4);
        data.remove(0, 4);

        // QByteArray&& ba_resistance = data.left(4);
        // data.remove(0, 4);

        QByteArray&& ba_fill_value = data.left(4);
        data.remove(0, 4);

        QByteArray&& ba_temp1 = data.left(4);
        data.remove(0, 4);

        QByteArray&& ba_temp2 = data.left(4);
        data.remove(0, 4);

        memcpy(&report.flow, ba_flow.data(), sizeof(float));
        // memcpy(&report.resistance, ba_resistance.data(), sizeof(float));
        memcpy(&report.fill_value, ba_fill_value.data(), sizeof(float));
        memcpy(&report.temp1, ba_temp1.data(), sizeof(float));
        memcpy(&report.temp2, ba_temp2.data(), sizeof(float));

        report.resistance = report.fill_value / report.flow;

        /* Extract time */
        QByteArray&& ba_time = data.left(3);
        data.remove(0, 3);

        report.time.setHMS(ba_time[0], ba_time[1], ba_time[2]);

        uint8_t packed_byte = data.front();
        data.remove(0, 1);

        report.regime = (Regime)(packed_byte & 0b111);
        report.kidney_selector = (packed_byte >> 3) & 0b1;
        report.is_blocked = (packed_byte >> 4) & 0b1;

        uint8_t alert_byte = data.front();

        for (uint8_t i = 0; i < report.alert.size(); i++) {
            report.alert[i] = (alert_byte >> i) & 0b1;
        }

        Q_EMIT newReport(report);
    }
}

void SerialCommunicator::onErrorOccured(const int error)
{
    if (error != QSerialPort::NoError) {
        Q_EMIT errorOccured(this->serialPort->errorString());
        Q_EMIT ledColorChanged(LedColor::RED);
    }
}
