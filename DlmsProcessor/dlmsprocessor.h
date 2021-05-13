#ifndef DLMSPROCESSOR_H
#define DLMSPROCESSOR_H

#include <QObject>

class DlmsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DlmsProcessor(QObject *parent = nullptr);

signals:

};

#endif // DLMSPROCESSOR_H
