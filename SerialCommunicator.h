#ifndef SERIALCOMMUNICATOR_H
#define SERIALCOMMUNICATOR_H

#include <memory>

#include <QObject>
#include <QSerialPort>

#include <QThread>
#include <Data_Types.h>

class SerialCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunicator();

    bool isConnected() const;

    Q_SIGNAL void connected();
    Q_SIGNAL void disconnected();
    Q_SIGNAL void errorOccured(const QString& error);
//    Q_SIGNAL void newReport(const ArincReportReceive& report);
    Q_SIGNAL void ledColorChanged(const LedColor& color);
    Q_SIGNAL void queryVersion();
    Q_SIGNAL void setVersion(const QString& version);
//    Q_SIGNAL void dataReceived(const ArincReportReceive& report, const uint8_t& channel_number);
    Q_SIGNAL void updateReceiverSettings(const QString&);
    Q_SIGNAL void updateTransmitterSettings(const QString&);

    Q_SLOT void onSetPortOpen(const bool connect);
    Q_SLOT void onSetConnectionTarget(const QString& target);
    Q_SLOT void onSendToPort(const QString& message);

    std::unique_ptr<QSerialPort> serialPort;
private:

    Q_SLOT void onReadyRead();
    Q_SLOT void onErrorOccured(const int error);
};

#endif // SERIALCOMMUNICATOR_H
