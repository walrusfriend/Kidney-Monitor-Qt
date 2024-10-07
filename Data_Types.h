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
    REMOVE_BUBBLE
};

const std::unordered_map<Regime, QString> RegimeMap {
    {Regime::STOPED, "Остановлен"},
    {Regime::REGIME1, "Перфузия"},
    {Regime::REGIME2, "Промывка"},
    {Regime::REMOVE_BUBBLE, "Удаление пузырьков"}
};

Q_DECLARE_METATYPE(LedColor)

#endif // DATA_TYPES_H
