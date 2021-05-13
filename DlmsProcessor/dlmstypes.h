#ifndef DLMSTYPES_H
#define DLMSTYPES_H

#include <QList>
#include <QByteArray>
#include <QString>

typedef QList<quint64> ObisList;
typedef QList<quint16> AttributeList;
typedef QList<QByteArray> ArrayHexList;

struct LastErrStruct{
    QString lastErrorStr;
    QString lastWarning;
};

#endif // DLMSTYPES_H
