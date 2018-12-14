#include "meterpluginhelper.h"
#include <QtCore>

#include "moji_defy.h"
#include "matildalimits.h"
#include "definedpollcodes.h"


#include "ucmetereventcodes.h"

#define DEF_TARIFF_NUMB     3


//-----------------------------------------------------------------------------------
MeterPluginHelper::MeterPluginHelper(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------------
void MeterPluginHelper::removeLineFromList(const QString &lPartOfline, QStringList &l)
{
    for(int i = 0, iMax = l.size(), left = lPartOfline.size(); i < iMax; i++){
        if(l.at(i).left(left) == lPartOfline){
            l.removeAt(i);
            i--;
            iMax--;
        }
    }
}
//-----------------------------------------------------------------------------------
QStringList MeterPluginHelper::replaceLineFromList(const QString &lPartOfline, QStringList l, const QString &rPartOfLine)
{

    for(int i = 0, iMax = l.size(), left = lPartOfline.size(); i < iMax; i++){
        if(l.at(i).left(left) == lPartOfline){
            l.replace(i, lPartOfline + rPartOfLine);
            return l;
        }
    }
    l.append(lPartOfline + rPartOfLine);
    return l;
}
//-----------------------------------------------------------------------------------
QBitArray MeterPluginHelper::uint8ToBitArray(const quint8 &value)
{
    QByteArray byteArr;
    byteArr.append(value);
    QBitArray bitArr(8);

    for(int i = 0, iMax = 1; i < iMax; ++i){
        for(int b = 0; b < 8; ++b)
            bitArr.setBit(i * 8 + b, byteArr.at(i) & (1 << b));
    }

    return bitArr;
}

//-----------------------------------------------------------------------------------
QByteArray MeterPluginHelper::bitArrToByteArr(const QBitArray &bitArr, const bool &toHex)
{
    QByteArray byteArr(bitArr.count() / 8, 0x0);
    int rightJustif = byteArr.size() * 2;

    for(int b = 0, bMax = bitArr.count(); b < bMax; b++)
        byteArr[(bMax - b - 1)/8] = (byteArr.at((bMax - b - 1)/8) | ((bitArr[b] ? 1 : 0) << (b%8)));

    if(toHex)
        return byteArr.toHex().rightJustified(rightJustif, '0');

    return byteArr;
}


//-----------------------------------------------------------------------------------
QBitArray MeterPluginHelper::byteArrayToBitArray(const QByteArray &byteArr)
{
    const int iMax = byteArr.size();
    QBitArray bitArr(8 * iMax);

    for(int i = 0; i < iMax; ++i){
        for(int b = 0; b < 8; ++b)
            bitArr.setBit(i * 8 + b, byteArr.at(iMax - i - 1) & (1 << b));
    }
    return bitArr;
}



//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyHexDump(const QList<quint8> &list, const quint8 &command)
{
    QByteArray arr;
    for(int i = 0, iMax = list.size(); i < iMax; i++)
        arr.append(list.at(i));
    arr = arr.toHex();
    QString str;
    for(int i = 0, iMax = arr.size(); i < iMax; i += 2)
        str.append(arr.mid(i,2) + " ");

    if(!str.isEmpty())
        str.chop(1);
//    if(str.isEmpty())
//        return "";

    if(!str.isEmpty())
        str.prepend(", D:");

    str.prepend(", C:" + QString::number(command, 16).toUpper());
    return  str.toUpper();
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyHexDump(const QByteArray &messArrH, const QByteArray &commandH, const quint8 &errCode)
{
    QByteArray arr(messArrH);

    QString str;
    for(int i = 0, iMax = arr.size(); i < iMax; i += 2)
        str.append(arr.mid(i,2) + " ");

    if(!str.isEmpty())
        str.chop(1);


    if(!str.isEmpty())
        str.prepend(", D:");

    if(!commandH.isEmpty()){
        str.prepend(", C:" + commandH.toUpper());
        str.append(", E:" + QString::number(errCode, 16).toUpper());
    }
    return  str.toUpper();
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyHexDump(QByteArray arr)//CE303 style
{
    arr = arr.toHex();
    QString str;
    for(int i = 0, iMax = arr.size(); i < iMax; i += 2)
        str.append(arr.mid(i,2) + " ");

    if(!str.isEmpty())
        str.chop(1);
    if(str.isEmpty())
        return "";
    str.prepend(", D:");

    return  str.toUpper();
}
//-----------------------------------------------------------------------------------
QDateTime MeterPluginHelper::tableName2DateTimeUTC(const QString &tableName, const qint32 addDays)
{
    if(tableName.mid(4,1) != "_")
        return QDateTime( QDate::fromString(tableName.mid(4, 10), "yyyy_MM_dd"), QTime::fromString(tableName.right(5), "hh_mm"), Qt::UTC ).addDays(addDays);
    else
        return QDateTime( QDate::fromString(tableName.left(10), "yyyy_MM_dd"), QTime::fromString(tableName.right(5), "hh_mm"), Qt::UTC ).addDays(addDays);
}
//-----------------------------------------------------------------------------------
QVariantHash MeterPluginHelper::addEvnt2hash(const quint16 &evnt, const QDateTime &evntDtInUTC, const QString comment)
{
    QVariantHash h;
    h.insert("dt", evntDtInUTC);
    h.insert("evnt", evnt);
    h.insert("c", comment);
    return h;
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::nextEvntName(const QVariantHash &hashTmpData)
{
    for(quint16 i = 0; i < 0xFFFE; i++){
        if(!hashTmpData.contains(QString("ev_%1").arg(QString::number(i))))
            return QString("ev_%1").arg(QString::number(i));
    }
    return QString("ev_%1").arg(QString::number(0xFFFF));//UCM
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::nextEvntName(const QVariantHash &hashTmpData, const QVariantHash &hash)
{
    const QString prevKey = nextEvntName(hashTmpData);
    quint16 i = prevKey.mid(3).toUShort();

    for( ; i < 0xFFFE; i++){
        if(!hash.contains(QString("ev_%1").arg(QString::number(i))))
            return QString("ev_%1").arg(QString::number(i));
    }
    return QString("ev_%1").arg(QString::number(0xFFFF));//UCM

}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::nextMatildaEvntName(const QVariantHash &hashTmpData)
{
    for(quint16 i = 0; i < 0xFFFE; i++){
        if(!hashTmpData.contains(QString("mev_%1").arg(QString::number(i))))
            return QString("mev_%1").arg(QString::number(i));
    }
    return QString("mev_%1").arg(QString::number(0xFFFF));//UC
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyNumber(const qreal &val, int mPrec, const int &maxPlgPrec)
{
    if(mPrec < 1 || mPrec > maxPlgPrec)
        return "";

    QString s = QString::number(val, 'f', mPrec);
    s = s.replace(",", ".");
    if(s.isEmpty())
        return s;
    while(s.right(1) == "0")
        s.chop(1);


    if(s == "0." || s.isEmpty())
        return "0";

    if(s.right(1) == ".")
        s.append("0");

    return s;

}
//-----------------------------------------------------------------------------------
qint32 MeterPluginHelper::calculateMonthAgo(const QDateTime &pollDateTimeL, bool &ok)
{
    const qint32 pollStamp = pollDateTimeL.date().year();// toString("yyyyMM").toInt();
    const QDate currDt = QDate::currentDate();
    qint32 nowStamp = currDt.year();// QDateTime::currentDateTime().date().year();// .toString("yyyyMM").toInt();


    if(nowStamp < pollStamp){
        ok = false;
        return 0;
    }


    nowStamp -= pollStamp;//yyyy - yyyy
    nowStamp *= 12;
    nowStamp += (currDt.month() - pollDateTimeL.date().month());


    if(nowStamp <= 0 || nowStamp > 0xFE){
        ok = false;
        return 0;
    }
    ok = true;
    return nowStamp;
}

//-----------------------------------------------------------------------------------
QString MeterPluginHelper::errWarnKey(int &val, const bool &isErr)
{
    return isErr ? QString("Error_%1").arg(val++) : QString("Warning_%1").arg(val++);
}
//-----------------------------------------------------------------------------------
QList<quint8> MeterPluginHelper::strList2uint8List(const QStringList &l, const bool ommitBadData)
{
    QList<quint8> list;
    for(int i = 0, iMax = l.size(); i < iMax; i++){
        bool ok;
        const quint8 v = quint8(l.at(i).toUInt(&ok));
        if(ommitBadData && !ok)
            continue;
        list.append(v);
    }
    return list;
}
//-----------------------------------------------------------------------------------
int MeterPluginHelper::uint8list2int(const QList<quint8> &meterMess, int startIndx, const int &len)
{
    int retVal = 0;
    const int meterMessS = meterMess.size();
    if(meterMessS > startIndx){
        QByteArray arr;
       for(int iMax = len + startIndx ; startIndx < meterMessS && startIndx < iMax; startIndx++)
            arr.append( QByteArray::number(meterMess.at(startIndx), 16).rightJustified(2, '0') );

        retVal = int(arr.toUInt(nullptr, 16));
    }
    return retVal;
}

QString MeterPluginHelper::uint8list2line(const QList<quint8> &meterMess, int startIndx, const int &len)
{
    QByteArray retVal;
    const int meterMessS = meterMess.size();
    if(meterMessS > startIndx){
        for(int iMax = len + startIndx ; startIndx < meterMessS && startIndx < iMax; startIndx++)
            retVal.append(meterMess.at(startIndx));
    }
    return QString(retVal);
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::uint8list2str(const QList<quint8> &meterMess, int startIndx, const int &len)
{
    QString retVal;
    const int meterMessS = meterMess.size();
    if(meterMessS > startIndx){
        QByteArray arr;
        for(int iMax = len + startIndx ; startIndx < meterMessS && startIndx < iMax; startIndx++)
            arr.append( QByteArray::number(meterMess.at(startIndx), 16).rightJustified(2, '0') );

        retVal = QString::number(arr.toULongLong(nullptr, 16));//8 bytes is the maximum
    }
    return retVal;
}
//-----------------------------------------------------------------------------------
QByteArray MeterPluginHelper::uint8list2strH(const QList<quint8> &meterMess, int startIndx, const int &len)
{
    const int meterMessS = meterMess.size();
    if(meterMessS > startIndx){
        QByteArray arr;
        for(int iMax = len + startIndx ; startIndx < meterMessS && startIndx < iMax; startIndx++)
            arr.append( QByteArray::number(meterMess.at(startIndx), 16).rightJustified(2, '0') );

        return arr;
    }
    return QByteArray();
}
//-----------------------------------------------------------------------------------
int MeterPluginHelper::getIntValFromList(const QStringList &list, const QString &key, const int &defRetVal)
{
    for(int i = 0, iMax = list.size(), len = key.length(); i < iMax; i++){
        if(list.at(i).left(len) == key){
            bool ok;
            int retVal = list.at(i).mid(len).toInt(&ok);
            if(ok)
                return retVal;

        }
    }
    return defRetVal;
}
//-----------------------------------------------------------------------------------
QByteArray MeterPluginHelper::intVal2arrH(const int &val, const int &rightJustif)
{
    return QByteArray::number(val, 16).rightJustified(rightJustif, '0');

}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::intVal2strH(const int &val, const int &rightJustif)
{
    return QString::number(val, 16).rightJustified(rightJustif, '0');
}
//-----------------------------------------------------------------------------------
QVariantHash MeterPluginHelper::oneTariff4json()
{
    QVariantHash h;
//    h.insert("Season_count", 1);
    h.insert("S0", "1.1");
    h.insert("0SDR", "0");
    h.insert("DR0", "T1{00:00-24:00}");
    return h;
}
//-----------------------------------------------------------------------------------
QStringList MeterPluginHelper::listFromArr(const QByteArray &inArr, const QByteArray &leftPart, const QByteArray &rightSymbol, const QString &separ, const bool skipEmpty)
{
    QString s = inArr.mid(inArr.indexOf(leftPart) + leftPart.length());
    int rLen = rightSymbol.length();
    if(!rightSymbol.isEmpty()){
        while(!s.isEmpty() && s.right(rLen) != rightSymbol)
            s.chop(1);
        if(s.right(rLen) == rightSymbol)
            s.chop(1);
    }
    return s.split(separ, skipEmpty ? QString::SkipEmptyParts : QString::KeepEmptyParts);

}
//-----------------------------------------------------------------------------------
bool MeterPluginHelper::dayProfilesFromInData(QList<quint8> listTariff, QStringList listTime, QVariantHash &hashTmpData, const int &lastDayProfileIndxD,  const QString &plgName)
{

    if(!listTime.isEmpty()){
        if(listTime.size() == 1){
            hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(listTariff.first())), listTime.first() + "-24:00");
        }else{
            bool allowCloseTimeInterval = (listTariff.size() > 1 && listTime.first() != "00:00");

            QString lastTimeStr = listTime.takeFirst();
            quint8 lastTarif = listTariff.takeFirst();

            //                            QString firstTimeStr(lastTimeStr);
            quint8 firstTarif = lastTarif;

            QTime lastTime = QTime::fromString(lastTimeStr + ":00.000", "hh:mm:ss.zzz");
            QTime timeMin =  lastTime;

            while(!listTime.isEmpty()){

                if(hashTmpData.contains(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)))){
                    hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)),
                                       QString("%1, %2-%3")
                                       .arg(hashTmpData.value(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif))).toString())
                                       .arg(lastTimeStr)
                                       .arg(listTime.first()) );

                }else{

                    hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)), lastTimeStr + "-" + listTime.first());
                }
                lastTimeStr = listTime.takeFirst();
                lastTarif = listTariff.takeFirst();
                lastTime = QTime::fromString(lastTimeStr + ":00.000", "hh:mm:ss.zzz");
                if(timeMin > lastTime)
                    timeMin = lastTime;
            }


            if(firstTarif == lastTarif){
                hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)),
                                   lastTimeStr + hashTmpData.value(QString(plgName + "_DP_%1_T%2")
                                                                   .arg(lastDayProfileIndxD)
                                                                   .arg(QString::number(lastTarif))).toString().mid(5));

            }else{
                QString strCloseIntervalStr = (allowCloseTimeInterval && timeMin.isValid() ) ? timeMin.toString("hh:mm") :  "24:00";


                if(hashTmpData.contains(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)))){
                    hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)),
                                       QString("%1, %2-%3")
                                       .arg(hashTmpData.value(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif))).toString())
                                       .arg(lastTimeStr).arg(strCloseIntervalStr) );

                }else{
                    hashTmpData.insert(QString(plgName + "_DP_%1_T%2").arg(lastDayProfileIndxD).arg(QString::number(lastTarif)), lastTimeStr + "-" + strCloseIntervalStr );

                }
            }

        }

