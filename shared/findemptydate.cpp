#include "findemptydate.h"
#include "matildalimits.h"
#include <QDebug>

//-------------------------------------------------------------------------------------------------------------

bool FindEmptyDate::go2nextEnrg(int &valKeysIndx, const int &listValKeysLen, const QStringList &listValKeys)
{
    if(valKeysIndx < listValKeysLen){
        const QString currEnrg = listValKeys.at(valKeysIndx).right(2);
        for(int i = 0 ; i < 1000 && valKeysIndx < listValKeysLen; valKeysIndx++, i++){
            if(currEnrg != listValKeys.at(valKeysIndx).right(2))
                break;
        }
        if(valKeysIndx < listValKeysLen)
            return false;
    }
    valKeysIndx = 0;
    return true;//go 2 next dateIndx
}
//-------------------------------------------------------------------------------------------------------------
EmptyValDtEnrgTrf FindEmptyDate::getEmptyDate(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const EnrgProfileParams &profParams, const int &startFromDateIndx, const EmptyValDtEnrgTrf &lastEmptyItem, const int &groupBy)
{
//pollDate_0   - oldest
//pollDate_x   - newest
    int dateIndx = startFromDateIndx;
    const bool verboseMode = true;

/*
 * hashTmpData      - тимчасові дані лічильника, хеш можу змінюватися в плагіні
 * hashConstData    - постійні дані лічильника, хеш може змінюватися лише у збираторі
 *
 * profParams
 *  maxDayMonthAgo   - максимальна глибина в днях/місяцях
 *  hasQuadrants     - може містити реактивну енергію по квадрантам
 *  timeCoefitient   - одиниці часу, хвилини, години, дні, місяці
 *  everyTime        - кількість одиниць часу, період для даних
 *
 *
 * startFromDateIndx- почати пошук порожнього місця від індексу
 *
 * listValKeys
 * For power: A+ A- R+ R-
 * For tariff vals: Tx_enrg
 *
*/
    const bool oldValsChecked = hashTmpData.value("oldValsChecked").toBool();

    if(!oldValsChecked){ //check old data
        bool endOfCheck = false;
       const EmptyValDtEnrgTrf e = checkOldIntervalEmptyDate(hashConstData, listValKeys, profParams, dateIndx, endOfCheck);
        if(!endOfCheck)
            return e;
        hashTmpData.insert("oldValsChecked", true);//старі інтервали перевірено, йду далі
    }

    const int listValKeysLen = listValKeys.size();
    //пролистую до місця зупинки

    int valKeysIndx = lastEmptyItem.indxInList; //індекс останнього елементу

    if(!oldValsChecked || valKeysIndx < 0){ //it is first time, there is no a previous data
        valKeysIndx = 0;
    }else{
        switch(groupBy){
        case FND_GROUP_BY_TARIFF    : if(go2nextEnrg(valKeysIndx, listValKeysLen, listValKeys)) dateIndx++; break; //go to next energy
        case FND_GROUP_BY_DATETIME  : valKeysIndx = 0; dateIndx++ ; break; //go to next dt
            //    case FND_GROUP_BY_VAL_KEY:
        default: valKeysIndx++; break;//переходжу на наступний
        }
    }

    const bool nHasVersion = hashConstData.value("vrsn").toString().isEmpty();
    const QDateTime dtPollIndxEnd = hashConstData.value("pollDate").toDateTime();
    //перебираю хеш на наявність даних, в межах часу що є доступним для опитування
    for(int i = 0, jMax = maxAddIntervalCount4timeCoefitient(profParams.timeCoefitient); dateIndx < MAX_INRVL_AGO && i < MAX_INRVL_AGO; i++){
        QDateTime dtPollIndx = getDt4dateIndx(hashConstData, dateIndx);

        if(!dtPollIndx.isValid())
            return EmptyValDtEnrgTrf(QDateTime(), "", 0xFF, MAX_INRVL_AGO, false, valKeysIndx);

        const int localDay = dtPollIndx.toLocalTime().date().day();

        //в профілях Доби і Місяця кожен індекс дати відповідає необхідному інтервалу
        //в Потужності лише межі початку, кінця і днів коли відсутні дані

        for(int j = 0; j < 1440 && j < jMax; j++){

            if(valKeysIndx < 0)
                valKeysIndx = 0;

            const QString dtstr = dtStr4hashTmp(dtPollIndx, profParams.timeCoefitient);

            for(  ; valKeysIndx < listValKeysLen; valKeysIndx++){

                const QString trffEnrg   = listValKeys.at(valKeysIndx);
                const QString key        = dtstr + "_" + trffEnrg;

                if(hashTmpData.contains(key) ? checkEmptyVal(hashTmpData.value(key).toString(), nHasVersion, profParams.hasQuadrants) : true){
                    if(verboseMode)
                        qDebug() << 77 << "foundEmptyVal" << key << hashTmpData.value(key).toString() << nHasVersion << dtPollIndx << profParams.currDtUtc;

                    return EmptyValDtEnrgTrf(dtPollIndx, getTariffEnrgPair(trffEnrg), dateIndx, false, valKeysIndx);
                }
            }

            if(dtPollIndx > dtPollIndxEnd )
                return EmptyValDtEnrgTrf(QDateTime(), "", 0xFF, MAX_INRVL_AGO, false, valKeysIndx);
            valKeysIndx = 0;

            if( jMax < 2 || dtPollIndx.toLocalTime().date().day() != localDay )
                break;

            //add interval
            dtPollIndx = addTimeInterval2dt(dtPollIndx, profParams.timeCoefitient, profParams.everyTime);

            if(dtPollIndx > dtPollIndxEnd )
                return EmptyValDtEnrgTrf(QDateTime(), "", 0xFF, MAX_INRVL_AGO, false, valKeysIndx);
        }
        dateIndx++;
    }
    return EmptyValDtEnrgTrf(QDateTime(), "", 0xFF, MAX_INRVL_AGO, false, valKeysIndx);
}
//-------------------------------------------------------------------------------------------------------------
EmptyValDtEnrgTrf FindEmptyDate::checkOldIntervalEmptyDate(const QVariantHash &hashConstData, const QStringList &listValKeys, const EnrgProfileParams &profParams, int &dateIndx, bool &oldValueChecked)
{
    //заповнюю пустими значеннями, інтервали по яким я не можу отримати дані
    if(dateIndx < 0)
        dateIndx = 0;

    for(int dayOrMonthAgo = calculateTimeAgoDayOrMonth(getDt4dateIndx(hashConstData, dateIndx), profParams.currDtUtc, profParams.timeCoefitient), i = 0; dayOrMonthAgo > profParams.maxDayMonthAgo && i < 0xFFFFFF; i++ ){
        const QDateTime dtPollIndx = getDt4dateIndx(hashConstData, dateIndx);
        if(!dtPollIndx.isValid() || dayOrMonthAgo < 0 || dayOrMonthAgo > profParams.maxDayMonthAgo)
            return EmptyValDtEnrgTrf(dtPollIndx, getTariffEnrgPair( listValKeys.isEmpty() ? "A+" : listValKeys.first()), dateIndx, true, 0);//end poll

        dateIndx++;
    }
    oldValueChecked = true;
    return EmptyValDtEnrgTrf();
}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::calculateTimeAgoDayOrMonth(const QDateTime &dtPollIndx, const QDateTime &currDate, const quint8 &timeCoefitient)
{
    int timeAgo = calculateTimeAgo(dtPollIndx, currDate, timeCoefitient);
    if(timeAgo > 0 && timeCoefitient < TIME_COEFITIENT_DAY){
        switch(timeCoefitient){
        case TIME_COEFITIENT_SECOND : timeAgo /= 60;
        case TIME_COEFITIENT_MINUTE : timeAgo /= 60;
        case TIME_COEFITIENT_HOUR   : timeAgo /= 24;
        }
    }
    return timeAgo;

}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::calculateTimeAgo(const QDateTime &dtPollIndx, const QDateTime &currDate, const quint8 &timeCoefitient)
{
    int timeAgo = -1;
    switch(timeCoefitient){
    case TIME_COEFITIENT_MONTH  : timeAgo = calculateMonthAgo(dtPollIndx, currDate)         ; break;
    case TIME_COEFITIENT_DAY    : timeAgo = calculateDaysAgo( dtPollIndx, currDate)         ; break;
    case TIME_COEFITIENT_HOUR   : timeAgo = calculateSecondsAgo(dtPollIndx, currDate)/3600  ; break;
    case TIME_COEFITIENT_MINUTE : timeAgo = calculateSecondsAgo(dtPollIndx, currDate)/60    ; break;
    case TIME_COEFITIENT_SECOND : timeAgo = calculateSecondsAgo(dtPollIndx, currDate)       ; break;

    }
    return timeAgo;
}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::calculateMonthAgo(const QDateTime &dtPollIndx, const QDateTime &currDate)
{
    return (currDate.isValid() && dtPollIndx.isValid()) ? (date2months(currDate) - date2months(dtPollIndx)) : -1;
}

