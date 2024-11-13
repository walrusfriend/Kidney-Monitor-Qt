// #ifndef DATAWRITER_H
// #define DATAWRITER_H

// #include <QObject>

// #include <QCoreApplication>
// #include <QVector>
// #include <QFile>
// #include <QTextStream>
// #include <QTimer>
// #include <QDebug>
// #include <QSerialPort>
// #include <QThread>

// #include "ReportUnit.h"

// class DataWriter : public QObject {
//     Q_OBJECT

// signals:
//     void writeToDiskFinished();

// public slots:
//     void writeToDisk(const QVector<ReportUnit>& list) {
//         if (list.isEmpty())
//             return;

//         QFile file("data_dump.txt");
//         if (!file.open(QIODevice::Append | QIODevice::Text)) {
//             qWarning() << "Cannot open file for writing:" << file.errorString();
//             emit writeToDiskFinished();
//             return;
//         }

//         QTextStream out(&file);
//         for (const ReportUnit &data : list) {
//             out << data << "\n";  // Запись данных в файл
//         }

//         file.close();
//         qDebug() << "Data written to disk. Total records:" << list.size();
//         // dataBuffer.clear();  // Очистить буфер после записи
//         emit writeToDiskFinished();
//     }
// };

// class DataLogger : public QObject {
//     Q_OBJECT

// public:
//     DataLogger(QObject *parent = nullptr) : QObject(parent) {
//         // Настройка таймера для записи в файл каждые 5 секунд
//         QTimer *timer = new QTimer(this);
//         connect(timer, &QTimer::timeout, this, &DataLogger::prepareDataForWriting);
//         timer->start(5000);  // 5000 мС = 5 секунд

//         // Создание потока для записи в файл
//         QThread *thread = new QThread;
//         dataWriter = new DataWriter;
//         dataWriter->moveToThread(thread);
//         connect(thread, &QThread::started, dataWriter, &DataWriter::writeToDisk);
//         connect(this, &DataLogger::requestWrite, dataWriter, &DataWriter::writeToDisk);
//         connect(dataWriter, &DataWriter::writeToDiskFinished, thread, &QThread::quit);
//         connect(dataWriter, &DataWriter::writeToDiskFinished, dataWriter, &DataWriter::deleteLater);
//         connect(thread, &QThread::finished, thread, &QThread::deleteLater);
//         thread->start();
//     }

//     ~DataLogger() {
//         serialPort->close();  // Закрыть COM-порт
//         emit requestWrite();  // Записать оставшиеся данные перед завершением
//     }

// private slots:
//     void readFromPort() {
//         while (serialPort->canReadLine()) {
//             QByteArray line = serialPort->readLine().trimmed();
//             QString data = QString::fromUtf8(line);
//             dataBuffer.append(data);  // Добавление данных в буфер
//             qDebug() << "Received data:" << data;
//         }
//     }

//     void prepareDataForWriting() {
//         if (!dataBuffer.isEmpty()) {
//             emit requestWrite();
//         }
//     }

// signals:
//     void requestWrite();

// private:
//     QVector<QString> dataBuffer;  // Буфер данных в оперативной памяти
//     QSerialPort *serialPort;       // COM-порт
//     DataWriter *dataWriter;         // Объект для записи данных
// };

// #endif // DATAWRITER_H
