/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../project/src/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[18];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "showSettingsRTU"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 14), // "openSerialPort"
QT_MOC_LITERAL(4, 43, 14), // "onStateChanged"
QT_MOC_LITERAL(5, 58, 5), // "state"
QT_MOC_LITERAL(6, 64, 11), // "transaction"
QT_MOC_LITERAL(7, 76, 12), // "showResponse"
QT_MOC_LITERAL(8, 89, 1), // "s"
QT_MOC_LITERAL(9, 91, 11), // "showRequest"
QT_MOC_LITERAL(10, 103, 4), // "data"
QT_MOC_LITERAL(11, 108, 12), // "processError"
QT_MOC_LITERAL(12, 121, 14), // "processTimeout"
QT_MOC_LITERAL(13, 136, 5), // "State"
QT_MOC_LITERAL(14, 142, 16), // "UnconnectedState"
QT_MOC_LITERAL(15, 159, 15), // "ConnectingState"
QT_MOC_LITERAL(16, 175, 14), // "ConnectedState"
QT_MOC_LITERAL(17, 190, 12) // "ClosingState"

    },
    "MainWindow\0showSettingsRTU\0\0openSerialPort\0"
    "onStateChanged\0state\0transaction\0"
    "showResponse\0s\0showRequest\0data\0"
    "processError\0processTimeout\0State\0"
    "UnconnectedState\0ConnectingState\0"
    "ConnectedState\0ClosingState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       1,   72, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    1,   56,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    1,   60,    2, 0x08 /* Private */,
       9,    1,   63,    2, 0x08 /* Private */,
      11,    1,   66,    2, 0x08 /* Private */,
      12,    1,   69,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QByteArray,   10,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,

 // enums: name, flags, count, data
      13, 0x0,    4,   76,

 // enum data: key, value
      14, uint(MainWindow::UnconnectedState),
      15, uint(MainWindow::ConnectingState),
      16, uint(MainWindow::ConnectedState),
      17, uint(MainWindow::ClosingState),

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showSettingsRTU(); break;
        case 1: _t->openSerialPort(); break;
        case 2: _t->onStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->transaction(); break;
        case 4: _t->showResponse((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->showRequest((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 6: _t->processError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->processTimeout((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