//-------------------------------------------------------------------------------------------------------------

QDateTime FindEmptyDate::addTimeInterval2dt(const QDateTime &dtPollIndx, const quint8 &timeCoefitient, const quint32 &everyTime)
{
    QDateTime dt;
    switch(timeCoefitient){
    case TIME_COEFITIENT_MONTH  : dt = dtPollIndx.addMonths(everyTime)      ; break;
    case TIME_COEFITIENT_DAY    : dt = dtPollIndx.addDays(everyTime)        ; break;
    case TIME_COEFITIENT_HOUR   : dt = dtPollIndx.addSecs(everyTime * 3600) ; break;
    case TIME_COEFITIENT_MINUTE : dt = dtPollIndx.addSecs(everyTime * 60  ) ; break;
    case TIME_COEFITIENT_SECOND : dt = dtPollIndx.addSecs(everyTime       ) ; break;
    default: dt = dtPollIndx.addMonths(100); break;
    }
    return dt;
}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::date2months(const QDateTime &dt)
{
    return (dt.date().year() * 12 + dt.date().month());
}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::calculateDaysAgo(const QDateTime &dtPollIndx, const QDateTime &currDate)
{
    return (currDate.isValid() && dtPollIndx.isValid()) ? dtPollIndx.daysTo(currDate) : -1;
}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::calculateSecondsAgo(const QDateTime &dtPollIndx, const QDateTime &currDate)
{
    return (currDate.isValid() && dtPollIndx.isValid()) ? (dtPollIndx.secsTo(currDate) ) : -1;

}

