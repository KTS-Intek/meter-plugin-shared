#ifndef FINDEMPTYDATE_H
#define FINDEMPTYDATE_H

#include <QVariantHash>
#include <QDateTime>

#define TIME_COEFITIENT_SECOND  0
#define TIME_COEFITIENT_MINUTE  1
#define TIME_COEFITIENT_HOUR    2
#define TIME_COEFITIENT_DAY     3
#define TIME_COEFITIENT_MONTH   4

#define FND_GROUP_BY_VAL_KEY    0 //default, находити всі порожні значення
#define FND_GROUP_BY_TARIFF     1 //находити порожнє значення тільки для одного тарифу, якщо хоч по одному тарифу є порожє значення, то переходжу на наступну енергію
#define FND_GROUP_BY_DATETIME   2 //находити порожнє порожнє значення тільки для однієї дати, якщо знайдено відсутні дані, то перехід на наступну дату

struct TrffEnrgPair
{
    QString enrg;
    quint8 tariff;
    TrffEnrgPair() {}
    TrffEnrgPair(const QString &enrg, const quint8 &tariff) : enrg(enrg), tariff(tariff) {}
};

struct EmptyValDtEnrgTrf
{
    QDateTime dt;
    QString enrg;
    quint8 tariff;
    int lastDateIndx;
    bool need2addEmptyVal2oldPeriod;
    int indxInList;
    EmptyValDtEnrgTrf() : indxInList(-1) {} //empty indx

    EmptyValDtEnrgTrf(const QDateTime &dt, const QString &enrg, const quint8 &tariff, const int &lastDateIndx, const bool &need2addEmptyVal2oldPeriod, const int &indxInList)
        : dt(dt), enrg(enrg), tariff(tariff), lastDateIndx(lastDateIndx), need2addEmptyVal2oldPeriod(need2addEmptyVal2oldPeriod), indxInList(indxInList) {}

    EmptyValDtEnrgTrf(const QDateTime &dt, const TrffEnrgPair &trffEnrg, const int &lastDateIndx, const bool &need2addEmptyVal2oldPeriod, const int &indxInList)
        : dt(dt), enrg(trffEnrg.enrg), tariff(trffEnrg.tariff), lastDateIndx(lastDateIndx), need2addEmptyVal2oldPeriod(need2addEmptyVal2oldPeriod), indxInList(indxInList) {}

};

struct EnrgProfileParams
{
    int maxDayMonthAgo;
    bool hasQuadrants;
    quint8 timeCoefitient;
    quint32 everyTime;
    QDateTime currDtUtc;

    EnrgProfileParams(const int &maxDayMonthAgo, const bool &hasQuadrants, const quint8 &timeCoefitient, const quint32 &everyTime, const QDateTime &currDtUtc) :
        maxDayMonthAgo(maxDayMonthAgo), hasQuadrants(hasQuadrants), timeCoefitient(timeCoefitient), everyTime(everyTime), currDtUtc(currDtUtc) {}
};


typedef QList<EmptyValDtEnrgTrf> ListEmptyVals;


struct ListEmptyValsCache
{
    QString ni;
    ListEmptyVals list;
    int lastDtIndx; //accepted
    int lastDtIndxMessSended; //sended and waiting for answer
    int session;
    int listSize;
    quint8 lastPollCode;

    ListEmptyValsCache() : lastDtIndx(-1), session(0), listSize(0), lastPollCode(0) {}

    ListEmptyValsCache(const QString &ni, const ListEmptyVals &list, const int &lastDtIndx) :
        ni(ni), list(list), lastDtIndx(lastDtIndx), lastDtIndxMessSended(0), session(0), listSize(list.size()), lastPollCode(0) {}

    ListEmptyValsCache(const QString &ni, const ListEmptyVals &list, const int &lastDtIndx, const int &session) :
        ni(ni), list(list), lastDtIndx(lastDtIndx), lastDtIndxMessSended(0), session(session), listSize(list.size()), lastPollCode(0) {}

};


class FindEmptyDate
{

public:

    static bool go2nextEnrg(int &valKeysIndx, const int &listValKeysLen, const QStringList &listValKeys);

    static EmptyValDtEnrgTrf getEmptyDate(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const EnrgProfileParams &profParams, const int &startFromDateIndx, const EmptyValDtEnrgTrf &lastEmptyItem, const int &groupBy);

    static EmptyValDtEnrgTrf checkOldIntervalEmptyDate(const QVariantHash &hashConstData, const QStringList &listValKeys, const EnrgProfileParams &profParams, int &dateIndx, bool &oldValueChecked);


    static int calculateTimeAgoDayOrMonth(const QDateTime &dtPollIndx, const QDateTime &currDate, const quint8 &timeCoefitient);

    static int calculateTimeAgo(const QDateTime &dtPollIndx, const QDateTime &currDate, const quint8 &timeCoefitient);

    static int calculateMonthAgo(const QDateTime &dtPollIndx, const QDateTime &currDate);

    static QDateTime addTimeInterval2dt(const QDateTime &dtPollIndx, const quint8 &timeCoefitient, const quint32 &everyTime);

    static int date2months(const QDateTime &dt);

    static int calculateDaysAgo(const QDateTime &dtPollIndx, const QDateTime &currDate);

    static int calculateSecondsAgo(const QDateTime &dtPollIndx, const QDateTime &currDate);

    static QString dtStr4hashTmp(const QDateTime &dtPollIndx, const quint8 &timeCoefitient);

    static QStringList valStr2quadrants(const QString &valStr);

    static bool checkEmptyVal(const QString &valStr, const bool &nHasVersion, const bool &hasQuadrants);

    static bool checkFoundEmptyVal(const QString &valStr, const bool &nHasVersion);

    static bool checkFoundEmptyValQuadrant(const QString &valStr, const bool &nHasVersion);

    static int maxAddIntervalCount4timeCoefitient(const quint8 &timeCoefitient);

    static TrffEnrgPair getTariffEnrgPair(const QString &trffEnrg);

    static QDateTime getDt4dateIndx(const QVariantHash &h, const int &dateIndx);

    static EmptyValDtEnrgTrf getEmptyDate4endOfDay(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const int &maxDayMonthAgo, const bool &hasQuadrants, const int &startFromDateInd);

    static ListEmptyVals getEmptyDates4endOfDay(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const int &maxDayMonthAgo, const bool &hasQuadrants, const int &startFromDateIndx, const int &limit, const int &groupBy);


    static EnrgProfileParams getParams4endOfDay(const int &maxDayMonthAgo, const bool &hasQuadrants);
};

#endif // FINDEMPTYDATE_H
