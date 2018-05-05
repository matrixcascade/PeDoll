#ifndef PEDOLLC_H
#define PEDOLLC_H

#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QFile>
#include "ui_pedollc.h"
#include "ui_Connector.h"
#include "PeDollcCore.h"
#include "ui_Register.h"
#include "ui_TransferTool.h"

class PeDollc_TransferDialog:public QDialog
{
	Q_OBJECT
public:
	PeDollc_TransferDialog(QWidget *parent):QDialog(parent)
	{
		ui.setupUi(this);
		this->setModal(true);
		connect(ui.pushButton_GTD,SIGNAL(clicked()),this,SLOT(SLOT_OnGBKtoDex()));
		connect(ui.pushButton_HTD,SIGNAL(clicked()),this,SLOT(SLOT_OnHextoDex()));
	}
	Ui::Form_TransFerTool ui;
public slots:
	void SLOT_OnGBKtoDex();
	void SLOT_OnHextoDex();
};

class PeDollc_ConnectorDialog:public QDialog
{
	Q_OBJECT
public:
	PeDollc_ConnectorDialog(QWidget *parent):QDialog(parent)
	{
		ui.setupUi(this);
		this->setModal(true);
		connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(SLOT_OnConfirm()));
	}
	Ui::Form_Connector ui;
public slots:
	void SLOT_OnConfirm();
signals:

	void SIGANL_ConfirmHost(QString);
};



class PeDollc_RegisterDialog:public QDialog
{
	Q_OBJECT
public:
	PeDollc_RegisterDialog(QWidget *parent):QDialog(parent)
	{
		ui.setupUi(this);
		this->setModal(true);
		connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(SLOT_OnConfirm()));
		connect(ui.pushButton_Buy,SIGNAL(clicked()),this,SLOT(SLOT_onBuy()));
	}
	Ui::Form_Register ui;
	public slots:
		void SLOT_onBuy();
		void SLOT_OnConfirm();
signals:

		void SIGANL_ConfirmHost(QString);
};

class PeDollc : public QMainWindow
{
	Q_OBJECT

public:
	PeDollc(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PeDollc();

	void OUTPUT_Monitor_Message_Disply(QColor Color,QString str);
	void OUTPUT_Registry_Message_Disply(QColor Color,QString str);
	void OUTPUT_Files_Message_Disply(QColor Color,QString str);
	void OUTPUT_WindowsOperator_Message_Disply(QColor Color,QString str);
	void OUTPUT_Processes_Message_Disply(QColor Color,QString str);
	void OUTPUT_Network_Disply(QColor Color,QString str);
	void UpdateConnectUI();
	void UpdateDumpInfo();
	void DisplayHexDump(pt_byte *dump,int Size);
	void DisplayANSIDump(pt_byte *dump,int Size);
	void DisplayHexStack(pt_byte *dump,int Size);
	void DisplayHexMemory();
	void DisplayANSIMemory();
	void DisplayANSIStack(pt_byte *dump,int Size);

public slots:
	void SLOT_onAntiVirtualization();
	void SLOT_onSwitchOld();
	void SLOT_onDumpClear();
	void SLOT_onDumpDelete();
	void SLOT_onDumpSave();
	void SLOT_onMemDumpSave();
	void SLOT_onDumpSaveAll();
	void SLOT_About();
	void SLOT_onClear();
	void SLOT_DumpConfigureChanged();
	void SLOT_LoadHookScript();
	void SLOT_onHookScript();
	void SLOT_onRun();
	void SLOT_onTerminate();
	void SLOT_onConnectionStatusChanged();
	void SLOT_ShowConnectorDialog();
	void SLOT_onSetupHost(QString);
	void SLOT_onControllerConnect();
	void SLOT_onControllerClose();
	void SLOT_onDollConnect();
	void SLOT_onMonitorMessageDiaplay(QString,QColor);
	void SLOT_onRegistryMessageDiaplay(QString,QColor);
	void SLOT_onFilesMessageDiaplay(QString,QColor);
	void SLOT_onWindowsOperatorMessageDiaplay(QString,QColor);
	void SLOT_onProcessesDiaplay(QString,QColor);
	void SLOT_onNetworkDiaplay(QString,QColor);
	void SLOT_onQuery();
	void SLOT_onClearHookTable();
	void SLOT_onClearFilterTable();
	void SLOT_onClearQuery();
	void SLOT_PassCurrentQuery();
	void SLOT_RejectCurrentQuery();
	void SLOT_TerminateCurrentQuery();
	void SLOT_onExectueCommand(QString);
	void SLOT_onCommandLineEnter();
	void SLOT_UpdateDumpPack();
	void SLOT_itemDoubleClicked ( QListWidgetItem * item );
	void SLOT_RadioClicked();
	void SLOT_UpdateStack();
	void SLOT_UpdateMemory();
	void SLOT_UpdateSearchStack();
	void SLOT_onStackClear();
	void SLOT_UpdateStackUI();
	void SLOT_UpdateMemoryUI();
	void SLOT_onToolDexTransfer();
signals:
	void SIGNAL_NextQuery();
private:
	vector<QString>			  m_OldInstr;
	unsigned int			  m_TabIndex;		 
	int						  m_Instr_hook,m_Instr_unhook,m_Instr_binary,m_Instr_FilterStart;
	QString					  m_Host;
	bool					  m_ControllerConnected,m_DollConnected;
	PeDollcCore				  m_Core;
	PeDollc_ConnectorDialog  *m_ConnectorDialog;
	PeDollc_RegisterDialog   *m_RegisterDialog;
	PeDollc_TransferDialog   *m_TransferDialog;
	CubeGrammar				  m_Grammar;
	CubeLexer				  m_Lexer;
	char					 *m_DumpBuffer;
	int						  m_DumpSize;
	Ui::PeDollcClass ui;
};

#endif // PEDOLLC_H