//-------------------------------------------------------------------------------------------------------------

QString FindEmptyDate::dtStr4hashTmp(const QDateTime &dtPollIndx, const quint8 &timeCoefitient)
{
    if(!dtPollIndx.isValid())
        return "";
    QString dtmask, rightPart;
    switch(timeCoefitient){
    case TIME_COEFITIENT_MONTH  : dtmask = "yyyy_MM"    ; rightPart = QString("_%1_23_59").arg(dtPollIndx.date().daysInMonth()) ; break;
    case TIME_COEFITIENT_DAY    : dtmask = "yyyy_MM_dd" ; rightPart = QString("_23_59")                                         ; break;
    case TIME_COEFITIENT_HOUR   :
    case TIME_COEFITIENT_MINUTE :
    case TIME_COEFITIENT_SECOND : dtmask = "yyyy_MM_dd_hh_mm"                                                                   ; break;
    default: dtmask = "yyyy_MM_dd_hh_mm"                                                                                        ; break;
    }
    return dtPollIndx.toString(dtmask) + rightPart;
}
//-------------------------------------------------------------------------------------------------------------
QStringList FindEmptyDate::valStr2quadrants(const QString &valStr)
{
    //[xxx.xx];[xxx.xx]
    const QStringList l = valStr.mid(1, valStr.length() - 2).split("];[",
                                                               #if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
                                                                   Qt::SkipEmptyParts
                                                               #else
                                                                   QString::SkipEmptyParts
                                                               #endif
                                                                   );
    return (l.size() == 2) ? l : QStringList();
}

//-------------------------------------------------------------------------------------------------------------

bool FindEmptyDate::checkEmptyVal(const QString &valStr, const bool &nHasVersion, const bool &hasQuadrants)
{

    return (hasQuadrants && valStr.contains("];[")) ?
                checkFoundEmptyValQuadrant(valStr, nHasVersion) :
                checkFoundEmptyVal(valStr, nHasVersion);

}

//-------------------------------------------------------------------------------------------------------------

bool FindEmptyDate::checkFoundEmptyVal(const QString &valStr, const bool &nHasVersion)
{
    return (valStr.isEmpty() || valStr == "?" || (valStr == "!" && nHasVersion));// || (valStr == "-" && qAbs(dtPollIndx.secsTo(currDtUtc)) < (pwrIntrvl * 60)));
}

//-------------------------------------------------------------------------------------------------------------

