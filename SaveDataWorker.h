#ifndef SAVEDATAWORKER_H
#define SAVEDATAWORKER_H

#include <QObject>
#include "ReportUnit.h"

class SaveDataWorker : public QObject
{
    Q_OBJECT

public:
    explicit SaveDataWorker(QObject *parent = nullptr);

    Q_SIGNAL void finished();

    Q_SLOT void saveData(const QString &file_name, const QVector<ReportUnit> &history);
};

#endif // SAVEDATAWORKER_H
