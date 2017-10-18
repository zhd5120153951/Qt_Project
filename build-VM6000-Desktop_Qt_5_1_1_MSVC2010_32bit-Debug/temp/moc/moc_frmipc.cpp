/****************************************************************************
** Meta object code from reading C++ file 'frmipc.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../VM6000/frmipc.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frmipc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_frmIPC_t {
    QByteArrayData data[12];
    char stringdata[201];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_frmIPC_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_frmIPC_t qt_meta_stringdata_frmIPC = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 17),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 20),
QT_MOC_LITERAL(4, 47, 20),
QT_MOC_LITERAL(5, 68, 19),
QT_MOC_LITERAL(6, 88, 20),
QT_MOC_LITERAL(7, 109, 5),
QT_MOC_LITERAL(8, 115, 22),
QT_MOC_LITERAL(9, 138, 4),
QT_MOC_LITERAL(10, 143, 33),
QT_MOC_LITERAL(11, 177, 22)
    },
    "frmIPC\0on_btnAdd_clicked\0\0"
    "on_btnDelete_clicked\0on_btnUpdate_clicked\0"
    "on_btnExcel_clicked\0on_tableMain_pressed\0"
    "index\0on_cboxNVRID_activated\0arg1\0"
    "on_txtIPCRtspAddrMain_textChanged\0"
    "on_cboxIPCID_activated\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_frmIPC[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08,
       3,    0,   55,    2, 0x08,
       4,    0,   56,    2, 0x08,
       5,    0,   57,    2, 0x08,
       6,    1,   58,    2, 0x08,
       8,    1,   61,    2, 0x08,
      10,    1,   64,    2, 0x08,
      11,    1,   67,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void frmIPC::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        frmIPC *_t = static_cast<frmIPC *>(_o);
        switch (_id) {
        case 0: _t->on_btnAdd_clicked(); break;
        case 1: _t->on_btnDelete_clicked(); break;
        case 2: _t->on_btnUpdate_clicked(); break;
        case 3: _t->on_btnExcel_clicked(); break;
        case 4: _t->on_tableMain_pressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: _t->on_cboxNVRID_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->on_txtIPCRtspAddrMain_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->on_cboxIPCID_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject frmIPC::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_frmIPC.data,
      qt_meta_data_frmIPC,  qt_static_metacall, 0, 0}
};


const QMetaObject *frmIPC::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *frmIPC::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_frmIPC.stringdata))
        return static_cast<void*>(const_cast< frmIPC*>(this));
    return QDialog::qt_metacast(_clname);
}

int frmIPC::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
