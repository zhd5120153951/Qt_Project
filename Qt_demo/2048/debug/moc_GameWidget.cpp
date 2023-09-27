/****************************************************************************
** Meta object code from reading C++ file 'GameWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../GameWidget.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GameWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGameWidgetENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSGameWidgetENDCLASS = QtMocHelpers::stringData(
    "GameWidget",
    "GestureMove",
    "",
    "GestureDirect",
    "ScoreInc",
    "GameOver",
    "win",
    "onGestureMove",
    "restart"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSGameWidgetENDCLASS_t {
    uint offsetsAndSizes[18];
    char stringdata0[11];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[9];
    char stringdata5[9];
    char stringdata6[4];
    char stringdata7[14];
    char stringdata8[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSGameWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSGameWidgetENDCLASS_t qt_meta_stringdata_CLASSGameWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "GameWidget"
        QT_MOC_LITERAL(11, 11),  // "GestureMove"
        QT_MOC_LITERAL(23, 0),  // ""
        QT_MOC_LITERAL(24, 13),  // "GestureDirect"
        QT_MOC_LITERAL(38, 8),  // "ScoreInc"
        QT_MOC_LITERAL(47, 8),  // "GameOver"
        QT_MOC_LITERAL(56, 3),  // "win"
        QT_MOC_LITERAL(60, 13),  // "onGestureMove"
        QT_MOC_LITERAL(74, 7)   // "restart"
    },
    "GameWidget",
    "GestureMove",
    "",
    "GestureDirect",
    "ScoreInc",
    "GameOver",
    "win",
    "onGestureMove",
    "restart"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGameWidgetENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       4,    1,   53,    2, 0x06,    3 /* Public */,
       5,    0,   56,    2, 0x06,    5 /* Public */,
       6,    0,   57,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   58,    2, 0x0a,    7 /* Public */,
       8,    0,   61,    2, 0x0a,    9 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GameWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QGLWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSGameWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGameWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGameWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GameWidget, std::true_type>,
        // method 'GestureMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<GestureDirect, std::false_type>,
        // method 'ScoreInc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'GameOver'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'win'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onGestureMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<GestureDirect, std::false_type>,
        // method 'restart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GameWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GameWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->GestureMove((*reinterpret_cast< std::add_pointer_t<GestureDirect>>(_a[1]))); break;
        case 1: _t->ScoreInc((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->GameOver(); break;
        case 3: _t->win(); break;
        case 4: _t->onGestureMove((*reinterpret_cast< std::add_pointer_t<GestureDirect>>(_a[1]))); break;
        case 5: _t->restart(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GameWidget::*)(GestureDirect );
            if (_t _q_method = &GameWidget::GestureMove; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GameWidget::*)(int );
            if (_t _q_method = &GameWidget::ScoreInc; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GameWidget::*)();
            if (_t _q_method = &GameWidget::GameOver; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GameWidget::*)();
            if (_t _q_method = &GameWidget::win; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *GameWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGameWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QGLWidget::qt_metacast(_clname);
}

int GameWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GameWidget::GestureMove(GestureDirect _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GameWidget::ScoreInc(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GameWidget::GameOver()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void GameWidget::win()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
