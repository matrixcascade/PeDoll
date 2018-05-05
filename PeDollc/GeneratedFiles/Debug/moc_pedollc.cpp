/****************************************************************************
** Meta object code from reading C++ file 'pedollc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pedollc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pedollc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PeDollc_TransferDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,
      42,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PeDollc_TransferDialog[] = {
    "PeDollc_TransferDialog\0\0SLOT_OnGBKtoDex()\0"
    "SLOT_OnHextoDex()\0"
};

void PeDollc_TransferDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PeDollc_TransferDialog *_t = static_cast<PeDollc_TransferDialog *>(_o);
        switch (_id) {
        case 0: _t->SLOT_OnGBKtoDex(); break;
        case 1: _t->SLOT_OnHextoDex(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PeDollc_TransferDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PeDollc_TransferDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PeDollc_TransferDialog,
      qt_meta_data_PeDollc_TransferDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PeDollc_TransferDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PeDollc_TransferDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PeDollc_TransferDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PeDollc_TransferDialog))
        return static_cast<void*>(const_cast< PeDollc_TransferDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PeDollc_TransferDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_PeDollc_ConnectorDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   24,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PeDollc_ConnectorDialog[] = {
    "PeDollc_ConnectorDialog\0\0"
    "SIGANL_ConfirmHost(QString)\0"
    "SLOT_OnConfirm()\0"
};

void PeDollc_ConnectorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PeDollc_ConnectorDialog *_t = static_cast<PeDollc_ConnectorDialog *>(_o);
        switch (_id) {
        case 0: _t->SIGANL_ConfirmHost((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->SLOT_OnConfirm(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PeDollc_ConnectorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PeDollc_ConnectorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PeDollc_ConnectorDialog,
      qt_meta_data_PeDollc_ConnectorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PeDollc_ConnectorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PeDollc_ConnectorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PeDollc_ConnectorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PeDollc_ConnectorDialog))
        return static_cast<void*>(const_cast< PeDollc_ConnectorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PeDollc_ConnectorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PeDollc_ConnectorDialog::SIGANL_ConfirmHost(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_PeDollc_RegisterDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   23,   23,   23, 0x0a,
      65,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PeDollc_RegisterDialog[] = {
    "PeDollc_RegisterDialog\0\0"
    "SIGANL_ConfirmHost(QString)\0SLOT_onBuy()\0"
    "SLOT_OnConfirm()\0"
};

void PeDollc_RegisterDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PeDollc_RegisterDialog *_t = static_cast<PeDollc_RegisterDialog *>(_o);
        switch (_id) {
        case 0: _t->SIGANL_ConfirmHost((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->SLOT_onBuy(); break;
        case 2: _t->SLOT_OnConfirm(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PeDollc_RegisterDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PeDollc_RegisterDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PeDollc_RegisterDialog,
      qt_meta_data_PeDollc_RegisterDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PeDollc_RegisterDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PeDollc_RegisterDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PeDollc_RegisterDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PeDollc_RegisterDialog))
        return static_cast<void*>(const_cast< PeDollc_RegisterDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PeDollc_RegisterDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PeDollc_RegisterDialog::SIGANL_ConfirmHost(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_PeDollc[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,    8,    8,    8, 0x0a,
      56,    8,    8,    8, 0x0a,
      75,    8,    8,    8, 0x0a,
      94,    8,    8,    8, 0x0a,
     114,    8,    8,    8, 0x0a,
     132,    8,    8,    8, 0x0a,
     153,    8,    8,    8, 0x0a,
     174,    8,    8,    8, 0x0a,
     187,    8,    8,    8, 0x0a,
     202,    8,    8,    8, 0x0a,
     230,    8,    8,    8, 0x0a,
     252,    8,    8,    8, 0x0a,
     272,    8,    8,    8, 0x0a,
     285,    8,    8,    8, 0x0a,
     304,    8,    8,    8, 0x0a,
     337,    8,    8,    8, 0x0a,
     364,    8,    8,    8, 0x0a,
     390,    8,    8,    8, 0x0a,
     417,    8,    8,    8, 0x0a,
     442,    8,    8,    8, 0x0a,
     465,  463,    8,    8, 0x0a,
     510,  463,    8,    8, 0x0a,
     556,  463,    8,    8, 0x0a,
     599,  463,    8,    8, 0x0a,
     652,  463,    8,    8, 0x0a,
     692,  463,    8,    8, 0x0a,
     730,    8,    8,    8, 0x0a,
     745,    8,    8,    8, 0x0a,
     769,    8,    8,    8, 0x0a,
     795,    8,    8,    8, 0x0a,
     815,    8,    8,    8, 0x0a,
     839,    8,    8,    8, 0x0a,
     865,    8,    8,    8, 0x0a,
     894,    8,    8,    8, 0x0a,
     925,    8,    8,    8, 0x0a,
     951,    8,    8,    8, 0x0a,
     978,  973,    8,    8, 0x0a,
    1019,    8,    8,    8, 0x0a,
    1039,    8,    8,    8, 0x0a,
    1058,    8,    8,    8, 0x0a,
    1078,    8,    8,    8, 0x0a,
    1103,    8,    8,    8, 0x0a,
    1123,    8,    8,    8, 0x0a,
    1144,    8,    8,    8, 0x0a,
    1166,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PeDollc[] = {
    "PeDollc\0\0SIGNAL_NextQuery()\0"
    "SLOT_onAntiVirtualization()\0"
    "SLOT_onSwitchOld()\0SLOT_onDumpClear()\0"
    "SLOT_onDumpDelete()\0SLOT_onDumpSave()\0"
    "SLOT_onMemDumpSave()\0SLOT_onDumpSaveAll()\0"
    "SLOT_About()\0SLOT_onClear()\0"
    "SLOT_DumpConfigureChanged()\0"
    "SLOT_LoadHookScript()\0SLOT_onHookScript()\0"
    "SLOT_onRun()\0SLOT_onTerminate()\0"
    "SLOT_onConnectionStatusChanged()\0"
    "SLOT_ShowConnectorDialog()\0"
    "SLOT_onSetupHost(QString)\0"
    "SLOT_onControllerConnect()\0"
    "SLOT_onControllerClose()\0SLOT_onDollConnect()\0"
    ",\0SLOT_onMonitorMessageDiaplay(QString,QColor)\0"
    "SLOT_onRegistryMessageDiaplay(QString,QColor)\0"
    "SLOT_onFilesMessageDiaplay(QString,QColor)\0"
    "SLOT_onWindowsOperatorMessageDiaplay(QString,QColor)\0"
    "SLOT_onProcessesDiaplay(QString,QColor)\0"
    "SLOT_onNetworkDiaplay(QString,QColor)\0"
    "SLOT_onQuery()\0SLOT_onClearHookTable()\0"
    "SLOT_onClearFilterTable()\0SLOT_onClearQuery()\0"
    "SLOT_PassCurrentQuery()\0"
    "SLOT_RejectCurrentQuery()\0"
    "SLOT_TerminateCurrentQuery()\0"
    "SLOT_onExectueCommand(QString)\0"
    "SLOT_onCommandLineEnter()\0"
    "SLOT_UpdateDumpPack()\0item\0"
    "SLOT_itemDoubleClicked(QListWidgetItem*)\0"
    "SLOT_RadioClicked()\0SLOT_UpdateStack()\0"
    "SLOT_UpdateMemory()\0SLOT_UpdateSearchStack()\0"
    "SLOT_onStackClear()\0SLOT_UpdateStackUI()\0"
    "SLOT_UpdateMemoryUI()\0SLOT_onToolDexTransfer()\0"
};

void PeDollc::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PeDollc *_t = static_cast<PeDollc *>(_o);
        switch (_id) {
        case 0: _t->SIGNAL_NextQuery(); break;
        case 1: _t->SLOT_onAntiVirtualization(); break;
        case 2: _t->SLOT_onSwitchOld(); break;
        case 3: _t->SLOT_onDumpClear(); break;
        case 4: _t->SLOT_onDumpDelete(); break;
        case 5: _t->SLOT_onDumpSave(); break;
        case 6: _t->SLOT_onMemDumpSave(); break;
        case 7: _t->SLOT_onDumpSaveAll(); break;
        case 8: _t->SLOT_About(); break;
        case 9: _t->SLOT_onClear(); break;
        case 10: _t->SLOT_DumpConfigureChanged(); break;
        case 11: _t->SLOT_LoadHookScript(); break;
        case 12: _t->SLOT_onHookScript(); break;
        case 13: _t->SLOT_onRun(); break;
        case 14: _t->SLOT_onTerminate(); break;
        case 15: _t->SLOT_onConnectionStatusChanged(); break;
        case 16: _t->SLOT_ShowConnectorDialog(); break;
        case 17: _t->SLOT_onSetupHost((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->SLOT_onControllerConnect(); break;
        case 19: _t->SLOT_onControllerClose(); break;
        case 20: _t->SLOT_onDollConnect(); break;
        case 21: _t->SLOT_onMonitorMessageDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 22: _t->SLOT_onRegistryMessageDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 23: _t->SLOT_onFilesMessageDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 24: _t->SLOT_onWindowsOperatorMessageDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 25: _t->SLOT_onProcessesDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 26: _t->SLOT_onNetworkDiaplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 27: _t->SLOT_onQuery(); break;
        case 28: _t->SLOT_onClearHookTable(); break;
        case 29: _t->SLOT_onClearFilterTable(); break;
        case 30: _t->SLOT_onClearQuery(); break;
        case 31: _t->SLOT_PassCurrentQuery(); break;
        case 32: _t->SLOT_RejectCurrentQuery(); break;
        case 33: _t->SLOT_TerminateCurrentQuery(); break;
        case 34: _t->SLOT_onExectueCommand((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 35: _t->SLOT_onCommandLineEnter(); break;
        case 36: _t->SLOT_UpdateDumpPack(); break;
        case 37: _t->SLOT_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 38: _t->SLOT_RadioClicked(); break;
        case 39: _t->SLOT_UpdateStack(); break;
        case 40: _t->SLOT_UpdateMemory(); break;
        case 41: _t->SLOT_UpdateSearchStack(); break;
        case 42: _t->SLOT_onStackClear(); break;
        case 43: _t->SLOT_UpdateStackUI(); break;
        case 44: _t->SLOT_UpdateMemoryUI(); break;
        case 45: _t->SLOT_onToolDexTransfer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PeDollc::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PeDollc::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PeDollc,
      qt_meta_data_PeDollc, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PeDollc::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PeDollc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PeDollc::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PeDollc))
        return static_cast<void*>(const_cast< PeDollc*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PeDollc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    }
    return _id;
}

// SIGNAL 0
void PeDollc::SIGNAL_NextQuery()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