//        lastDayProfileIndxD++;
//        hashTmpData.insert("MTX_lastDayProfileIndxD", lastDayProfileIndxD);
        return true;//перейти до наступного денного профілю
    }
    return false;
}
//-----------------------------------------------------------------------------------
QVariantHash MeterPluginHelper::hash4jsonTariff(const QVariantHash &hashTmpData, const int &lastDayProfileIndxD, const int &seasonMax, const QString &plgName)
{
    QVariantHash hash4json;
    /*  jObj["comment"] = "ДП НЕК \"Укренерго\" (факсограма від 25.11.2014 р. № 06-1-1/13396)";
jObj["link"] = "http://www.oe.if.ua/tarif_jur.php?id=02214";
jObj["Season_count"] = 3;
jObj["S0"] = "1.1,1.11"; // first day.month of season
jObj["S1"] = "1.3,1.9"; // first day.month of season
jObj["S2"] = "1.5"; // first day.month of season


jObj["0SDR"] = "0" ; //,2.3,2.3,2.3,2.3,1.3,1.2"; //day profile (from monday to sunday)
jObj["DR0"] = "T1{08:00-10:00, 17:00-21:00}; T2{06:00-08:00, 10:00-17:00, 21:00-23:00}; T3{23:00-06:00}";


jObj["1SDR"] = "1"; //week profile (every day)
jObj["DR1"] = "T1{08:00-10:00, 18:00-22:00}; T2{06:00-08:00, 10:00-18:00, 22:00-23:00}; T3{23:00-06:00}";

jObj["2SDR"] = "2"; //day profile (monday is a first day)
jObj["DR2"] = "T1{08:00-11:00, 20:00-23:00}; T2{07:00-08:00, 11:00-20:00, 23:00-24:00}; T3{24:00-07:00}";*/

    for(int i = 0; i < lastDayProfileIndxD; i++){
        QString dayProfile("");

        for(int t = 1 ; t < 5; t++){
            QString str = hashTmpData.value(QString(plgName + "_DP_%1_T%2").arg(i).arg(QString::number(t)) ).toString();
            if(!str.isEmpty()){
                dayProfile.append( QString("T%1{%2}; ").arg(t).arg(str));
            }
        }
        if(!dayProfile.isEmpty()){
            dayProfile.chop(2);
            hash4json.insert(QString("DR%1").arg(i), dayProfile);

        }
    }


    //            hash4json.insert("Season_count", seasonMax);
    for(int season = 0; season < seasonMax; season++ ){

        QVariantHash hash4jsonL;

        QString seasonDR = QString(hashTmpData.value( QString(plgName + "_season%1_dayProfile").arg(season) ).toStringList().join(",")).remove(' ');


        if(!seasonDR.isEmpty()){
            QStringList list = seasonDR.split(",", QString::SkipEmptyParts);
            if(list.size() == 7 && list.removeDuplicates() > 5){
                seasonDR = list.first();

            }else{
                list = seasonDR.split(",", QString::SkipEmptyParts);
                list.append(list.takeFirst());
                seasonDR = list.join(",");
            }
        }else{
            if(!hashTmpData.contains(QString(plgName + "_season%1_date").arg(season)))
                continue;
        }

        hash4jsonL.insert(QString("%1SDR").arg(season), seasonDR);

        bool buvTutTakyj = false;

        int lastSeasonBuvTutTakyj = -1;
        for(int seasonL = 0; seasonL < season && !buvTutTakyj; seasonL++){

            if(hash4jsonL.value(QString("%1SDR").arg(season)).toString() == hash4json.value(QString("%1SDR").arg(seasonL)).toString() &&
                    !hash4jsonL.value(QString("%1SDR").arg(season)).toString().isEmpty() ){
                buvTutTakyj = true;
                lastSeasonBuvTutTakyj = seasonL;
            }

//            qDebug() << "2304 season " << seasonL << season << buvTutTakyj << lastSeasonBuvTutTakyj << hash4jsonL.value(QString("%1SDR").arg(season)).toString() << hash4json.value(QString("%1SDR").arg(seasonL)).toString();
        }


        if(buvTutTakyj){
//            qDebug() << plgName + " buvTutTakyj " << lastSeasonBuvTutTakyj << season;
            if(hash4json.value(QString("S%1").arg(lastSeasonBuvTutTakyj )).toDate().toString("d.M") != hashTmpData.value(QString(plgName + "_season%1_date").arg(season)).toDate().toString("d.M"))
                hash4json.insert(QString("S%1").arg(lastSeasonBuvTutTakyj ),
                                 QString("%1,%2")
                                 .arg(hash4json.value(QString("S%1").arg(lastSeasonBuvTutTakyj)).toString())
                                 .arg(hashTmpData.value(QString(plgName + "_season%1_date").arg(season)).toDate().toString("d.M")));

            continue;
        }

        hash4json.insert(QString("S%1").arg(season ), hashTmpData.value(QString(plgName + "_season%1_date").arg(season)).toDate().toString("d.M"));

        foreach (QString key, hash4jsonL.keys()) {
            hash4json.insert(key, hash4jsonL.value(key));
        }


//        qDebug() << plgName + " hash4jsonL " << hash4jsonL ;

        hash4json.insert(QString("S%1").arg(season ), hashTmpData.value(QString(plgName + "_season%1_date").arg(season)).toDate().toString("d.M"));

        hash4json.insert(QString("%1SDR").arg(season), seasonDR);


    }

    return hash4json;
}
//-----------------------------------------------------------------------------------
int MeterPluginHelper::extendedDayProfile(QVariantHash &hashTmpData, QVariantHash &hash4json, const QString &plgName)//MTX || CE303
{
    QStringList list; //.clear();
    for( int meterDayProfileCount = TRFF_MIN_DAYPROFILE; meterDayProfileCount < TRFF_MAX_DAYPROFILE; meterDayProfileCount++){
        if(hash4json.contains(QString("DR%1").arg(meterDayProfileCount)))
            list.append(hash4json.value(QString("DR%1").arg(meterDayProfileCount)).toString());
    }

    int meterDayProfileCount = list.size();
    hashTmpData.insert(plgName + "_meterDayProfile_mas", list);
    hashTmpData.insert(plgName + "_lastDayProfileIndx", meterDayProfileCount);
    return meterDayProfileCount;
}
//-----------------------------------------------------------------------------------
QByteArray MeterPluginHelper::hexArrFromNumber(const int &inVal, const int &byteCount, const int base )
{
    return QByteArray::number(inVal,base).rightJustified(byteCount * 2, '0');
}
//-----------------------------------------------------------------------------------
int MeterPluginHelper::hoursInDay(QDateTime dateTimeL)
{
    dateTimeL.setTime(QTime(0,0,0));
    int hour = 0;
    for(int i = 0, lastHour = 0; i < 34 ; i++, hour++){
        if(dateTimeL.time().hour() < lastHour)
            break;
        lastHour = dateTimeL.time().hour();
        dateTimeL =  dateTimeL.addSecs(3600);
    }
    return hour;

}
//-----------------------------------------------------------------------------------
bool MeterPluginHelper::has25hoursInDay(QDateTime dateTimeL)
{
    return (hoursInDay(dateTimeL) == 25);
}
//-----------------------------------------------------------------------------------
bool MeterPluginHelper::has23hoursInDay(QDateTime dateTimeL)
{
return (hoursInDay(dateTimeL) == 23);
}
//-----------------------------------------------------------------------------------
qint32 MeterPluginHelper::secsTo25hour(QDateTime dateTimeL)
{
//    QDateTime dt = dateTimeL;
//    for(int i = 1; i < 12; i++){
//        dt = dateTimeL.addMonths(i);
//        if(dt.offsetFromUtc()tc()tc()tc().offsetFromUtc()romUtc()romUtc()romUtc()romUtc()romUtc() );
//            }

//            return qAbs(dt.offsetFromUtc()tc()tc()tc().offsetFromUtc()romUtc()romUtc()romUtc()romUtc()romUtc() );
//        }
//    }
//    return 0;

    QDateTime dt = dateTimeL;
    for(int i = 1; i < 12; i++){
        dt = dateTimeL.addMonths(i);
        if(dt.offsetFromUtc() != dateTimeL.offsetFromUtc()){
            /*
         * Шукаю офсет інший ніж зараз є
         * коли знаходжу то порівнюю
*/
            return qAbs(dt.offsetFromUtc() - dateTimeL.offsetFromUtc() );
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyMess(const QString &mess, const QString &hexDump, QString &lastErrorStr, QString &lastWarning, const bool &isErr)
{
    ErrsStrct e(lastErrorStr, lastWarning, isErr);
    const QString s = prettyMess(mess, hexDump, isErr, e);
    if(isErr)
        lastErrorStr = e.lastErrorStr;
    else
        lastWarning = e.lastWarning;
    return s;
}
//-----------------------------------------------------------------------------------
QString MeterPluginHelper::prettyMess(const QString &mess, const QString &hexDump, const bool &isErr, ErrsStrct &errwarn)
{
    const QString messfull = QString("%1 %2%3").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(mess).arg(hexDump );

    if(isErr)
        errwarn.lastErrorStr = messfull;
    else
        errwarn.lastWarning = messfull;
    errwarn.lastIsErr = isErr;
    return messfull;
}
//-----------------------------------------------------------------------------------
float MeterPluginHelper::hexstr2float(const QByteArray &arrh, bool &ok)
{
    union{
        quint32 i;
        float f;
    } value;
    value.i = arrh.toUInt(&ok, 16);// std::stoll(arrh.toStdString(), nullptr, 16);
    return value.f;
}
//-----------------------------------------------------------------------------------
double MeterPluginHelper::hexstr2double(const QByteArray &arrh, bool &ok)
{
    union{
        quint64 i;
        double d;
    } value;
    value.i = arrh.toULongLong(&ok, 16);// std::stoll(arrh.toStdString(), nullptr, 16);
    return value.d;
}
//-----------------------------------------------------------------------------------
quint8 MeterPluginHelper::ucmEvent2groupCode(const quint32 &ucmEventCode)
{
    quint8 localEvntCode = ZBR_EVENT_OTHER;
    switch (ucmEventCode) {

    ///MTX3 Section
    case UCM_EV_ENRG_T1_FAULT        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x01 //значення накоплюючих регістрів тарифу 1 втрачені
    case UCM_EV_ENRG_T2_FAULT        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x02 //значення накоплюючих регістрів тарифу 2 втрачені
    case UCM_EV_ENRG_T3_FAULT        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x03 //значення накоплюючих регістрів тарифу 3 втрачені
    case UCM_EV_ENRG_T4_FAULT        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x04 //значення накоплюючих регістрів тарифу 4 втрачені
    case UCM_EV_ACCESS_LOCKED        : localEvntCode = ZBR_EVENT_ACCESS              ; break;  //0x11  //доступ закритий до кінця доби із-за помилки ключа доступу
    case UCM_EV_ACCESS_UNLOCKED      : localEvntCode = ZBR_EVENT_ACCESS              ; break;  //0x12  //доступ відкритий ( була помилки ключа доступу)
    case UCM_EV_ERR_ACCESS           : localEvntCode = ZBR_EVENT_ACCESS              ; break;  //0x13 //Неправильний ключ
    case UCM_EV_CASE_OPEN            : localEvntCode = ZBR_EVENT_METER_OPEN          ; break;  //0x14 //Корпус лічильника відкритий
    case UCM_EV_CASE_CLOSE           : localEvntCode = ZBR_EVENT_METER_OPEN          ; break;  //0x15 //Корпус лічильника закритий
    case UCM_EV_MAGNETIC_ON          : localEvntCode = ZBR_EVENT_MAGNET              ; break;  //0x16 //виявлено наявність впливу постійного магнітного поля
    case UCM_EV_MAGNETIC_OFF         : localEvntCode = ZBR_EVENT_MAGNET              ; break;  //0x17 //виявлено закінчення впливу постійного магнітного поля
    case UCM_EV_CHANGE_ACCESS_KEY_0  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x20 //Ключ доступу рівня 0 змінено
    case UCM_EV_CHANGE_ACCESS_KEY_1  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x21 //Ключ доступу рівня 1 змінено
    case UCM_EV_CHANGE_ACCESS_KEY_2  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x22 //Ключ доступу рівня 2 змінено
    case UCM_EV_CHANGE_ACCESS_KEY_3  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x23 //Ключ доступу рівня 3 змінено
    case UCM_EV_CHANGE_PAR_LOCAL     : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x24 //параметри змінено локально
    case UCM_EV_CHANGE_PAR_REMOTE    : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x25 //параметри змінено локально !!!???
    case UCM_EV_CMD_CHANGE_TIME      : localEvntCode = ZBR_EVENT_DATETIME_CORRECTED  ; break;  //0x26 //отримана команда зміни часу, час змінено
    case UCM_EV_CMD_RELAY_ON         : localEvntCode = ZBR_EVENT_RELAY               ; break;  //0x27 //отримана команда увімкнення реле
    case UCM_EV_CMD_RELAY_OFF        : localEvntCode = ZBR_EVENT_RELAY               ; break;  //0x28 //отримана команда вимкнення реле
    case UCM_EV_ENERGY_REG_OVERFLOW  : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x31 //переповнення накоплюючого регістру енергії
    case UCM_EV_CHANGE_TARIFF_TBL    : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x32 //тарифний план змінено
    case UCM_EV_SET_TARIFF_TBL       : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x33 //отриманий новий тарифний план
    case UCM_EV_SUMMER_TIME          : localEvntCode = ZBR_EVENT_DST_STATE_CHANGED   ; break;  //0x34 //прехід на літній час
    case UCM_EV_WINTER_TIME          : localEvntCode = ZBR_EVENT_DST_STATE_CHANGED   ; break;  //0x35 //перехід на зимовий час
    case UCM_EV_RELAY_ON             : localEvntCode = ZBR_EVENT_RELAY               ; break;  //0x36 //реле вимкнено
    case UCM_EV_RELAY_OFF            : localEvntCode = ZBR_EVENT_RELAY               ; break;  //0x37 //реле умвімкнено
    case UCM_EV_RESTART              : localEvntCode = ZBR_EVENT_METER_ONOFF         ; break;  //0x38 //рестарт ПЗ контролера
    case UCM_EV_WD_RESTART           : localEvntCode = ZBR_EVENT_METER_ONOFF         ; break;  //0x39 //рестарт по Сторожовику
    case UCM_EV_VA_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x40 //Відновлення нормальної напруги L1 після підвищеної напруги
    case UCM_EV_VA_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x41 //напруга L1 вище порогу максимальної напруги
    case UCM_EV_VA_MIN_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x42 //Відновлення нормальної напруги L1 після пониженої напруги
    case UCM_EV_VA_MIN_UNDER         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x43 //Напруга L1 нище порогу мінімальної напруги
    case UCM_EV_VB_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x44 //Відновлення нормальної напруги L2 після підвищеної напруги
    case UCM_EV_VB_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x45 //напруга L2 вище порогу максимальної напруги
    case UCM_EV_VB_MIN_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x46 //Відновлення нормальної напруги L2 після пониженої напруги
    case UCM_EV_VB_MIN_UNDER         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x47 //Напруга L2 нище порогу мінімальної напруги
    case UCM_EV_VC_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x48 //Відновлення нормальної напруги L3 після підвищеної напруги
    case UCM_EV_VC_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x49 //напруга L3 вище порогу максимальної напруги
    case UCM_EV_VC_MIN_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4A //Відновлення нормальної напруги L3 після пониженої напруги
    case UCM_EV_VC_MIN_UNDER         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4B //Напруга L3 нище порогу мінімальної напруги
    case UCM_EV_F_MAX_OK             : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4C //Відновлення нормальної частоти після підвищеної
    case UCM_EV_F_MAX_OVER           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4D //Частота вижче порогу нормальної
    case UCM_EV_F_MIN_OK             : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4E //Відновлення нормальної частоти після пониженої
    case UCM_EV_F_MIN_UNDER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x4F //Частора нижче порогу нормальної
    case UCM_EV_T_MAX_OK             : localEvntCode = ZBR_EVENT_CLIMAT              ; break;  //0x50 //Відновлення нормальної температури після підвищеної
    case UCM_EV_T_MAX_OVER           : localEvntCode = ZBR_EVENT_CLIMAT              ; break;  //0x51 //Температура вище порогу максильної температури
    case UCM_EV_T_MIN_OK             : localEvntCode = ZBR_EVENT_CLIMAT              ; break;  //0x52 //Відновлення нормальної температури після пониженої
    case UCM_EV_T_MIN_UNDER          : localEvntCode = ZBR_EVENT_CLIMAT              ; break;  //0x53 //Температура нижче порогу нормальної
    case UCM_EV_IA_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x54 //Відновлення допустимого струму L1 після підвищеного
    case UCM_EV_IA_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x55 //Струм L1 вище порогу допустимого
    case UCM_EV_IB_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x56 //Відновлення допустимого струму L2 після підвищеного
    case UCM_EV_IB_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x57 //Струм L2 вище порогу допустимого
    case UCM_EV_IC_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x58 //Відновлення допустимого струму L3 після підвищеного
    case UCM_EV_IC_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x59 //Струм L3 вище порогу допустимого
    case UCM_EV_PA_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5A //Відновлення допустимої активної споживаної потужності після підвищеної
    case UCM_EV_PA_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5B //Активна споживана потужність вище максимальної
    case UCM_EV_PV_MAX_OK            : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5C //Відновлення допустимої реактивної споживаної потужності після підвищеної
    case UCM_EV_PV_MAX_OVER          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5D //Реактивна спожива потужність вище максимальної
    case UCM_EV_IDIFF_OK             : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5E //Відновлення допустимого дифференціального струму
    case UCM_EV_IDIFF_OVER           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x5F //Перевищення допустимого дифференціального струму
    case UCM_EV_CLOCK_OK             : localEvntCode = ZBR_EVENT_HARDWARE            ; break;  //0x60 //Нормальний стан RTC відновлено
    case UCM_EV_CLOCK_FAULT          : localEvntCode = ZBR_EVENT_HARDWARE            ; break;  //0x61 //RTC не встановлені
    case UCM_EV_POWER_C_ON           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x62 //Увімкнення напруги L3
    case UCM_EV_POWER_C_OFF          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x63 //Вимкнення напруги L3
    case UCM_EV_POWER_B_ON           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x64 //Увімкнення напруги Фази В
    case UCM_EV_POWER_B_OFF          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x65 //Вимкнення напруги Фази В
    case UCM_EV_POWER_A_ON           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x66 //Увімкнення напруги L1
    case UCM_EV_POWER_A_OFF          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM       ; break;  //0x67 //Вимкнення напруги L1
    case UCM_EV_BAT_OK               : localEvntCode = ZBR_EVENT_HARDWARE            ; break;  //0x68 //Відновлення нормальної напруги батареї RTC
    case UCM_EV_BAT_FAULT            : localEvntCode = ZBR_EVENT_HARDWARE            ; break;  //0x69 //Понижена напруга батареї RTC
    case UCM_EV_CAL_OK               : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x6A //Параметри калібровки встановлені
    case UCM_EV_CAL_FAULT            : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x6B //Калібровочні параметри втрачені
    case UCM_EV_V_PAR_OK             : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;
    case UCM_EV_V_PAR_FAULT          : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x6D //Заводські параметри втрачено
    case UCM_EV_O_PAR_OK             : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x6E //Параметри встановлено
    case UCM_EV_O_PAR_FAULT          : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x6F //Параметри втрачено
    case UCM_EV_CHANGE_COR_TIME      : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x70 //Змінено параметри переходу на літній час
    case UCM_EV_CMD_RELAY_2_ON       : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x71 //Включення другого реле
    case UCM_EV_CMD_RELAY_2_OFF      : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x72 //Вимкнення другого реле
    case UCM_EV_CRSSZERO_ENT1        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x73 //Перехід через 0 лічильника активної енергії по тарифу 1 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_ENT2        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x74 //Перехід через 0 лічильника активної енергії по тарифу 2 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_ENT3        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x75 //Перехід через 0 лічильника активної енергії по тарифу 3 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_ENT4        : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x76 //Перехід через 0 лічильника активної енергії по тарифу 4 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_VARi1       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x77 //Перехід через 0 лічильника позитивної реактивної енергії по тарифу 1 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARi2       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x78 //Перехід через 0 лічильника позитивної реактивної енергії по тарифу 2 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARi3       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;   //0x79 //Перехід через 0 лічильника позитивної реактивної енергії по тарифу 3 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARi4       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x7A //Перехід через 0 лічильника позитивної реактивної енергії по тарифу 4 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARe1       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x7B //Перехід через 0 лічильника негативної реактивної енергії по тарифу 1 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARe2       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x7C //Перехід через 0 лічильника негативної реактивної енергії по тарифу 2 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARe3       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x7D //Перехід через 0 лічильника негативної реактивної енергії по тарифу 3 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_VARe4       : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0x7E //Перехід через 0 лічильника негативної реактивної енергії по тарифу 4 при досягненні 1000000.00 кВар
    case UCM_EV_CALFLAG_SET          : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x7F //Калібровочний біт встановлено
    case UCM_EV_CALFLAG_RESET        : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x80 //Калібровочний біт скинено
    case UCM_EV_BAD_TEST_EEPROM      : localEvntCode = ZBR_EVENT_HARDWARE            ; break;   //0x81 //Тест EEPROM не пройшов
    case UCM_EV_BAD_TEST_FRAM        : localEvntCode = ZBR_EVENT_HARDWARE            ; break;   //0x82 //Тест FRAM не пройшов
    case UCM_EV_SET_NEW_SALDO        : localEvntCode = ZBR_EVENT_BILLING             ; break;  //0x83 //Отримана нова передоплата
    case UCM_EV_SALDO_PARAM_BAD      : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x84 //Параметри сальдо втрачені
    case UCM_EV_ACCPARAM_BAD         : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x85 //Акумуляційні параметри втрачено
    case UCM_EV_ACCPARAM_EXT_BAD     : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x86 //Додаткові акумуляційні параметри втрачено
    case UCM_EV_CALC_PERIOD_BAD      : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x87 //Дані розрахункових періодів втрачено
    case UCM_EV_BLOCK_TARIF_BAD      : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x88 //Параметри блочних тарифів втрачено
    case UCM_EV_CALIBR_PARAM_BAD     : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x89 //Значення калібровочних параметрів втрачено
    case UCM_EV_WINTER_SUMMER_BAD    : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x8A //Значення параметру переходу на зимовий/літній час втрачено
    case UCM_EV_OP_PARAM_BAD         : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x8B //Операторські параметри втрачено
    case UCM_EV_OP_PARAM_EXT_BAD     : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x8C //Операторські параметри 2 втрачено
    case UCM_EV_SALDO_EN_BAD         : localEvntCode = ZBR_EVENT_PARAM_FAULT         ; break;  //0x8D //Значення енергій для розрахунку сальдо втрачено
    case UCM_EV_TIME_CORRECT         : localEvntCode = ZBR_EVENT_DATETIME_CORRECTED  ; break;  //0x8E //Коррекція часу
    case UCM_EV_COEFF_TRANSF_CHANGE  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x8F //Змінено коефіцієнти трансформації
    case UCM_EV_RELAY_HARD_BAD_OFF   : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x90 //Реле вимкнено механічно
    case UCM_EV_RELAY_HARD_ON        : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x91 //Реле увімкнено після механічного впливу. Відновлено стан реле
    case UCM_EV_RELAY_HARD_BAD_ON    : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x93 //Реле увімкнено механічно
    case UCM_EV_RELAY_HARD_OFF       : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x94 //Реле вимкнено після механічного впливу. Відновлено стан реле
    case UCM_EV_METER_TROUBLE        : localEvntCode = ZBR_EVENT_INTERNAL_ERROR      ; break;   //0x95 //Збій лічильника
    case UCM_EV_CASE_KLEMA_OPEN      : localEvntCode = ZBR_EVENT_KLEMA_OPEN          ; break;   //0x96 //Клемна коробка лічильника відкрита
    case UCM_EV_CASE_KLEMA_CLOSE     : localEvntCode = ZBR_EVENT_KLEMA_OPEN          ; break;   //0x97 //Клемна коробка лічильника закрита
    case UCM_EV_CHANGE_TARIFF_TBL_2  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x98 //Тарифний план 2 змінено
    case UCM_EV_CHANGE_TARIFF_TBL_3  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x99 //Тарифний план 3 змінено
    case UCM_EV_CASE_MODULE_OPEN     : localEvntCode = ZBR_EVENT_MODULE_OPEN         ; break;  //0x9A //Відсік модуля лічильника відкрито
    case UCM_EV_CASE_MODULE_CLOSE    : localEvntCode = ZBR_EVENT_MODULE_OPEN         ; break;  //0x9B //Відсік модуля лічильника закрито
    case UCM_EV_SET_SALDO_PARAM      : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x9C //Параметри сальдо встановлено
    case UCM_EV_POWER_OVER_RELAY_OFF : localEvntCode = ZBR_EVENT_RELAY               ; break;   //0x9D //Викнення реле після перевищення активної потужності (Версія ПЗ >= 302.15.001)
    case UCM_EV_CHANGE_PARAM_CANAL1  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x9E //Змінено параметр профіля навантаження 1 (Версія ПЗ >= 302.17.001)
    case UCM_EV_CHANGE_PARAM_CANAL2  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0x9F //Змінено парамтер профіля навантаження 2
    case UCM_EV_CHANGE_PARAM_CANAL3  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0xA0 //Змінено параметр профіля навантаження 3
    case UCM_EV_CHANGE_PARAM_CANAL4  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0xA1 //Змінено параметр профіля навантаження 4
    case UCM_EV_CHANGE_PARAM_CANAL5  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0xA2 //Змінено параметр профіля навантаження 5
    case UCM_EV_CHANGE_PARAM_CANAL6  : localEvntCode = ZBR_EVENT_CHANGED_PARAM       ; break;  //0xA3 //Змінено параметр профіля навантаження 6
    case UCM_EV_CRSSZERO_EXP_ENT1    : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA4 //Перехід через 0 лічильника активної експортної енергії по тарифу 1 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_EXP_ENT2    : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA5 //Перехід через 0 лічильника активної експортної енергії по тарифу 2 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_EXP_ENT3    : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA6 //Перехід через 0 лічильника активної експортної енергії по тарифу 3 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_EXP_ENT4    : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA7 //Перехід через 0 лічильника активної експортної енергії по тарифу 4 при досягненні 1000000.00 кВт
    case UCM_EV_CRSSZERO_EXP_VARi1   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA8 //Перехід через 0 лічильника реактивної позитивної експортної енергії по тарифу 1 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARi2   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xA9 //Перехід через 0 лічильника реактивної позитивної експортної енергії по тарифу 2 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARi3   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xAA //Перехід через 0 лічильника реактивної позитивної експортної енергії по тарифу 3 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARi4   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xAB //Перехід через 0 лічильника реактивної позитивної експортної енергії по тарифу 4 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARe1   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;   //0xAC //Перехід через 0 лічильника реактивної негативної експортної енергії по тарифу 1 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARe2   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xAD //Перехід через 0 лічильника реактивної негативної експортної енергії по тарифу 2 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARe3   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xAE //Перехід через 0 лічильника реактивної негативної експортної енергії по тарифу 3 при досягненні 1000000.00 кВар
    case UCM_EV_CRSSZERO_EXP_VARe4   : localEvntCode = ZBR_EVENT_METERING_EV         ; break;  //0xAF //Перехід через 0 лічильника реактивної негативної експортної енергії по тарифу 4 при досягненні 1000000.00 кВар
    case UCM_EV_EM_MAGNETIC_ON       : localEvntCode = ZBR_EVENT_MAGNET              ; break;   //0xB0 //Виявлено наявність впливу змінного магнітного поля
    case UCM_EV_EM_MAGNETIC_OFF      : localEvntCode = ZBR_EVENT_MAGNET              ; break;   //0xB1 //Виявлено закінчення впливу змінного магнітного поля

        ///MTX1 Section
    case UCM_EV_ENERGY_REG_FAULT        : localEvntCode = ZBR_EVENT_METERING_EV      ; break;//  0x101 // Значення накоплюючих регістрів втрачено

    case UCM_EV_4K_POWERSALDO_OK        : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//        0x150 // Перевещення потужності в режимі кредиту відсутнє
    case UCM_EV_4K_POWERSALDO_OVER      : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//        0x151 // Перевищена потужність в режимі кредиту
    case UCM_EV_4K_POWER_GOOD_DIO       : localEvntCode = ZBR_EVENT_HARDWARE         ; break;//        0x17E //Відсутній сигнал POWER_GOOD

    case UCM_EV_4K_CURRENT_UNEQUIL_FAULT: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//   0x1B2 // Виявлено нерівність струмів
    case UCM_EV_4K_CURRENT_UNEQUIL_OK   : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//   0x1B3 // Нерівність струмів відсутня
    case UCM_EV_4K_BIPOLAR_POWER_FAULT  : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//   0x1B4 // Виявлені різнополярні потужності у фазі та нейтралі
    case UCM_EV_4K_BIPOLAR_POWER_OK     : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//   0x1B5 // Різнополярні потужності у фазі та нейтралі відсутні
    case UCM_EV_4K_RESET_EM_FLAG        : localEvntCode = ZBR_EVENT_MAGNET           ; break;//   0x1B6 // Скинуто екран ел.магнітного впливу
    case UCM_EV_4K_RESET_MAGN_FLAG      : localEvntCode = ZBR_EVENT_MAGNET           ; break;//    0x1B7 // Скинуто екрат магнітного впливу


        ///Global section                         +0x200
        //BUS
    case UCM_EV_BUS_I2C_FAULT           : localEvntCode = ZBR_EVENT_HARDWARE        ; break;//   0x200 //


        //Meter STATE OK
    case UCM_EV_MSTATE_DIAGNOSTIC_OK    : localEvntCode = ZBR_EVENT_HARDWARE        ; break;//     0x300 //

    case UCM_EV_RELAY_OFF_HIGH_VLTG     : localEvntCode = ZBR_EVENT_RELAY           ; break; //     0x402 //Реле вимкнено по перевищенню напруги
    case UCM_EV_RELAY_OFF_LOW_VLTG      : localEvntCode = ZBR_EVENT_RELAY           ; break; //         0x403 //Реле вимкнено по зниженню напруги
    case UCM_EV_RELAY_OFF_HACKER        : localEvntCode = ZBR_EVENT_RELAY           ; break; //          0x404 //Реле вимкнено із-за несанкціонованого доступу до зажимної плати лічильника

        //clear data
    case UCM_EV_CLEAR_LOADPROFILEDATA   : localEvntCode = ZBR_EVENT_METERING_EV     ; break;//        0x500 //



    case UCM_EV_MSTATE_POWERON          : localEvntCode = ZBR_EVENT_METER_ONOFF      ; break; //             0x301 //
    case UCM_EV_MSTATE_POWEROFF         : localEvntCode = ZBR_EVENT_METER_ONOFF      ; break; //             0x302 //

        //Події що фіксуюють лише початок зміни стану
    case UCM_EV_VA_MIN_UNDER_SS         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //             0x600 //Напруга L1 нище порогу мінімальної напруги, не очікую на нормальну
    case UCM_EV_VB_MIN_UNDER_SS         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //             0x601 //Напруга L2 нище порогу мінімальної напруги, не очікую на нормальну
    case UCM_EV_VC_MIN_UNDER_SS         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //             0x602 //Напруга L3 нище порогу мінімальної напруги, не очікую на нормальну

    case UCM_EV_VA_MAX_OVER_SS          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //              0x603 //напруга L1 вище порогу максимальної напруги, не очікую на нормальну
    case UCM_EV_VB_MAX_OVER_SS          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //             0x604 //напруга L2 вище порогу максимальної напруги, не очікую на нормальну
    case UCM_EV_VC_MAX_OVER_SS          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break; //             0x605 //напруга L3 вище порогу максимальної напруги, не очікую на нормальну
    case UCM_EV_CASE_OPEN_SS            : localEvntCode = ZBR_EVENT_METER_OPEN       ; break; //            0x606 //Корпус лічильника відкритий, не очікую на закриття
    case UCM_EV_CASE_KLEMA_OPEN_SS      : localEvntCode = ZBR_EVENT_KLEMA_OPEN       ; break; //         0x607 //Клемна коробка лічильника відкрита, не очікую на закриття

    case UCM_EV_VA_ON_SS                : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                    0x608 //Є напруга L1
    case UCM_EV_VB_ON_SS                : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                    0x609 //Є напруга L2
    case UCM_EV_VC_ON_SS                : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                   0x60A //Є напруга L3

        //set new meter settings
    case UCM_EV_SET_PARAM               : localEvntCode = ZBR_EVENT_CHANGED_PARAM   ; break; //                0x1000 // узагальнена подія змін параметрів лічильника, параметризація.




        ///[HELLO DLMS]

        ///STANDARD EVENT LOG
        //RESET AND SUPPLY DIRUPTIONS   +0x2000
    case UCM_EV_STANDARD_RESET_WTH_DATA_LOSS        : localEvntCode = ZBR_EVENT_PARAM_FAULT     ; break; //   0x2001
    case UCM_EV_STANDARD_RESET_WTHOUT_DATA_LOSS     : localEvntCode = ZBR_EVENT_PARAM_FAULT     ; break; //        0x2002
//    case UCM_EV_STANDARD_POWER_OUTAGES              : localEvntCode = ZBR_EVENT_METER_ONOFF   ; break; //           0x2003
    case UCM_EV_STANDARD_NO_CONNECTION_2_N          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM   ; break; //           0x2004

        //CHANGE OF PARAMETERS
    case UCM_EV_STANDARD_CHNG_PRMTRS_REGISTER       : localEvntCode = ZBR_EVENT_CHANGED_PARAM    ; break; //          0x201E //30
    case UCM_EV_STANDARD_CHNG_PRMTRS_COMM_PORT      : localEvntCode = ZBR_EVENT_CHANGED_PARAM    ; break; //          0x201F //31
        //case UCM_EV_STANDARD_CHNG_PASSWD_2_READ              UCM_EV_CHANGE_ACCESS_KEY_3// 0x2020 //32
        //case UCM_EV_STANDARD_CHNG_PASSWD_4_PRM               UCM_EV_CHANGE_ACCESS_KEY_2//  0x2021 //33
        //case UCM_EV_STANDARD_CHNG_PASSWD_4_FIRMWR            UCM_EV_CHANGE_ACCESS_KEY_1//  0x2022 //34

        //case UCM_EV_STANDARD_CHNG_TIME_OF_SSN_TRANSTN        UCM_EV_CHANGE_COR_TIME// 0x2024 //36
    case UCM_EV_STANDARD_CHNG_MIN_TIME_BTWN_INVOICN : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //    0x2025 //37
    case UCM_EV_STANDARD_CHNG_PRD_4_LOADPROFILE     : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //      0x2026 //38
    case UCM_EV_STANDARD_CHNG_SYNC                  : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //                0x2027 //39
    case UCM_EV_STANDARD_CHNG_PROGRAM_NAME          : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //            0x2028 //40


    case UCM_EV_ACIN1_NO_VOLTAGE                    : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                  0x2032//50 - ACIN1 state changed to on
    case UCM_EV_ACIN1_VOLTAGE                       : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                  0x2033//51 - ACIN1 state changed to off
    case UCM_EV_ACIN2_NO_VOLTAGE                    : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                  0x2034//52 - ACIN2 state changed to on
    case UCM_EV_ACIN2_VOLTAGE                       : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM    ; break;//                 0x2035//53 - ACIN2 changed to off

        //CHANGING PARAMETERS OF PWR QUALITY SETT
    case UCM_EV_STANDARD_CHNG_DURTN_OF_VLTG_SGS_SWLS    : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //   0x205A //90
    case UCM_EV_STANDARD_CHNG_LIMIT_OF_PWR_OUTAGE       : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //    0x205B //91
    case UCM_EV_STANDARD_CHNG_RATED_VOLTAGE             : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //     0x205C //92
    case UCM_EV_STANDARD_CHNG_UPPR_LIMIT_PERMSSBL_VLTG  : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //  0x205D //93
    case UCM_EV_STANDARD_CHNG_LOWR_LIMIT_PERMSSBL_VLTG  : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; // 0x205E //94
    case UCM_EV_STANDARD_CHNG_LACK_OF_VTG               : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //            0x205F //95

        // RESET
    case UCM_EV_STANDARD_PASSWRD_RESET              : localEvntCode = ZBR_EVENT_PARAM_FAULT ; break; //           0x2060 //96
    case UCM_EV_STANDARD_ALL_2_FACTORY_SETT         : localEvntCode = ZBR_EVENT_PARAM_FAULT ; break; //           0x2061 //97


        ///FIRMWARE UPDATE LOG
        //UPDATE
    case UCM_EV_FIRMWARE_CHNG_PROGRAM               : localEvntCode = ZBR_EVENT_CHANGED_PARAM; break; //            0x2501


        ///CLOCK SYNC LOG
        //CLOCK SYNC
    case UCM_EV_CLOCK_SYNC                          : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //                  0x2601




        ///POWER FAILURE (OUTAGES)
        //POWER OUTAGES AND RECOVERY
//    case UCM_EV_PWR_OUTAGE_OUTG_IN_ALL_PHASES       : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //         0x2701
        //case UCM_EV_PWR_OUTAGE_OUTG_L1                       UCM_EV_POWER_A_OFF// 0x2702
        //case UCM_EV_PWR_OUTAGE_OUTG_L2                       UCM_EV_POWER_B_OFF// 0x2703
        //case UCM_EV_PWR_OUTAGE_OUTG_L3                       UCM_EV_POWER_C_OFF// 0x2704
//    case UCM_EV_PWR_OUTAGE_RTRN_IN_ALL_PHASES       : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //         0x2705
        //case UCM_EV_PWR_OUTAGE_RTRN_L1                       UCM_EV_POWER_A_ON// 0x2706
        //case UCM_EV_PWR_OUTAGE_RTRN_L2                       UCM_EV_POWER_B_ON// 0x2707
        //case UCM_EV_PWR_OUTAGE_RTRN_L3                       UCM_EV_POWER_C_ON// 0x2708


        ///POWER QUALITY
        //VOLTAGE SAGS AND SWELLS



    case UCM_EV_PWR_QLT_VLTG_SAG_10_UN_L1           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //        0x2901
    case UCM_EV_PWR_QLT_VLTG_SAG_10_UN_L2           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2902
    case UCM_EV_PWR_QLT_VLTG_SAG_10_UN_L3           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2903
    case UCM_EV_PWR_QLT_VLTG_SAG_20_UN_L1           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2904
    case UCM_EV_PWR_QLT_VLTG_SAG_20_UN_L2           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2905
    case UCM_EV_PWR_QLT_VLTG_SAG_20_UN_L3           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2906
    case UCM_EV_PWR_QLT_VLTG_SAG_50_UN_L1           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //       0x2907
    case UCM_EV_PWR_QLT_VLTG_SAG_50_UN_L2           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //      0x2908
    case UCM_EV_PWR_QLT_VLTG_SAG_50_UN_L3           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //      0x2909
    case UCM_EV_PWR_QLT_VLTG_SWLL_10_UN_L1          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //     0x290A
    case UCM_EV_PWR_QLT_VLTG_SWLL_10_UN_L2          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //      0x290B
    case UCM_EV_PWR_QLT_VLTG_SWLL_10_UN_L3          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //   0x290C
    case UCM_EV_PWR_QLT_VLTG_RTRN_2_ACCPTBL_VALUE_L1: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //   0x290D
    case UCM_EV_PWR_QLT_VLTG_RTRN_2_ACCPTBL_VALUE_L2: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //   0x290E
    case UCM_EV_PWR_QLT_VLTG_RTRN_2_ACCPTBL_VALUE_L3: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //   0x290F

    case UCM_EV_IN_MAX_OVER                         : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM; break;//   0x2917 //23 Over current in neutral
    case UCM_EV_IN_MAX_OK                           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM; break;//                     0x291B //27 Current I1 returned to an acceptable value

    case UCM_EV_PWR_QLT_VLTG_VSS_RCURR_ON           : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM; break;//           0x291E//30 Revers current detected at least in one of phases
    case UCM_EV_PWR_QLT_VLTG_VSS_RCURR_OFF          : localEvntCode = ZBR_EVENT_VOLTAGE_PARAM; break;//          0x291F//31 No reverse current




        ///FRAUD DETECTION
        //Theft events register
        //case UCM_EV_THFT_OPENING_COVER                       UCM_EV_CASE_OPEN//  0x3101
        //case UCM_EV_THFT_CLOSING_COVER                       UCM_EV_CASE_CLOSE// 0x3102
        //case UCM_EV_THFT_DETECTON_MAGNETIC_FIELD             UCM_EV_MAGNETIC_ON// 0x3103
        //case UCM_EV_THFT_DISAPPRNCE_OF_MAGNETIC_FIELD        UCM_EV_MAGNETIC_OFF//    0x3104
    case UCM_EV_THFT_DTCTN_OF_PHASE_AND_ZERO_RPLC_BEG: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //    0x3105
    case UCM_EV_THFT_DTCTN_OF_PHASE_AND_ZERO_RPLC_END: localEvntCode = ZBR_EVENT_VOLTAGE_PARAM ; break; //    0x3106
        //case UCM_EV_THFT_REMOVNG_COVER_OF_TRMNL_STRIP        UCM_EV_CASE_KLEMA_OPEN//  0x3107
        //case UCM_EV_THFT_CLOSING_COVER_OF_TRMNL_STRIP        UCM_EV_CASE_KLEMA_CLOSE//    0x3108
        //case UCM_EV_THFT_DIFF_CURRENT_DETECTION_BEG          UCM_EV_IDIFF_OVER//    0x3109
        //case UCM_EV_THFT_DIFF_CURRENT_DETECTION_END          UCM_EV_IDIFF_OK//    0x310A
    case UCM_EV_THFT_KOPEN_SUS_ON                   : localEvntCode = ZBR_EVENT_KLEMA_OPEN          ; break; //gama 87 terminal cover opened in security suspend mode





        ///COMMUNICATION IFACE
        //COMMUNICATION EVENTS
    case UCM_EV_COMM_IFACE_PLC_PORT_END         : localEvntCode = ZBR_EVENT_ACCESS ; break; //           0x3301
    case UCM_EV_COMM_IFACE_PLC_PORT_BEG         : localEvntCode = ZBR_EVENT_ACCESS ; break; //          0x3302
    case UCM_EV_COMM_IFACE_OPT_PORT_END         : localEvntCode = ZBR_EVENT_ACCESS ; break; //           0x3303
    case UCM_EV_COMM_IFACE_OPT_PORT_BEG         : localEvntCode = ZBR_EVENT_ACCESS ; break; //          0x3304
    case UCM_EV_COMM_IFACE_ETH_PORT_END         : localEvntCode = ZBR_EVENT_ACCESS ; break; //          0x3305
    case UCM_EV_COMM_IFACE_ETH_PORT_BEG         : localEvntCode = ZBR_EVENT_ACCESS ; break; //          0x3306
    case UCM_EV_COMM_IFACE_3GPP_PORT_END        : localEvntCode = ZBR_EVENT_ACCESS ; break; //         0x3307
    case UCM_EV_COMM_IFACE_3GPP_PORT_BEG        : localEvntCode = ZBR_EVENT_ACCESS ; break; //         0x3308

    case UCM_EV_SET_PARAM_TXT                   : localEvntCode = ZBR_EVENT_CHANGED_PARAM ; break; //                  0x2601


    }
    return localEvntCode;

}
//-----------------------------------------------------------------------------------
QVariantHash MeterPluginHelper::addOneKeyValue(const QString &key, const QVariant &val, QVariantHash h)
{
    h.insert(key, val);
    return h;
}
//-----------------------------------------------------------------------------------
void MeterPluginHelper::copyHash2hash(const QVariantHash &src, QVariantHash &hashDest)
{
    const QList<QString> k = src.keys();
    for(int i = 0, iMax = k.size(); i < iMax; i++)
        hashDest.insert(k.at(i), src.value(k.at(i)));
}
//-----------------------------------------------------------------------------------
void MeterPluginHelper::copyHash2hashKeys(const QVariantHash &src, QVariantHash &hashDest, const QString &startWithLine, const QString &separ)
{
    const QStringList l = startWithLine.split(separ, QString::SkipEmptyParts);
    const QList<QString> k = src.keys();

    const int lMax = l.size();
    for(int i = 0, iMax = k.size(); i < iMax; i++){
        for(int j = 0; j < lMax; j++){
            if(k.at(i).startsWith(l.at(j))){
                hashDest.insert(k.at(i), src.value(k.at(i)));
                break;
            }
        }
    }

}
//-----------------------------------------------------------------------------------
void MeterPluginHelper::copyHash2hashErrAndWarn(const QVariantHash &src, QVariantHash &hashDest)
{
    copyHash2hashKeys(src, hashDest, "Error_ Warning_", " ");
}

//-----------------------------------------------------------------------------------

QByteArray MeterPluginHelper::convertData7ToData8(const QByteArray &readArr)
{
    const int iMax = readArr.size();
    QByteArray arr(iMax, 0x0);

    for(int i = 0; i < iMax; i++){
        QBitArray bitArr(8);
        for(int b = 0; b < 8; ++b)
            bitArr.setBit(b, readArr.at(i) & (1 << b));

        bitArr.setBit( 7, false);

        for(int b = 0; b < 8; b++){
            arr[i] = arr.at(i) | ((bitArr.at(b) ? 1 : 0) << (b%8));
        }

    }
    return arr;

}

//-----------------------------------------------------------------------------------

QByteArray MeterPluginHelper::convertData8ToData7(const QByteArray &writeArr)
{
    const int iMax = writeArr.size();
    QByteArray arr(iMax, 0x0);

    for(int i = 0; i < iMax; i++){
        QBitArray bitArr(8);
        int counter = 0;
        for(int b = 0; b < 8; ++b){
            bitArr.setBit(b, writeArr.at(i) & (1 << b));
            if(bitArr.at(b) && b < 7)
                counter++;
        }

        bool falseVal = true;
        if(counter%2 == 0)
            falseVal = false;

        bitArr.setBit( 7, falseVal);

        for(int b = 0; b < 8; b++){
          arr[i] = arr.at(i) | ((bitArr.at(b) ? 1 : 0) << (b%8));
        }
    }
    return arr;
}

//-----------------------------------------------------------------------------------

QByteArray MeterPluginHelper::listUint8mid2arr(const QList<quint8> &meterMess, const int &startPos, const int &len)
{   
    return QByteArray::number(listUint8mid2arrMess(meterMess, startPos, len).toHex().toULongLong(0, 16)) ;
}
//-----------------------------------------------------------------------------------
QByteArray MeterPluginHelper::listUint8mid2arrMess(const QList<quint8> &meterMess, const int &startPos, const int &len)
{
    QByteArray arr;
    for(int i = startPos, iMax = meterMess.size(), l = 0; i < iMax && l != len; i++, l++)
        arr.append(meterMess.at(i));
    return arr;
}

//-----------------------------------------------------------------------------------

qreal MeterPluginHelper::listUint8mid2real(const QList<quint8> &meterMess, const int &startPos, const int &len, const int &dotPos, const int isUnsigned, bool &ok)
{
    qreal dilnyk = 1.0;
    for(int i = 0; i < dotPos; i++)
        dilnyk *= 10;

    bool ok1, ok2;

//    QByteArray arrh = uint8list2strH(meterMess, startPos, len);


    qint64 valInteger = listUint8mid2arr(meterMess, startPos, len).toLongLong(&ok1);
    quint32 valUnsignedInteger = listUint8mid2arr(meterMess, startPos, len).toUInt(&ok2);

    if(valInteger > 2147483647){
//        qint64 v = 0xFFFFFFFF;
        valInteger -= (qint64)0xFFFFFFFF;
        valInteger--;
    }

    qreal val = 0;//
    if(isUnsigned){
        val = (qreal)valUnsignedInteger;
    }else{
        val = (qreal)valInteger;
    }
    val /= dilnyk;
//    (qreal)( (isUnsigned != 0) ? valUnsignedInteger : valInteger) / dilnyk;//  (listUint8mid2arr(meterMess, startPos, len).toDouble(&ok) / dilnyk);
    ok = isUnsigned ? ok2 : ok1;
    return val;
}
//-----------------------------------------------------------------------------------


QString MeterPluginHelper::getMeterVersion(const QVariantHash &hashTmpData, const QVariantHash &hashConstData)
{
    return hashTmpData.value("vrsn").toString().isEmpty() ? hashConstData.value("vrsn").toString() : hashTmpData.value("vrsn").toString();
}
//-----------------------------------------------------------------------------------
bool MeterPluginHelper::getCorrDateTime(QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt)
{
//    const bool lockWriteDt = (hashConstData.value("memo").toString().startsWith("DNWT ") || hashConstData.value("dta", false).toBool());
//    const bool corDTallow = lockWriteDt ? false : hashConstData.value("corDTallow", false).toBool();

    return getCorrDateTimeExt(true, hashTmpData, hashConstData, meterDateTime, err, warn, mtdEvnt);
}

//-----------------------------------------------------------------------------------

bool MeterPluginHelper::getCorrDateTimeExt(const bool &ignoreDiff, QVariantHash &hashTmpData, const QVariantHash &hashConstData, const QDateTime &meterDateTime, QString &err, QString &warn, QString &mtdEvnt)
{
    //true - allowExchange
    //false - stop reading meter data

    const bool lockWriteDt = (hashConstData.value("memo").toString().startsWith("DNWT ") || hashConstData.value("dta", false).toBool());
    const QDateTime currDt = QDateTime::currentDateTime();
    const qint64 diff = meterDateTime.secsTo(currDt);
    hashTmpData.remove("corrDateTime");

    if(!ignoreDiff && lockWriteDt){
        //lock write
        if(qAbs(diff) > MAX_DIFF_DT_SECS){
            hashTmpData.insert("step", 0xFFFF);
            err = tr("Time correction: disabled, diff.: %1 [s], exiting").arg(diff);
            return false;
        }
        warn = tr("Time correction: disabled, diff.: %1 [s], ignoring").arg(diff);
    }

    if(!ignoreDiff && !lockWriteDt && qAbs(diff) > hashConstData.value("corDTintrvl", SETT_DT_DIFF_MAX_SECS).toInt()){ //need to correct dateTime  90 sec;
        hashTmpData.insert("corrDateTime", true);
        mtdEvnt = tr("Meter's date is %1 UTC%2%3").arg(meterDateTime.toString("yyyy-MM-dd hh:mm:ss")).arg( (meterDateTime.offsetFromUtc() > 0) ? "+" : "" ).arg(QString::number(meterDateTime.offsetFromUtc())) ;
        warn = tr("Need to correct time, diff.: %1 [s]").arg(diff);

        if(hashConstData.value("corDTallow", false).toBool())
            return true;

        hashTmpData.insert("step", 0xFFFF);
        err = tr("Time correction: disabled, diff.: %1 [s], exiting").arg( diff );
        return false;
    }

    //regular condition,
    hashTmpData.insert("currentDateTime"    , currDt);
    hashTmpData.insert("lastMeterDateTime"  , meterDateTime);
    hashTmpData.insert("dateTime"           , meterDateTime);
    hashTmpData.insert("timeDiffSec"        , diff);
    return true;

}

//-----------------------------------------------------------------------------------

int MeterPluginHelper::getTariffCount(const QVariantHash &hashConstData)
{
    int maxTarff = hashConstData.value("trff").toInt();
    if(maxTarff < 1)
        maxTarff = DEF_TARIFF_NUMB;

    return (maxTarff > 4) ? 4 : maxTarff;

}


//----------------------------------------------------------------------------

bool MeterPluginHelper::isTime4updateSchedule(const QVariantHash &hashConstDataSchedule, const QVariantHash &currentMeterSchedule)
{
    const QStringList lk = QString("dom dow hour minute actvt").split(" ", QString::SkipEmptyParts);// waterSleepParamKeys();
    for(int i = 0, imax = lk.size(); i < imax; i++){
        if(hashConstDataSchedule.value(lk.at(i)).toString() != currentMeterSchedule.value(lk.at(i)).toString() || currentMeterSchedule.value(lk.at(i)).toString().isEmpty()){
            bool okl, okr;

            const quint8 vl = hashConstDataSchedule.value(lk.at(i)).toUInt(&okl);
            const quint8 vr = currentMeterSchedule.value(lk.at(i)).toUInt(&okr);

            if(okl && okr && vl == vr)
                continue;// if 00 != 0
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------

bool MeterPluginHelper::isTime4updateScheduleHConstData(const QVariantHash &hashConstData, const QVariantHash &currentMeterSchedule)
{
    return isTime4updateSchedule(hashConstData.value("sleepprofile").toHash(), currentMeterSchedule);
}

//-----------------------------------------------------------------------------------
