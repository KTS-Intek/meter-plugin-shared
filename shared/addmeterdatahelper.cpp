#include "addmeterdatahelper.h"


//-------------------------------------------------------------------------------------------------------------

int AddMeterDataHelper::fullEmptyCells(ListEmptyValsCache &cache, QVariantHash &hashTmpData, const quint8 &timeCoefitient, const QStringList &listTariffEnrg)
{
    const int enrgSize = listTariffEnrg.size();

    int lastEmptyValIndex = -1;
    int counter = 0;
    QStringList listDate = hashTmpData.value("listDate").toStringList();

    for(int i = 0 ; i < cache.listSize; i++){
        const EmptyValDtEnrgTrf empty = cache.list.at(i);

        if(!empty.need2addEmptyVal2oldPeriod || !empty.dt.isValid())
            continue;
        lastEmptyValIndex = i;
        const QString dtstr = FindEmptyDate::dtStr4hashTmp(empty.dt, timeCoefitient);
        if(dtstr.isEmpty())
            continue;

        if(!listDate.contains(dtstr))
            listDate.append(dtstr);
        counter++;
        for(int e = 0; e < enrgSize; e++)
            hashTmpData.insert(QString("%1_%2").arg(dtstr).arg(listTariffEnrg.at(e)), "-");

    }
    if(counter > 0)
        hashTmpData.insert("listDate", listDate);

    lastEmptyValIndex++;
    cache.lastDtIndx = lastEmptyValIndex;
    return counter;
}

void AddMeterDataHelper::insertNotSupValues2hash(const QString &prefics, const QString &enrg, const int &trffs, QVariantHash &hash)
{
    insertStatuses2hash(prefics, enrg, trffs, "!", hash);

}

void AddMeterDataHelper::insertStatuses2hash(const QString &prefics, const QString &enrg, const int &trffs, const QString &stts, QVariantHash &hash)
{
    if(enrg.isEmpty() || stts.isEmpty())
        return;//it is impossible
    const QString fullprefics = prefics.isEmpty() ? QString() : QString("%1_").arg(prefics);
    for(int i = 0; i <= trffs; i++)
        hash.insert(QString("%1T%2_%3").arg(fullprefics).arg(i).arg(enrg), stts);
}

//-------------------------------------------------------------------------------------------------------------
