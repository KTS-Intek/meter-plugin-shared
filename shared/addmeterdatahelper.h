#ifndef ADDMETERDATAHELPER_H
#define ADDMETERDATAHELPER_H

#include <QVariantHash>
#include "shared/findemptydate.h"

class AddMeterDataHelper
{

public:
    static int fullEmptyCells(ListEmptyValsCache &cache, QVariantHash &hashTmpData, const quint8 &timeCoefitient, const QStringList &listTariffEnrg);


    static void insertNotSupValues2hash(const QString &prefics, const QString &enrg, const int &trffs, QVariantHash &hash);

    static void insertStatuses2hash(const QString &prefics, const QString &enrg, const int &trffs, const QString &stts, QVariantHash &hash);


};

#endif // ADDMETERDATAHELPER_H
