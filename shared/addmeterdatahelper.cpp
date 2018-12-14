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

//-------------------------------------------------------------------------------------------------------------
