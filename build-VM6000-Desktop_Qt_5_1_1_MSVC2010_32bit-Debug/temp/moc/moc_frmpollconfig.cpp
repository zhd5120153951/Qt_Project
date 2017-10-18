/****************************************************************************
** Meta object code from reading C++ file 'frmpollconfig.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../VM6000/frmpollconfig.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frmpollconfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_frmPollConfig_t {
    QByteArrayData data[10];
    char stringdata[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_frmPollConfig_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_frmPollConfig_t qt_meta_stringdata_frmPollConfig = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 20),
QT_MOC_LITERAL(2, 35, 0),
QT_MOC_LITERAL(3, 36, 20),
QT_MOC_LITERAL(4, 57, 23),
QT_MOC_LITERAL(5, 81, 23),
QT_MOC_LITERAL(6, 105, 19),
QT_MOC_LITERAL(7, 125, 25),
QT_MOC_LITERAL(8, 151, 5),
QT_MOC_LITERAL(9, 157, 26)
    },
    "frmPollConfig\0on_btnAddOne_clicked\0\0"
    "on_btnAddAll_clicked\0on_btnRemoveOne_clicked\0"
    "on_btnRemoveAll_clicked\0on_btnExcel_clicked\0"
    "on_treeMain_doubleClicked\0index\0"
    "on_tableMain_doubleClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_frmPollConfig[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08,
       3,    0,   50,    2, 0x08,
       4,    0,   51,    2, 0x08,
       5,    0,   52,    2, 0x08,
       6,    0,   53,    2, 0x08,
       7,    1,   54,    2, 0x08,
       9,    1,   57,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    8,
    QMetaType::Void, QMetaType::QModelIndex,    8,

       0        // eod
};

void frmPollConfig::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        frmPollConfig *_t = static_cast<frmPollConfig *>(_o);
        switch (_id) {
        case 0: _t->on_btnAddOne_clicked(); break;
        case 1: _t->on_btnAddAll_clicked(); break;
        case 2: _t->on_btnRemoveOne_clicked(); break;
        case 3: _t->on_btnRemoveAll_clicked(); break;
        case 4: _t->on_btnExcel_clicked(); break;
        case 5: _t->on_treeMain_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: _t->on_tableMain_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject frmPollConfig::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_frmPollConfig.data,
      qt_meta_data_frmPollConfig,  qt_static_metacall, 0, 0}
};


const QMetaObject *frmPollConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *frmPollConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_frmPollConfig.stringdata))
        return static_cast<void*>(const_cast< frmPollConfig*>(this));
    return QDialog::qt_metacast(_clname);
}

int frmPollConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