bool FindEmptyDate::checkFoundEmptyValQuadrant(const QString &valStr, const bool &nHasVersion)
{

    const QStringList list = valStr2quadrants(valStr);//[LFT];[RGT]
    return list.isEmpty() ? true : (checkFoundEmptyVal(list.first(), nHasVersion) || checkFoundEmptyVal(list.last(), nHasVersion));

}

//-------------------------------------------------------------------------------------------------------------

int FindEmptyDate::maxAddIntervalCount4timeCoefitient(const quint8 &timeCoefitient)
{
    int maxAdd = 1;
    switch(timeCoefitient){
    case TIME_COEFITIENT_MONTH  :
    case TIME_COEFITIENT_DAY    : maxAdd = 1    ; break;
    case TIME_COEFITIENT_HOUR   : maxAdd = 24   ; break;
    case TIME_COEFITIENT_MINUTE :
    case TIME_COEFITIENT_SECOND : maxAdd = 1440 ; break;
    }
    return maxAdd;
}
//-------------------------------------------------------------------------------------------------------------
TrffEnrgPair FindEmptyDate::getTariffEnrgPair(const QString &trffEnrg)
{
    const QString enrg = trffEnrg.split("_").last();
    bool ok = false;
    const quint8  trff = trffEnrg.startsWith("T") ? trffEnrg.mid(1,1).toUInt(&ok) : 0xFF; //enrg.toUInt()
    return TrffEnrgPair(enrg.isEmpty() ? "A+" : enrg, ok ? trff : 0xFF);

}

//-------------------------------------------------------------------------------------------------------------

QDateTime FindEmptyDate::getDt4dateIndx(const QVariantHash &h, const int &dateIndx)
{
    return h.value(QString("pollDate_%1").arg(qMax(0, dateIndx))).toDateTime();
}

//-------------------------------------------------------------------------------------------------------------

EmptyValDtEnrgTrf FindEmptyDate::getEmptyDate4endOfDay(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const int &maxDayMonthAgo, const bool &hasQuadrants, const int &startFromDateIndx)
{    
    return getEmptyDate(hashTmpData, hashConstData, listValKeys, getParams4endOfDay(maxDayMonthAgo, hasQuadrants), startFromDateIndx, EmptyValDtEnrgTrf(), FND_GROUP_BY_VAL_KEY);//if -1, the first time,  check for Old period
}

//-------------------------------------------------------------------------------------------------------------

ListEmptyVals FindEmptyDate::getEmptyDates4endOfDay(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QStringList &listValKeys, const int &maxDayMonthAgo, const bool &hasQuadrants, const int &startFromDateIndx, const int &limit, const int &groupBy)
{
    //
    ListEmptyVals l;

    const EnrgProfileParams params = getParams4endOfDay(maxDayMonthAgo, hasQuadrants);

    for(int i = 0, lastStartDayIndx = startFromDateIndx; i < limit; i++){
        const EmptyValDtEnrgTrf v = getEmptyDate(hashTmpData, hashConstData, listValKeys, params, lastStartDayIndx, l.isEmpty() ? EmptyValDtEnrgTrf() : l.last(), groupBy);//if -1, the first time,  check for Old period
        if(!v.dt.isValid()){
            qDebug() << "FindEmptyDate::getEmptyDates4endOfDay " << v.dt << hashConstData.value("ModemNI").toString() ;
            break;//no value
        }

        l.append(v);
        lastStartDayIndx = v.lastDateIndx;

        if(v.need2addEmptyVal2oldPeriod){
            i--;//мені необхідно показати інтервали датою для опитування, а не для пустих значень
            lastStartDayIndx++; //за цей індекс даних нема, переходжу на наступний
        }


    }
    if(l.isEmpty() && hashConstData.value("verboseMode").toBool()){

        qDebug() << "FindEmptyDate::getEmptyDates4endOfDay no data " << hashConstData.value("ModemNI").toString() << listValKeys << maxDayMonthAgo << hasQuadrants;

    }
    return l;
}

//-------------------------------------------------------------------------------------------------------------

EnrgProfileParams FindEmptyDate::getParams4endOfDay(const int &maxDayMonthAgo, const bool &hasQuadrants)
{
    return EnrgProfileParams(maxDayMonthAgo, hasQuadrants, TIME_COEFITIENT_DAY, 1, QDateTime::currentDateTimeUtc());
}

//-------------------------------------------------------------------------------------------------------------
