#include "ReportUnit.h"

QTextStream& operator<<(QTextStream& stream, const ReportUnit& reportUnit) {
    stream << "\nflow: " << reportUnit.flow
           << "\nresistance: " << reportUnit.resistance
           << "\nfill_value: " << reportUnit.fill_value
           << "\ntemp1: " << reportUnit.temp1
           << "\ntemp2: " << reportUnit.temp2
           << "\ntime: " << reportUnit.time.toString()
           << "\nregime: " << static_cast<int>(reportUnit.regime)
           << "\nkidney_selector: " << ((reportUnit.kidney_selector) ? "left" : "right")
           << "\nis_blocked: " << reportUnit.is_blocked
           << "\nalerts: [";

    for (size_t i = 0; i < reportUnit.alert.size(); ++i) {
        stream << (reportUnit.alert[i] ? "true" : "false");
        if (i < reportUnit.alert.size() - 1) {
            stream << ", ";
        }
    }

    stream << "])\n";
    stream << "peripheral status: [";

    for (size_t i = 0; i < reportUnit.peripheral_status.size(); ++i) {
        stream << (reportUnit.peripheral_status[i] ? "true" : "false");

        if (i < reportUnit.peripheral_status.size() - 1)
            stream << ", ";

    }
    stream << "])\n";

    return stream;
}

QDebug operator<<(QDebug debug, const ReportUnit &reportUnit) {
    QDebugStateSaver saver(debug); // Сохранить текущее состояние

    debug.nospace() << "\nflow: " << reportUnit.flow
                    << "\nresistance: " << reportUnit.resistance
                    << "\nfill_value: " << reportUnit.fill_value
                    << "\ntemp1: " << reportUnit.temp1
                    << "\ntemp2: " << reportUnit.temp2
                    << "\ntime: " << reportUnit.time.toString()
                    << "\nregime: " << static_cast<int>(reportUnit.regime)
                    << "\nkidney_selector: " << ((reportUnit.kidney_selector) ? "left" : "right")
                    << "\nis_blocked: " << reportUnit.is_blocked
                    << "\nalerts: [";

    for (size_t i = 0; i < reportUnit.alert.size(); ++i) {
        debug.nospace() << (reportUnit.alert[i] ? "true" : "false");
        if (i < reportUnit.alert.size() - 1) {
            debug.nospace() << ", "; // Добавляем запятую между элементами
        }
    }

    debug.nospace() << "])\n";
    debug.nospace() << "peripheral status: [";

    for (size_t i = 0; i < reportUnit.peripheral_status.size(); ++i) {
        debug.nospace() << (reportUnit.peripheral_status[i] ? "true" : "false");

        if (i < reportUnit.peripheral_status.size() - 1)
            debug.nospace() << ", ";

    }
    debug.nospace() << "])\n";

    return debug;
}
