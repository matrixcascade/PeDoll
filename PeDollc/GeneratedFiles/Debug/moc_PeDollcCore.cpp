/****************************************************************************
** Meta object code from reading C++ file 'PeDollcCore.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../PeDollcCore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PeDollcCore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PeDollcCore[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   13,   12,   12, 0x05,
      57,   13,   12,   12, 0x05,
     100,   13,   12,   12, 0x05,
     140,   13,   12,   12, 0x05,
     184,   13,   12,   12, 0x05,
     234,   13,   12,   12, 0x05,
     276,   12,   12,   12, 0x05,
     305,   12,   12,   12, 0x05,
     338,   12,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_PeDollcCore[] = {
    "PeDollcCore\0\0,\0SIGNAL_EmitMonitorMessage(QString,QColor)\0"
    "SIGNAL_EmitRegistryMessage(QString,QColor)\0"
    "SIGNAL_EmitFilesMessage(QString,QColor)\0"
    "SIGNAL_EmitProcessesMessage(QString,QColor)\0"
    "SIGNAL_EmitWindowsOperatorMessage(QString,QColor)\0"
    "SIGNAL_EmitNetworkMessage(QString,QColor)\0"
    "SIGNAL_EmitDumpTableUpdate()\0"
    "SIGNAL_ConnectionStatisChanged()\0"
    "SIGNAL_onQuery()\0"
};

void PeDollcCore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PeDollcCore *_t = static_cast<PeDollcCore *>(_o);
        switch (_id) {
        case 0: _t->SIGNAL_EmitMonitorMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 1: _t->SIGNAL_EmitRegistryMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 2: _t->SIGNAL_EmitFilesMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 3: _t->SIGNAL_EmitProcessesMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 4: _t->SIGNAL_EmitWindowsOperatorMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 5: _t->SIGNAL_EmitNetworkMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 6: _t->SIGNAL_EmitDumpTableUpdate(); break;
        case 7: _t->SIGNAL_ConnectionStatisChanged(); break;
        case 8: _t->SIGNAL_onQuery(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PeDollcCore::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PeDollcCore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PeDollcCore,
      qt_meta_data_PeDollcCore, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PeDollcCore::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PeDollcCore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PeDollcCore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PeDollcCore))
        return static_cast<void*>(const_cast< PeDollcCore*>(this));
    return QObject::qt_metacast(_clname);
}

int PeDollcCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void PeDollcCore::SIGNAL_EmitMonitorMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PeDollcCore::SIGNAL_EmitRegistryMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PeDollcCore::SIGNAL_EmitFilesMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PeDollcCore::SIGNAL_EmitProcessesMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PeDollcCore::SIGNAL_EmitWindowsOperatorMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PeDollcCore::SIGNAL_EmitNetworkMessage(QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PeDollcCore::SIGNAL_EmitDumpTableUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void PeDollcCore::SIGNAL_ConnectionStatisChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void PeDollcCore::SIGNAL_onQuery()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}
QT_END_MOC_NAMESPACE
