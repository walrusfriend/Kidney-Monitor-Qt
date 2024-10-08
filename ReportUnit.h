#ifndef REPORTUNIT_H
#define REPORTUNIT_H

#include <QString>
#include <QTime>
#include <QDebug>
#include <array>

#include "Data_Types.h"

class ReportUnit {
public:
    float flow;
    float resistance;
    float fill_value;
    float temp1;
    float temp2;
    QTime time;
    Regime regime;
    bool kidney_selector;
    bool is_blocked;

    std::array<bool, 8> alert;
};

QDebug operator<<(QDebug debug, const ReportUnit &reportUnit);

Q_DECLARE_METATYPE(ReportUnit);

#endif // REPORTUNIT_H
