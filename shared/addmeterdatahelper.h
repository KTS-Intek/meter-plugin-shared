#ifndef ADDMETERDATAHELPER_H
#define ADDMETERDATAHELPER_H

#include <QVariantHash>
#include "shared/findemptydate.h"

class AddMeterDataHelper
{

public:
    static int fullEmptyCells(ListEmptyValsCache &cache, QVariantHash &hashTmpData, const quint8 &timeCoefitient, const QStringList &listTariffEnrg);


};

#endif // ADDMETERDATAHELPER_H
