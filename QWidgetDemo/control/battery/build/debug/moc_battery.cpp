/****************************************************************************
** Meta object code from reading C++ file 'battery.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../battery.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'battery.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSBatteryENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSBatteryENDCLASS = QtMocHelpers::stringData(
    "Battery",
    "valueChanged",
    "",
    "value",
    "updateValue",
    "setValue",
    "setAlarmValue",
    "alarmValue",
    "minValue",
    "maxValue",
    "animation",
    "animationStep",
    "borderWidth",
    "borderRadius",
    "bgRadius",
    "headRadius",
    "borderColorStart",
    "borderColorEnd",
    "alarmColorStart",
    "alarmColorEnd",
    "normalColorStart",
    "normalColorEnd"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBatteryENDCLASS_t {
    uint offsetsAndSizes[44];
    char stringdata0[8];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[12];
    char stringdata5[9];
    char stringdata6[14];
    char stringdata7[11];
    char stringdata8[9];
    char stringdata9[9];
    char stringdata10[10];
    char stringdata11[14];
    char stringdata12[12];
    char stringdata13[13];
    char stringdata14[9];
    char stringdata15[11];
    char stringdata16[17];
    char stringdata17[15];
    char stringdata18[16];
    char stringdata19[14];
    char stringdata20[17];
    char stringdata21[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBatteryENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSBatteryENDCLASS_t qt_meta_stringdata_CLASSBatteryENDCLASS = {
    {
        QT_MOC_LITERAL(0, 7),  // "Battery"
        QT_MOC_LITERAL(8, 12),  // "valueChanged"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 5),  // "value"
        QT_MOC_LITERAL(28, 11),  // "updateValue"
        QT_MOC_LITERAL(40, 8),  // "setValue"
        QT_MOC_LITERAL(49, 13),  // "setAlarmValue"
        QT_MOC_LITERAL(63, 10),  // "alarmValue"
        QT_MOC_LITERAL(74, 8),  // "minValue"
        QT_MOC_LITERAL(83, 8),  // "maxValue"
        QT_MOC_LITERAL(92, 9),  // "animation"
        QT_MOC_LITERAL(102, 13),  // "animationStep"
        QT_MOC_LITERAL(116, 11),  // "borderWidth"
        QT_MOC_LITERAL(128, 12),  // "borderRadius"
        QT_MOC_LITERAL(141, 8),  // "bgRadius"
        QT_MOC_LITERAL(150, 10),  // "headRadius"
        QT_MOC_LITERAL(161, 16),  // "borderColorStart"
        QT_MOC_LITERAL(178, 14),  // "borderColorEnd"
        QT_MOC_LITERAL(193, 15),  // "alarmColorStart"
        QT_MOC_LITERAL(209, 13),  // "alarmColorEnd"
        QT_MOC_LITERAL(223, 16),  // "normalColorStart"
        QT_MOC_LITERAL(240, 14)   // "normalColorEnd"
    },
    "Battery",
    "valueChanged",
    "",
    "value",
    "updateValue",
    "setValue",
    "setAlarmValue",
    "alarmValue",
    "minValue",
    "maxValue",
    "animation",
    "animationStep",
    "borderWidth",
    "borderRadius",
    "bgRadius",
    "headRadius",
    "borderColorStart",
    "borderColorEnd",
    "alarmColorStart",
    "alarmColorEnd",
    "normalColorStart",
    "normalColorEnd"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBatteryENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
      16,   48, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,   17 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   41,    2, 0x08,   19 /* Private */,
       5,    1,   42,    2, 0x0a,   20 /* Public */,
       6,    1,   45,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    7,

 // properties: name, type, flags
       8, QMetaType::Double, 0x00015103, uint(-1), 0,
       9, QMetaType::Double, 0x00015103, uint(-1), 0,
       3, QMetaType::Double, 0x00015103, uint(-1), 0,
       7, QMetaType::Double, 0x00015103, uint(-1), 0,
      10, QMetaType::Bool, 0x00015103, uint(-1), 0,
      11, QMetaType::Double, 0x00015103, uint(-1), 0,
      12, QMetaType::Int, 0x00015103, uint(-1), 0,
      13, QMetaType::Int, 0x00015103, uint(-1), 0,
      14, QMetaType::Int, 0x00015103, uint(-1), 0,
      15, QMetaType::Int, 0x00015103, uint(-1), 0,
      16, QMetaType::QColor, 0x00015103, uint(-1), 0,
      17, QMetaType::QColor, 0x00015103, uint(-1), 0,
      18, QMetaType::QColor, 0x00015103, uint(-1), 0,
      19, QMetaType::QColor, 0x00015103, uint(-1), 0,
      20, QMetaType::QColor, 0x00015103, uint(-1), 0,
      21, QMetaType::QColor, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Battery::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSBatteryENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBatteryENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBatteryENDCLASS_t,
        // property 'minValue'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'maxValue'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'value'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'alarmValue'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'animation'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'animationStep'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'borderWidth'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'borderRadius'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'bgRadius'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'headRadius'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'borderColorStart'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'borderColorEnd'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'alarmColorStart'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'alarmColorEnd'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'normalColorStart'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // property 'normalColorEnd'
        QtPrivate::TypeAndForceComplete<QColor, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Battery, std::true_type>,
        // method 'valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'updateValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setAlarmValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void Battery::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Battery *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->updateValue(); break;
        case 2: _t->setValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->setAlarmValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Battery::*)(double );
            if (_t _q_method = &Battery::valueChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Battery *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->getMinValue(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getMaxValue(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->getValue(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->getAlarmValue(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->getAnimation(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->getAnimationStep(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->getBorderWidth(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->getBorderRadius(); break;
        case 8: *reinterpret_cast< int*>(_v) = _t->getBgRadius(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->getHeadRadius(); break;
        case 10: *reinterpret_cast< QColor*>(_v) = _t->getBorderColorStart(); break;
        case 11: *reinterpret_cast< QColor*>(_v) = _t->getBorderColorEnd(); break;
        case 12: *reinterpret_cast< QColor*>(_v) = _t->getAlarmColorStart(); break;
        case 13: *reinterpret_cast< QColor*>(_v) = _t->getAlarmColorEnd(); break;
        case 14: *reinterpret_cast< QColor*>(_v) = _t->getNormalColorStart(); break;
        case 15: *reinterpret_cast< QColor*>(_v) = _t->getNormalColorEnd(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Battery *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setValue(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setAlarmValue(*reinterpret_cast< double*>(_v)); break;
        case 4: _t->setAnimation(*reinterpret_cast< bool*>(_v)); break;
        case 5: _t->setAnimationStep(*reinterpret_cast< double*>(_v)); break;
        case 6: _t->setBorderWidth(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setBorderRadius(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setBgRadius(*reinterpret_cast< int*>(_v)); break;
        case 9: _t->setHeadRadius(*reinterpret_cast< int*>(_v)); break;
        case 10: _t->setBorderColorStart(*reinterpret_cast< QColor*>(_v)); break;
        case 11: _t->setBorderColorEnd(*reinterpret_cast< QColor*>(_v)); break;
        case 12: _t->setAlarmColorStart(*reinterpret_cast< QColor*>(_v)); break;
        case 13: _t->setAlarmColorEnd(*reinterpret_cast< QColor*>(_v)); break;
        case 14: _t->setNormalColorStart(*reinterpret_cast< QColor*>(_v)); break;
        case 15: _t->setNormalColorEnd(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Battery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Battery::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBatteryENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Battery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void Battery::valueChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
