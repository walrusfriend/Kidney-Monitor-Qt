#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <QObject>

enum LedColor {
    GRAY,
    GREEN,
    RED
};

Q_DECLARE_METATYPE(LedColor)

#endif // DATA_TYPES_H
