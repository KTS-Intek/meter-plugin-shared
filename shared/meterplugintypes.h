#ifndef METERPLUGINTYPES_H
#define METERPLUGINTYPES_H

#include <QVariantHash>

struct Mess2meterArgs
{
    QVariantHash hashConstData;
    QVariantHash hashTmpData;
    Mess2meterArgs() {}
    Mess2meterArgs(const QVariantHash &hashConstData, const QVariantHash &hashTmpData) : hashConstData(hashConstData), hashTmpData(hashTmpData) {}
};

struct Mess2meterRezult
{
    QVariantHash hashMessage;
    QVariantHash hashTmpData;
    Mess2meterRezult() {}
    Mess2meterRezult(const QVariantHash &hashMessage, const QVariantHash &hashTmpData) : hashMessage(hashMessage), hashTmpData(hashTmpData) {}
};


struct DecodeMeterMess
{
    QVariantHash hashConstData;
    QVariantHash hashTmpData;
    QVariantHash hashRead;
    DecodeMeterMess() {}
    DecodeMeterMess(const QVariantHash &hashConstData, const QVariantHash &hashTmpData, const QVariantHash &hashRead) : hashConstData(hashConstData), hashTmpData(hashTmpData), hashRead(hashRead) {}
};


#endif // METERPLUGINTYPES_H
