/****************************************************************************
** Meta object code from reading C++ file 'frmmain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../VM6000/frmmain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frmmain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_frmMain_t {
    QByteArrayData data[17];
    char stringdata[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_frmMain_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_frmMain_t qt_meta_stringdata_frmMain = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 12),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 11),
QT_MOC_LITERAL(4, 34, 13),
QT_MOC_LITERAL(5, 48, 16),
QT_MOC_LITERAL(6, 65, 16),
QT_MOC_LITERAL(7, 82, 18),
QT_MOC_LITERAL(8, 101, 18),
QT_MOC_LITERAL(9, 120, 12),
QT_MOC_LITERAL(10, 133, 12),
QT_MOC_LITERAL(11, 146, 12),
QT_MOC_LITERAL(12, 159, 13),
QT_MOC_LITERAL(13, 173, 24),
QT_MOC_LITERAL(14, 198, 22),
QT_MOC_LITERAL(15, 221, 25),
QT_MOC_LITERAL(16, 247, 5)
    },
    "frmMain\0change_style\0\0screen_full\0"
    "screen_normal\0delete_video_one\0"
    "delete_video_all\0snapshot_video_one\0"
    "snapshot_video_all\0show_video_1\0"
    "show_video_4\0show_video_9\0show_video_16\0"
    "on_btnMenu_Close_clicked\0"
    "on_btnMenu_Min_clicked\0on_treeMain_doubleClicked\0"
    "index\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_frmMain[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08,
       3,    0,   85,    2, 0x08,
       4,    0,   86,    2, 0x08,
       5,    0,   87,    2, 0x08,
       6,    0,   88,    2, 0x08,
       7,    0,   89,    2, 0x08,
       8,    0,   90,    2, 0x08,
       9,    0,   91,    2, 0x08,
      10,    0,   92,    2, 0x08,
      11,    0,   93,    2, 0x08,
      12,    0,   94,    2, 0x08,
      13,    0,   95,    2, 0x08,
      14,    0,   96,    2, 0x08,
      15,    1,   97,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,   16,

       0        // eod
};

void frmMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        frmMain *_t = static_cast<frmMain *>(_o);
        switch (_id) {
        case 0: _t->change_style(); break;
        case 1: _t->screen_full(); break;
        case 2: _t->screen_normal(); break;
        case 3: _t->delete_video_one(); break;
        case 4: _t->delete_video_all(); break;
        case 5: _t->snapshot_video_one(); break;
        case 6: _t->snapshot_video_all(); break;
        case 7: _t->show_video_1(); break;
        case 8: _t->show_video_4(); break;
        case 9: _t->show_video_9(); break;
        case 10: _t->show_video_16(); break;
        case 11: _t->on_btnMenu_Close_clicked(); break;
        case 12: _t->on_btnMenu_Min_clicked(); break;
        case 13: _t->on_treeMain_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject frmMain::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_frmMain.data,
      qt_meta_data_frmMain,  qt_static_metacall, 0, 0}
};


const QMetaObject *frmMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *frmMain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_frmMain.stringdata))
        return static_cast<void*>(const_cast< frmMain*>(this));
    return QDialog::qt_metacast(_clname);
}

int frmMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
