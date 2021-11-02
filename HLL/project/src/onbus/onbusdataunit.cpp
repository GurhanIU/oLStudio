#include "onbusdataunit.h"

#include "onbusdata.h"

namespace Private {

enum struct Type {
    Request,
    Response
};

/// TODO Burasi ayrilacak gibi duruyor.
static QByteArray collateDataset(const QList<OnBusData*> &dataset)
{
    if (dataset.isEmpty())
        return QByteArray();
    QByteArray packet;
//    packet.append((char)(dataset.count()*5 + 1)); // 5: Her veri icin 4byte adres bilgisi ve 1 adet boyut bilgisi; 1: toplam veri adedi
//    packet.append((char)dataset.count()); // toplam veri adedi
    foreach (OnBusData *data, dataset ) {
        bool isOk = false;
        QMetaType t(data->dataType());
        packet.append((char)data->address());
        packet.append((char)(data->address() >> 8));
        packet.append((char)(data->address() >> 16));
        packet.append((char)(data->address() >> 24));
        packet.append((char)t.sizeOf());
//        qDebug() << data->data()->toUShort(&isOk);
//        packet.append((char)(data->data()->toUShort(&isOk)));
//        packet.append((char)(data->data()->toUShort(&isOk) >> 8));
    }
    return packet;
}

}   // namespace Private

QVector<quint8> OnBusDataUnit::values()
{
    QVector<quint8> vec;
    QByteArray arr = Private::collateDataset(m_dataset);
    QDataStream in(arr);
    in >> vec;
    return vec;
}

QByteArray OnBusDataUnit::values() const
{
    return Private::collateDataset(m_dataset);
}
