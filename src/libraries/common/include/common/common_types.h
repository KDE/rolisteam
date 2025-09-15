#ifndef TYPES_H
#define TYPES_H

#include "common_global.h"

#include <QString>

namespace common
{
struct COMMON_EXPORT Log
{
    QString m_category;
    QString m_message;
    QString m_timestamp;
    QString m_level;
};
} // namespace common

#endif // TYPES_H
