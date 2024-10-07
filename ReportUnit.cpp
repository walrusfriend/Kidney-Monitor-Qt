#include "ReportUnit.h"

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

    return debug;
}
