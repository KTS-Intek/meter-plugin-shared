#ifndef METERPLUGINHELPER_H
#define METERPLUGINHELPER_H

#include <QObject>
#include <QVariantHash>


struct ErrsStrct
{
    QString lastErrorStr;
    QString lastWarning;
    bool lastIsErr;
    ErrsStrct() {}
    ErrsStrct(const QString &lastErrorStr, const QString &lastWarning, const bool &lastIsErr) : lastErrorStr(lastErrorStr), lastWarning(lastWarning), lastIsErr(lastIsErr) {}
};

struct ErrCounters
{
    int error_counter;
    int warning_counter;
    ErrCounters() : error_counter(0), warning_counter(0){}
    ErrCounters(const int &error_counter, const int &warning_counter) : error_counter(error_counter), warning_counter(warning_counter) {}
};


class MeterPluginHelper : public QObject
{
    Q_OBJECT
public:
    explicit MeterPluginHelper(QObject *parent = nullptr);

     static  void removeLineFromList(const QString &lPartOfline, QStringList &l);

     static  QStringList replaceLineFromList(const QString &lPartOfline, QStringList l, const QString &rPartOfLine);

     static QBitArray uint8ToBitArray(const quint8 &value);

     static QByteArray bitArrToByteArr(const QBitArray &bitArr, const bool &toHex);

     static  QBitArray byteArrayToBitArray(const QByteArray &byteArr);

     static QString prettyHexDump(const QList<quint8> &list, const quint8 &command);

     static QString prettyHexDump(const QByteArray &messArrH, const QByteArray &commandH, const quint8 &errCode);

     static QString prettyHexDump(QByteArray arr);

     static QDateTime tableName2DateTimeUTC(const QString &tableName, const qint32 addDays = 0) ;

     static QVariantHash addEvnt2hash(const quint16 &evnt, const QDateTime &evntDtInUTC, const QString comment)  ;

     static QString nextEvntName(const QVariantHash &hashTmpData)  ;

     static QString nextEvntName(const QVariantHash &hashTmpData, const QVariantHash &hash)  ;

     static QString nextMatildaEvntName(const QVariantHash &hashTmpData)  ;

     static QString prettyNumber(const qreal &val, int mPrec, const int &maxPlgPrec) ;

     static qint32 calculateMonthAgo(const QDateTime &pollDateTimeL, bool &ok) ;


     static QString errWarnKey(int &val, const bool &isErr);

     static QList<quint8> strList2uint8List(const QStringList &l, const bool ommitBadData = true);

     static int uint8list2int(const QList<quint8> &meterMess, int startIndx, const int &len);

     static QString uint8list2line(const QList<quint8> &meterMess, int startIndx, const int &len);

     static QString uint8list2str(const QList<quint8> &meterMess, int startIndx, const int &len);

     static QByteArray uint8list2strH(const QList<quint8> &meterMess, int startIndx, const int &len);

     static int getIntValFromList(const QStringList &list, const QString &key, const int &defRetVal);

     static QByteArray intVal2arrH(const int &val, const int &rightJustif);

     static QString intVal2strH(const int &val, const int &rightJustif);

     static QVariantHash oneTariff4json();


     static QStringList listFromArr(const QByteArray &inArr, const QByteArray &leftPart, const QByteArray &rightSymbol, const QString &separ, const bool skipEmpty = true);

     static bool dayProfilesFromInData(QList<quint8> listTariff, QStringList listTime, QVariantHash &hashTmpData, const int &lastDayProfileIndxD, const QString &plgName);

     static QVariantHash hash4jsonTariff(const QVariantHash &hashTmpData, const int &lastDayProfileIndxD, const int &seasonMax, const QString &plgName);

     static int extendedDayProfile(QVariantHash &hashTmpData, QVariantHash &hash4json, const QString &plgName);

     static QByteArray hexArrFromNumber(const int &inVal, const int &byteCount, const int base = 10);


     static int hoursInDay(QDateTime dateTimeL);

     static bool has25hoursInDay(QDateTime dateTimeL);

     static bool has23hoursInDay(QDateTime dateTimeL);

     static qint32 secsTo25hour(QDateTime dateTimeL);

     static QString prettyMess(const QString &mess, const QString &hexDump, QString &lastErrorStr, QString &lastWarning, const bool &isErr);

     static QString prettyMess(const QString &mess, const QString &hexDump, const bool &isErr, ErrsStrct &errwarn);

     static float hexstr2float(const QByteArray &arrh, bool &ok);

     static double hexstr2double(const QByteArray &arrh, bool &ok);


     static quint8 ucmEvent2groupCode(const quint32 &ucmEventCode);

     static QVariantHash addOneKeyValue(const QString &key, const QVariant &val, QVariantHash h = QVariantHash());

     static void copyHash2hash(const QVariantHash &src, QVariantHash &hashDest);

     static void copyHash2hashKeys(const QVariantHash &src, QVariantHash &hashDest, const QString &startWithLine, const QString &separ);

     static void copyHash2hashErrAndWarn(const QVariantHash &src, QVariantHash &hashDest);

     static QByteArray convertData7ToData8(const QByteArray &readArr);

     static QByteArray convertData8ToData7(const QByteArray &writeArr);

     static QByteArray listUint8mid2arr(const QList<quint8> &meterMess, const int &startPos, const int &len);

     static QByteArray listUint8mid2arrMess(const QList<quint8> &meterMess, const int &startPos, const int &len);


     static qreal listUint8mid2real(const QList<quint8> &meterMess, const int &startPos, const int &len, const int &dotPos, const int isUnsigned, bool &ok);

     static QString getMeterVersion(const QVariantHash &hashTmpData, const QVariantHash &hashConstData);

     static bool getCorrDateTime(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt);

     static bool getCorrDateTimeExt(const bool &ignoreDiff, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt);

     static int getTariffCount(const QVariantHash &hashConstData);

     static bool isTime4updateSchedule(const QVariantHash &hashConstDataSchedule, const QVariantHash &currentMeterSchedule);

     static bool isTime4updateScheduleHConstData(const QVariantHash &hashConstData, const QVariantHash &currentMeterSchedule);

signals:

public slots:
};

#endif // METERPLUGINHELPER_H
