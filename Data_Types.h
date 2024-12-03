#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <QObject>
#include <unordered_map>

enum LedColor {
    GRAY,
    GREEN,
    RED
};

enum Regime {
    STOPED,
    REGIME1,
    REGIME2,
    REMOVE_BUBBLE,
    BLOCKED
};

const std::unordered_map<Regime, QString> RegimeMap {
    {Regime::STOPED, "Остановлен"},
    {Regime::REGIME1, "Перфузия"},
    {Regime::REGIME2, "Промывка"},
    {Regime::REMOVE_BUBBLE, "Удаление пузырьков"},
    {Regime::BLOCKED, "Аварийная остановка"}
};

enum AlertType
{
    PRESSURE_LOW,
    PRESSURE_HIGH,
    PRESSURE_UP,
    TEMP1_LOW,
    TEMP1_HIGH,
    TEMP2_LOW,
    TEMP2_HIGH,
    RESISTANCE
};

namespace Graphs {
    enum DisplayedOptions {
        PRESSURE = 0,
        PERFUSION_SPEED,
        RESISTANCE,
        REGIME,
        TEMP1,
        TEMP2
    };

    const std::unordered_map<DisplayedOptions, QString> graphs_names_map {
        {DisplayedOptions::PRESSURE, "Давление"},
        {DisplayedOptions::PERFUSION_SPEED, "Скорость перфузии"},
        {DisplayedOptions::RESISTANCE, "Сопротивление"},
        {DisplayedOptions::REGIME, "Режим работы"},
        {DisplayedOptions::TEMP1, "Температура 1"},
        {DisplayedOptions::TEMP2, "Температура 2"}
    };
}

Q_DECLARE_METATYPE(LedColor)

#endif // DATA_TYPES_H
