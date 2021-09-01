#ifndef MODBUSDATAENTRIES_H
#define MODBUSDATAENTRIES_H

#include <QObject>

class EData;
class EBusData;
class OnBusMaster;

class ModbusDataEntries : public QObject
{
    Q_OBJECT
public:
    enum Address {
        ADR_SOP = 0,
        ADR_FNC = 1,
        ADR_LEN = 2,
    };
    Q_ENUM(Address)

    enum Function {
        FC_WRITE_MEM,
        FC_WATCH_CONF,
        FC_WATCH_VARS,
        FC_CONNECT
    };

    explicit ModbusDataEntries(OnBusMaster *master, QObject *parent = nullptr);
    explicit ModbusDataEntries(QObject *parent = nullptr) : ModbusDataEntries(nullptr, parent) {}

    EBusData *addEntry(int registerId, int address, EData *data, int precision = 0, const QString &alias = QString());

    EBusData * entry(int address);
    bool hasEntry(int address);
    void deleteEntry(int address);

    void deleteAll();

    typedef QList<EBusData *> EntryList;
    EntryList allEntries();
    QList<EntryList *> collectSubEntries();
    QList<EntryList *> collectSubEntries(const EntryList &entries);

    int size();
    int count();

    static QByteArray prepareRequest(const EntryList &entryList);
    void prepareRequest();
    QByteArray requestPacket() const;
    void openCloseRequest();


public slots:
    void sendRequest();
    void sendRequest(const QByteArray &request);


private:
    OnBusMaster *m_onbusMaster;
    QByteArray m_request;

private slots:
    void response(const QByteArray &rawData);

signals:
    void sgMessage(const QString &caption, const QString &text);
    void sgProcessRange(int min, int max);
    void sgProcessValue(int value);
};

#endif // MODBUSDATAENTRIES_H
