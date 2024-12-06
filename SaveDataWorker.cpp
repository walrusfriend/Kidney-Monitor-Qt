#include "SaveDataWorker.h"

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

SaveDataWorker::SaveDataWorker(QObject *parent)
    : QObject{parent}
{}

void SaveDataWorker::saveData(const QString &file_name,
                      const QVector<ReportUnit> &history) {
    qDebug() << "File name: " << file_name;

    QXlsx::Document xlsx(file_name);

    /* Add table header */
    xlsx.write(1, 1, "№");
    xlsx.write(1, 2, "Скорость перфузии");
    xlsx.write(1, 3, "Сопротивление");
    xlsx.write(1, 4, "Давление");
    xlsx.write(1, 5, "Температура 1");
    xlsx.write(1, 6, "Температура 2");
    xlsx.write(1, 7, "Время");
    xlsx.write(1, 8, "Режим работы");
    xlsx.write(1, 9, "Почка");
    xlsx.write(1, 10, "Блокировка");
    xlsx.write(1, 11, "Код ошибки");

    /* Export history */
    for (qsizetype i = 0; i < history.size(); ++i) {
        uint64_t row = i + 2;

        const ReportUnit &unit = history[i];

        xlsx.write(row, 1, i + 1); /* Add message number */
        xlsx.write(row, 2, unit.flow);
        xlsx.write(row, 3, unit.resistance);
        xlsx.write(row, 4, unit.fill_value);
        xlsx.write(row, 5, unit.temp1);
        xlsx.write(row, 6, unit.temp2);
        xlsx.write(row, 7, unit.time);
        xlsx.write(row, 8, unit.regime);
        xlsx.write(row, 9, (unit.kidney_selector) ? "Левая" : "Правая");
        xlsx.write(row, 10, static_cast<uint8_t>(unit.is_blocked));

        uint8_t error_code = 0;
        for (uint8_t i = 0; i < unit.alert.size(); ++i) {
            error_code |= (unit.alert[i] & 0b1) << i;
        }

        xlsx.write(row, 11, error_code);
    }

    xlsx.save();
}
