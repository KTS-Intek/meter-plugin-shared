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






     static QVariantHash addEvnt2hash(const quint16 &evnt, const QDateTime &evntDtInUTC, const QString comment)  ;

     static QString nextEvntName(const QVariantHash &hashTmpData)  ;

     static QString nextEvntName(const QVariantHash &hashTmpData, const QVariantHash &hash)  ;

     static QString nextMatildaEvntName(const QVariantHash &hashTmpData)  ;


     static qint32 calculateMonthAgo(const QDateTime &pollDateTimeL, bool &ok) ;


     static QString errWarnKey(int &val, const bool &isErr);


     static QVariantHash oneTariff4json();


     static QStringList listFromArr(const QByteArray &inArr, const QByteArray &leftPart, const QByteArray &rightSymbol, const QString &separ, const bool skipEmpty = true);

     static bool dayProfilesFromInData(QList<quint8> listTariff, QStringList listTime, QVariantHash &hashTmpData, const int &lastDayProfileIndxD, const QString &plgName);

     static QVariantHash hash4jsonTariff(const QVariantHash &hashTmpData, const int &lastDayProfileIndxD, const int &seasonMax, const QString &plgName);

     static int extendedDayProfile(QVariantHash &hashTmpData, QVariantHash &hash4json, const QString &plgName);



     static int hoursInDay(QDateTime dateTimeL);

     static bool has25hoursInDay(QDateTime dateTimeL);

     static bool has23hoursInDay(QDateTime dateTimeL);

     static qint32 secsTo25hour(QDateTime dateTimeL);

     static QString prettyMess(const QString &mess, const QString &hexDump, QString &lastErrorStr, QString &lastWarning, const bool &isErr);

     static QString prettyMess(const QString &mess, const QString &hexDump, const bool &isErr, ErrsStrct &errwarn);




     static quint8 ucmEvent2groupCode(const quint32 &ucmEventCode);

     static QVariantHash addOneKeyValue(const QString &key, const QVariant &val, QVariantHash h = QVariantHash());

     static void copyHash2hash(const QVariantHash &src, QVariantHash &hashDest);

     static void copyHash2hashKeys(const QVariantHash &src, QVariantHash &hashDest, const QString &startWithLine, const QString &separ);

     static void copyHash2hashErrAndWarn(const QVariantHash &src, QVariantHash &hashDest);



     static QString getMeterVersion(const QVariantHash &hashTmpData, const QVariantHash &hashConstData);

     static bool getCorrDateTime(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt);

     static bool getCorrDateTimeExt(const bool &ignoreDiff, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt);

     static int getTariffCount(const QVariantHash &hashConstData);

     static bool isTime4updateSchedule(const QVariantHash &hashConstDataSchedule, const QVariantHash &currentMeterSchedule);

     static bool isTime4updateScheduleHConstData(const QVariantHash &hashConstData, const QVariantHash &currentMeterSchedule);

     static void addRelayStatus(QVariantHash &hashTmpData, const int &relayStts, const bool &isMain);

     static void addRelayStatusAll(QVariantHash &hashTmpData, const int &mainstts, const int &secondarystts);


signals:

public slots:
};

#endif // METERPLUGINHELPER_H
