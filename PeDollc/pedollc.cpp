#include "pedollc.h"


static int hex_table[]={0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,1,2,3,4,5,6,  
	7,8,9,0,0,0,0,0,0,  
	0,10,11,12,13,14,15,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,10,  
	11,12,13,14,15};   
unsigned int hex_to_decimal(char* hex_str)   
{  
	char ch;   
	unsigned int iret=0;  
	while(ch=*hex_str++)   
	{   
		iret=(iret<<4)|hex_table[ch];  
	}   
	return iret;   
} 

void PeDollc_ConnectorDialog::SLOT_OnConfirm()
{
	emit SIGANL_ConfirmHost(ui.lineEdit->text());
	hide();
}


PeDollc::PeDollc(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	m_DumpBuffer=NULL;
	m_AboutDialog=new ProAbout();
	if(!m_Core.Initialize())
	{
		QMessageBox::information(NULL, tr("ERROR"), tr("Could not initialize network."), QMessageBox::Ok);
		exit(0);
	}
	ui.setupUi(this);

	UpdateConnectUI();
	UpdateDumpInfo();

	m_ControllerConnected=false;
	m_DollConnected=false;
	m_ConnectorDialog=new PeDollc_ConnectorDialog(this);
	m_RegisterDialog=new PeDollc_RegisterDialog(this);
	m_TransferDialog=new PeDollc_TransferDialog(this);

#define GRAMMAR_TOKEN_NUMBER 1
	m_Lexer.RegisterSpacer(' ');
	m_Lexer.RegisterSpacer('\r');
	m_Lexer.RegisterSpacer('\n');
	unsigned int ContainerIndex=m_Lexer.RegisterContainer('<','>');

	m_Grammar.SetLexer(&m_Lexer);

	CubeBlockType Contain=CubeBlockType(NULL,2,GRAMMAR_CONATINER,ContainerIndex);
	CubeBlockType Param=CubeBlockType(NULL,1,GRAMMAR_TYPE_ANY);
	CubeBlockType Spacer=CubeBlockType(NULL,1,GRAMMAR_SPACER);
	CubeBlockType Number=CubeBlockType("[0-9]+",2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_NUMBER);
	Number.AsRegex();

	m_Grammar.RegisterBlockType(Contain);
	m_Grammar.RegisterBlockType(Number);
	m_Grammar.RegisterBlockType(Param);
	m_Grammar.RegisterDiscard(Spacer);

	CubeBlockType cbt_hook=CubeBlockType(PEDOLL_DOOL_CMD_HOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_HOOK);
	m_Grammar.RegisterBlockType(cbt_hook);

	CubeBlockType cbt_unhook=CubeBlockType(PEDOLL_DOOL_CMD_UNHOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_UNHOOK);
	m_Grammar.RegisterBlockType(cbt_unhook);

	CubeBlockType cbt_binary=CubeBlockType(PEDOLL_DOLL_CMD_HOOK_BIN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_BINARY);
	m_Grammar.RegisterBlockType(cbt_binary);

	CubeBlockType cbt_filter_Start=CubeBlockType(PEDOLL_PARSER_FILTER_START_STR,2,GRAMMAR_TOKEN,PEDOLL_PARSER_FILTER_START);
	m_Grammar.RegisterBlockType(cbt_filter_Start);

	CubeGrammarSentence Sen;
	Sen.Reset();
	Sen.add(cbt_hook);
	Sen.add(Param);
	m_Instr_hook=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_unhook);
	Sen.add(Param);
	
	m_Instr_unhook=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_binary);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	m_Instr_binary=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_filter_Start);
	m_Instr_FilterStart=m_Grammar.RegisterSentence(Sen);
	connect(this->ui.spinBox_DumpMaxCount,SIGNAL(valueChanged(int)),this,SLOT(SLOT_DumpConfigureChanged()));
	connect(this->ui.spinBox_DumpMaxSize,SIGNAL(valueChanged(int)),this,SLOT(SLOT_DumpConfigureChanged()));

	connect(this->ui.actionLoad_menu_hookregulars,SIGNAL(triggered()),this,SLOT(SLOT_LoadHookScript()));
	connect(this->ui.action_Menu_MonitorConnector,SIGNAL(triggered()),this,SLOT(SLOT_ShowConnectorDialog()));
	connect(this->ui.action_Menu_ClearHook,SIGNAL(triggered()),this,SLOT(SLOT_onClearHookTable()));
	connect(this->ui.action_Menu_ClearFilter,SIGNAL(triggered()),this,SLOT(SLOT_onClearFilterTable()));
	connect(this->ui.action_menu_regist,SIGNAL(triggered()),m_RegisterDialog,SLOT(show()));
	connect(this->ui.actionHexToDex,SIGNAL(triggered()),m_TransferDialog,SLOT(show()));
	connect(this->ui.action_PeDoll,SIGNAL(triggered()),this,SLOT(SLOT_About()));
	connect(this->ui.action_tool_Clear,SIGNAL(triggered()),this,SLOT(SLOT_onClear()));
	connect(this->ui.listWidget_Dump,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(SLOT_itemDoubleClicked(QListWidgetItem*)));
	connect(this->ui.action_Menu_VMWare_vmx_Anti_virtualization,SIGNAL(triggered()),this,SLOT(SLOT_onAntiVirtualization()));
	
	connect(this->ui.radioButton_GBK,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));
	connect(this->ui.radioButton_UTF8,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));
	connect(this->ui.radioButton_UTF16,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));
	connect(this->ui.radioButton_ASM64,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));
	connect(this->ui.radioButton_ASM86,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));
	connect(this->ui.radioButton_ASM16,SIGNAL(clicked()),this,SLOT(SLOT_RadioClicked()));

	connect(this->ui.radioButton_Mem_GBK,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));
	connect(this->ui.radioButton_Mem_UTF8,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));
	connect(this->ui.radioButton_Mem_UTF16,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));
	connect(this->ui.radioButton_Mem_ASM64,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));
	connect(this->ui.radioButton_Mem_ASM86,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));
	connect(this->ui.radioButton_Mem_ASM16,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemoryUI()));


	connect(this->ui.action_Tool_ControllerConnect,SIGNAL(triggered()),this,SLOT(SLOT_onControllerConnect()));
	connect(this->ui.actionControllerClose,SIGNAL(triggered()),this,SLOT(SLOT_onControllerClose()));
	connect(this->ui.action_Tool_DollConnect,SIGNAL(triggered()),this,SLOT(SLOT_onDollConnect()));
	connect(this->ui.action_Tool_Hook,SIGNAL(triggered()),this,SLOT(SLOT_onHookScript()));
	connect(this->ui.action_Tool_Run,SIGNAL(triggered()),this,SLOT(SLOT_onRun()));
	connect(this->ui.action_Tool_Stop,SIGNAL(triggered()),this,SLOT(SLOT_onTerminate()));

	connect(this->ui.action_tool_Tab,SIGNAL(triggered()),this,SLOT(SLOT_onSwitchOld()));
	connect(this->ui.pushButton_Switch,SIGNAL(clicked()),this,SLOT(SLOT_onSwitchOld()));
	connect(this->ui.pushButton_Enter,SIGNAL(clicked()),this,SLOT(SLOT_onCommandLineEnter()));

	connect(this->ui.pushButton_QueryPass,SIGNAL(clicked()),this,SLOT(SLOT_PassCurrentQuery()));
	connect(this->ui.pushButton_QueryReject,SIGNAL(clicked()),this,SLOT(SLOT_RejectCurrentQuery()));
	connect(this->ui.pushButton_QueryTerminate,SIGNAL(clicked()),this,SLOT(SLOT_TerminateCurrentQuery()));

	connect(this->ui.pushButton_DumpClear,SIGNAL(clicked()),this,SLOT(SLOT_onDumpClear()));
	connect(this->ui.pushButton_DumpDelete,SIGNAL(clicked()),this,SLOT(SLOT_onDumpDelete()));
	connect(this->ui.pushButton_DumpSave,SIGNAL(clicked()),this,SLOT(SLOT_onDumpSave()));
	connect(this->ui.pushButton_DumpSaveAll,SIGNAL(clicked()),this,SLOT(SLOT_onDumpSaveAll()));
	connect(this->ui.pushButton_MemorySaveToFile,SIGNAL(clicked()),this,SLOT(SLOT_onMemDumpSave()));
	connect(this->ui.pushButton_StackUpdate,SIGNAL(clicked()),this,SLOT(SLOT_UpdateStack()));
	connect(this->ui.pushButton_StackSearch,SIGNAL(clicked()),this,SLOT(SLOT_UpdateSearchStack()));
	connect(this->ui.pushButton_StackClear,SIGNAL(clicked()),this,SLOT(SLOT_onStackClear()));

	connect(this->ui.pushButton_Mem_Dump,SIGNAL(clicked()),this,SLOT(SLOT_UpdateMemory()));

	connect(this->m_ConnectorDialog,SIGNAL(SIGANL_ConfirmHost(QString)),this,SLOT(SLOT_onSetupHost(QString)));
	connect(this->ui.lineEdit_CommandLine,SIGNAL(returnPressed()),this,SLOT(SLOT_onCommandLineEnter()));
	connect(&this->m_Core,SIGNAL(SIGNAL_ConnectionStatisChanged()),this,SLOT(SLOT_onConnectionStatusChanged()));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitMonitorMessage(QString,QColor)),this,SLOT(SLOT_onMonitorMessageDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitRegistryMessage(QString,QColor)),this,SLOT(SLOT_onRegistryMessageDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitFilesMessage(QString,QColor)),this,SLOT(SLOT_onFilesMessageDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitProcessesMessage(QString,QColor)),this,SLOT(SLOT_onProcessesDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitWindowsOperatorMessage(QString,QColor)),this,SLOT(SLOT_onWindowsOperatorMessageDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitNetworkMessage(QString,QColor)),this,SLOT(SLOT_onNetworkDiaplay(QString,QColor)));
	connect(&this->m_Core,SIGNAL(SIGNAL_EmitDumpTableUpdate()),this,SLOT(SLOT_UpdateDumpPack()));
	connect(&this->m_Core,SIGNAL(SIGNAL_onQuery()),this,SLOT(SLOT_onQuery()));
	connect(this,SIGNAL(SIGNAL_NextQuery()),this,SLOT(SLOT_onQuery()));

	if (m_Core.IsReg())
	{
		this->setWindowTitle(tr("PeDoll--PE32 Monitor www.52pojie.cn"));
		ui.widget_DumpRegOpen->setVisible(true);
		ui.label_RegNotice->setVisible(false);
		ui.action_menu_regist->setEnabled(false);
		ui.widget_StackPanel->setVisible(true);
		ui.label_StackRegNotice->setVisible(false);
	}
	else
	{
		ui.widget_DumpRegOpen->setVisible(false);
		ui.label_RegNotice->setVisible(true);
		ui.widget_StackPanel->setVisible(false);
		ui.label_StackRegNotice->setVisible(true);
		DWORD reg=m_Core.GetSerialDWORD();
		char NumSer[32];
		sprintf(NumSer,"%02X-%02X-%02X-%02X",(reg>>24)&0xff,(reg>>16)&0xff,(reg>>8)&0xff,reg&0xff);
		m_RegisterDialog->ui.label_MacID->setText(NumSer);
	}

}

PeDollc::~PeDollc()
{

}

void PeDollc::OUTPUT_Monitor_Message_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Monitor_Messages->append(ColorStr);
	
}

void PeDollc::OUTPUT_Registry_Message_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Register->append(ColorStr);
}

void PeDollc::OUTPUT_Files_Message_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Files->append(ColorStr);
}

void PeDollc::OUTPUT_WindowsOperator_Message_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Windows_Operator->append(ColorStr);
}

void PeDollc::OUTPUT_Processes_Message_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Processes->append(ColorStr);
}

void PeDollc::OUTPUT_Network_Disply(QColor Color,QString str)
{
	QString ColorStr;
	ColorStr.sprintf("<font color=\"#%06X\">",Color.rgba()&0xFFFFFF);
	ColorStr+=(str+"</font>");
	ui.textEdit_Network->append(ColorStr);
}

void PeDollc::UpdateConnectUI()
{
	if (m_Core.GetControllerNetWorkInterface()->IsConnecting())
	{
		ui.action_Tool_ControllerConnect->setEnabled(false);
		ui.actionControllerClose->setEnabled(true);
	}
	else
	{
		ui.action_Tool_ControllerConnect->setEnabled(true);
		ui.actionControllerClose->setEnabled(false);
	}

	if (m_Core.GetDollNetWorkInterface()->IsConnecting())
	{
		ui.action_Tool_DollConnect->setEnabled(false);
		ui.action_Tool_Run->setEnabled(true);
		ui.action_Tool_Hook->setEnabled(true);
		ui.action_Tool_Stop->setEnabled(true);
	}
	else
	{
		SLOT_onClearQuery();
		ui.action_Tool_DollConnect->setEnabled(true);
		ui.action_Tool_Run->setEnabled(false);
		ui.action_Tool_Hook->setEnabled(false);
		ui.action_Tool_Stop->setEnabled(false);
	}
}

void PeDollc::UpdateDumpInfo()
{
	ui.spinBox_DumpMaxCount->setValue(m_Core.GetMaxDumpCount());
	ui.spinBox_DumpMaxSize->setValue(m_Core.GetMaxDumpSize());
}

void PeDollc::DisplayHexDump(pt_byte *dump,int Size)
{
	QString htmlCodes;
	QString format;
	for (int i=0;i<Size;i++)
	{
		if (i%2)
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0x00FF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0xFFFF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
	}

	ui.textEdit_NetworkHex->clear();
	ui.textEdit_NetworkHex->append(htmlCodes);
}

void PeDollc::DisplayANSIDump(pt_byte *dump,int Size)
{
#define ANSI_ENCODE_GBK		1
#define ANSI_ENCODE_UTF8	2
#define ANSI_ENCODE_UTF16	3
#define ANSI_ASM_X64		4
#define ANSI_ASM_X86		5
#define ANSI_ASM_X16		6
	int Encode=ANSI_ENCODE_GBK;

	if (ui.radioButton_GBK->isChecked())
	{
		Encode=ANSI_ENCODE_GBK;
	}

	if (ui.radioButton_UTF8->isChecked())
	{
		Encode=ANSI_ENCODE_UTF8;
	}

	if (ui.radioButton_UTF16->isChecked())
	{
		Encode=ANSI_ENCODE_UTF16;
	}

	if (ui.radioButton_ASM64->isChecked())
	{
		Encode=ANSI_ASM_X64;
	}

	if (ui.radioButton_ASM86->isChecked())
	{
		Encode=ANSI_ASM_X86;
	}

	if (ui.radioButton_ASM16->isChecked())
	{
		Encode=ANSI_ASM_X16;
	}


	QString htmlCodes;
	QString format;
	QString local;
	for (int i=0;i<Size;i++)
	{
		if(dump[i]>='!'&&dump[i]<='~')
		{
			format.sprintf("<font color=\"#%06X\">%c</font>",0x00FF00,dump[i]);
			htmlCodes+=format;
		}
		else if(dump[i]<0&&i<Size-1)
		{
			char _Encode4Bytesw[8]={0};
			_Encode4Bytesw[0]=dump[i];
			i++;
			_Encode4Bytesw[1]=dump[i];
			format.sprintf("<font color=\"#%06X\">",0x00FF00);
			if(Encode==ANSI_ENCODE_GBK)
				format+=QString::fromLocal8Bit(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF8)
				format+=QString::fromUtf8(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF16)
				format+=QString::fromUtf16((ushort *)_Encode4Bytesw);

			if (Encode==ANSI_ASM_X64)
			{
				ud_t ud_obj;
				ud_init(&ud_obj);
				ud_set_input_buffer(&ud_obj,(uint8_t *)dump,Size);
				ud_set_mode(&ud_obj, 64);
				ud_set_syntax(&ud_obj, UD_SYN_INTEL);

				while (ud_disassemble(&ud_obj)) {
					format+=QString(ud_insn_asm(&ud_obj))+"<br>";
				}
			}

			if (Encode==ANSI_ASM_X86)
			{
				ud_t ud_obj;
				ud_init(&ud_obj);
				ud_set_input_buffer(&ud_obj,(uint8_t *)dump,Size);
				ud_set_mode(&ud_obj,32);
				ud_set_syntax(&ud_obj, UD_SYN_INTEL);

				while (ud_disassemble(&ud_obj)) {
					format+=QString(ud_insn_asm(&ud_obj))+"<br>";
				}
			}

			if (Encode==ANSI_ASM_X16)
			{
				ud_t ud_obj;
				ud_init(&ud_obj);
				ud_set_input_buffer(&ud_obj,(uint8_t *)dump,Size);
				ud_set_mode(&ud_obj, 16);
				ud_set_syntax(&ud_obj, UD_SYN_INTEL);

				while (ud_disassemble(&ud_obj)) {
					format+=QString(ud_insn_asm(&ud_obj))+"<br>";
				}
			}

			format+="</font>";
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">.</font>",0x00FF00);
			htmlCodes+=format;
		}
	}
	ui.textEdit_NetworkASC->clear();
	ui.textEdit_NetworkASC->append(htmlCodes);
}

void PeDollc::SLOT_onAntiVirtualization()
{
	char appendstring[]="isolation.tools.getPtrLocation.disable = \"TRUE\"\n\
isolation.tools.setPtrLocation.disable = \"TRUE\"\n\
isolation.tools.setVersion.disable = \"TRUE\"\n\
isolation.tools.getVersion.disable = \"TRUE\"\n\
monitor_control.disable_directexec = \"TRUE\"\n\
monitor_control.disable_chksimd = \"TRUE\"\n\
monitor_control.disable_ntreloc = \"TRUE\"\n\
monitor_control.disable_selfmod = \"TRUE\"\n\
monitor_control.disable_reloc = \"TRUE\"\n\
monitor_control.disable_btinout = \"TRUE\"\n\
monitor_control.disable_btmemspace = \"TRUE\"\n\
monitor_control.disable_btpriv = \"TRUE\"\n\
monitor_control.disable_btseg = \"TRUE\"";

	QString fileName = QFileDialog::getOpenFileName(this,tr("Open vmx File"),".",tr("VMX File(*.vmx)"));
	if(fileName.length() != 0)
	{
		FILE *pf=fopen(fileName.toLocal8Bit().data(),"a+");
		if (pf!=NULL)
		{
			fwrite(appendstring,sizeof(appendstring)-1,1,pf);
			fclose(pf);
			QMessageBox::information(NULL,tr("Succeeded"),"Done,Restart VM Please");
		}
		else
		{
			QMessageBox::information(NULL,tr("error"),"Could not open target file.");
		}
	}
}

void PeDollc::SLOT_onSwitchOld()
{
	if (m_OldInstr.empty())
	{
		return;
	}
	if (m_TabIndex>=m_OldInstr.size())
	{
		m_TabIndex=0;
	}
	ui.lineEdit_CommandLine->setText(m_OldInstr[m_OldInstr.size()-m_TabIndex-1]);
	m_TabIndex++;
}

void PeDollc::SLOT_onDumpClear()
{
	m_Core.ClearDumpPack();
	ui.listWidget_Dump->clear();
	ui.textEdit_NetworkASC->clear();
	ui.textEdit_NetworkHex->clear();
}

void PeDollc::SLOT_onDumpDelete()
{
	m_Core.RemoveDumpPack(ui.listWidget_Dump->currentRow());
	SLOT_UpdateDumpPack();
}

void PeDollc::SLOT_onDumpSave()
{
	if (ui.listWidget_Dump->currentRow()<0)
	{
		return;
	}
	int index=ui.listWidget_Dump->currentRow();

	vector<PEDOLL_DUMP_PACK> snapshot=m_Core.GetDumpPacketsSnapshot();

	if (index>=snapshot.size())
	{
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Dump"),
		"",
		tr("Dump Files (*.bin)"));

	if (!fileName.isNull())
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		file.write(snapshot.at(index).Dump,snapshot.at(index).Size);
		file.close();
	}
	
}

void PeDollc::SLOT_onMemDumpSave()
{
	if (m_DumpBuffer==NULL)
	{
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Dump"),
		"",
		tr("Dump Files (*.bin)"));

	if (!fileName.isNull())
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		file.write(m_DumpBuffer,m_DumpSize);
		file.close();
	}

}

void PeDollc::SLOT_onDumpSaveAll()
{

	vector<PEDOLL_DUMP_PACK> snapshot=m_Core.GetDumpPacketsSnapshot();

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save dumps"),
		"",
		tr("Config Files (*.*)"));

	if (!fileName.isNull())
	{
		for (int i=0;i<snapshot.size();i++)
		{
			QFile file(fileName+QString::number(i)+".bin");
			file.open(QIODevice::WriteOnly);
			file.write(snapshot.at(i).Dump,snapshot.at(i).Size);
			file.close();
		}
		
	}
}


void PeDollc::SLOT_About()
{
	m_AboutDialog->show();
}

void PeDollc::SLOT_onClear()
{
	ui.textEdit_Files->clear();
	ui.textEdit_Monitor_Messages->clear();
	ui.textEdit_Network->clear();
	ui.textEdit_Processes->clear();
	ui.textEdit_Register->clear();
	ui.textEdit_Windows_Operator->clear();
}

void PeDollc::SLOT_DumpConfigureChanged()
{
	m_Core.SetDumpMaxCount(ui.spinBox_DumpMaxCount->value());
	m_Core.SetDumpMaxSize(ui.spinBox_DumpMaxSize->value());
}

void PeDollc::SLOT_LoadHookScript()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open a File"),".",tr("Hook script File(*.txt)"));
		if(fileName.length() == 0)
		{}
		else
		{
			QDir *pDir = new QDir(".");
			QString fileDir = pDir->filePath(fileName);

			QFile file(fileName);
			if (!file.open(QIODevice::ReadWrite))  
				return;
			QTextStream out(&file);
			while(TRUE)
			{
				QString lineString=out.readLine();
				if (lineString==0)
				{
					break;
				}
				CubeGrammarSentence sen;
				m_Lexer.SortText((char *)lineString.toLocal8Bit().data());
				unsigned int SenType;
				SenType=m_Grammar.GetNextInstr(sen);

				if (SenType==m_Instr_hook||SenType==m_Instr_hook)
				{
					QString ColorStr;
					ColorStr.sprintf("<font color=\"#%06X\">",0x00FF00);
					ColorStr+=(lineString+"</font>");
					ui.textEdit_Hook_Regular->append(ColorStr);
					continue;
				}
				if (SenType==m_Instr_binary)
				{
					QString ColorStr;
					ColorStr.sprintf("<font color=\"#%06X\">",0x4FFF00);
					ColorStr+=(lineString+"</font>");
					ui.textEdit_Hook_Regular->append(ColorStr);
					continue;
				}

				if (SenType==m_Instr_FilterStart)
				{
					goto FILTER_START;
				}
				QMessageBox::information(NULL,tr("Error"),lineString+" is illegal script");
				return;
			}
			OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Hook table loaded completed."));
			return;
FILTER_START:
			while (TRUE)
			{
				QString lineString=out.readLine();
				if (lineString==0)
				{
					break;
				}
				QString ColorStr;
				
				m_Core.AddFunctionFilter(lineString.toLocal8Bit().data());

				lineString.replace('<',"\"");
				lineString.replace('>',"\"");
				ColorStr.sprintf("<font color=\"#%06X\">",0x00FF00);
				ColorStr+=(lineString+"</font>");
				ui.textEdit_Functions_Filter->append(ColorStr);
			}
			OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Filter table loaded completed."));
		}
}



void PeDollc::SLOT_onHookScript()
{
	if (!m_Core.GetDollNetWorkInterface()->IsConnecting())
	{
		SLOT_onMonitorMessageDiaplay(tr("Doll must be connected before hook functions"),QColor(255,0,0));
	}
	CubeGrammarSentence sen;
	m_Lexer.SortText((char *)ui.textEdit_Hook_Regular->toPlainText().toStdString().c_str());
	unsigned int SenType;
	
	while(TRUE)
	{
		SenType=m_Grammar.GetNextInstr(sen);
		if (SenType==GRAMMAR_SENTENCE_UNKNOW)
		{
			QMessageBox::information(this,tr("Error"),m_Lexer.GetLexemeString()+tr(" is illegal script"));
			return;
		}
		if(SenType==GRAMMAR_SENTENCE_END)
		{
			break;
		}
		if(SenType==m_Instr_hook)
		{
			char CMD[PEDOLL_EXECUTE_REGULAR_CMD_LEN];
			sprintf(CMD,"%s %s",sen.GetBlockString(0),sen.GetBlockString(1));
			if(m_Core.GetDollNetWorkInterface()->SendCommand(CMD))
			{
				SLOT_onMonitorMessageDiaplay(tr("Hook Function succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
			}
			else
			{
				SLOT_onMonitorMessageDiaplay(tr("Hook Function failed:")+sen.GetBlockString(1),QColor(255,255,0));
			}
			continue;
		}
		if(SenType==m_Instr_unhook)
		{
			char CMD[PEDOLL_EXECUTE_REGULAR_CMD_LEN];
			sprintf(CMD,"%s %s",sen.GetBlockString(0),sen.GetBlockString(1));
			if(m_Core.GetDollNetWorkInterface()->SendCommand(CMD))
			{
				SLOT_onMonitorMessageDiaplay(tr("Unhook Function succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
			}
			else
			{
				SLOT_onMonitorMessageDiaplay(tr("Unhook Function failed:")+sen.GetBlockString(1),QColor(255,255,0));
			}
			continue;
		}
		if(SenType==m_Instr_binary)
		{
			char CMD[PEDOLL_EXECUTE_REGULAR_CMD_LEN];
			sprintf(CMD,"%s %s %s %s %s",sen.GetBlockString(0),sen.GetBlockString(1),sen.GetBlockString(2),sen.GetBlockString(3),sen.GetBlockString(4));
			if(m_Core.GetDollNetWorkInterface()->SendCommand(CMD))
			{
				SLOT_onMonitorMessageDiaplay(tr("Hook binary succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
			}
			else
			{
				SLOT_onMonitorMessageDiaplay(tr("Hook binary failed:")+sen.GetBlockString(1),QColor(255,255,0));
			}
			continue;
		}


		SLOT_onMonitorMessageDiaplay(tr("Not a support command:")+sen.GetBlockString(0),QColor(255,255,0));
	}
}

void PeDollc::SLOT_onRun()
{
	if (!m_Core.GetDollNetWorkInterface()->IsConnecting())
	{
		SLOT_onMonitorMessageDiaplay(tr("Doll must be connected before run"),QColor(255,0,0));
	}
	m_Core.GetDollNetWorkInterface()->SendCommand("run");
}

void PeDollc::SLOT_onTerminate()
{
	char TerCmd[MAX_PATH];
	sprintf(TerCmd,"%s <%s>",PEDOLL_CONTROLLER_CMD_KILL,m_Core.GetProcName());

	if (!m_Core.GetDollNetWorkInterface()->IsConnecting())
	{
		SLOT_onMonitorMessageDiaplay(tr("Doll must be connected before terminate"),QColor(255,0,0));
	}
	m_Core.GetDollNetWorkInterface()->SendCommand("terminate");
	m_Core.GetDollNetWorkInterface()->Disconnect();

	if(m_Core.GetControllerNetWorkInterface()->IsConnecting())
	m_Core.GetControllerNetWorkInterface()->SendCommand(TerCmd);
}

void PeDollc::SLOT_onControllerConnect()
{
	if (m_Host=="")
	{
		OUTPUT_Monitor_Message_Disply(QColor(255,0,0),tr("You must setup host before connect."));
		return;
	}
	OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Try to connect to controller :")+m_Host);
	if (m_Core.ConnectToController())
	{
		OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Controller connected succeeded"));
	}
	else
	{
		OUTPUT_Monitor_Message_Disply(QColor(255,0,0),tr("Can not connect to controller"));
	}


	UpdateConnectUI();
}

void PeDollc::SLOT_onControllerClose()
{
	
	OUTPUT_Monitor_Message_Disply(QColor(0,255,255),tr("Controller close from:")+m_Host);
	m_Core.GetControllerNetWorkInterface()->Disconnect();
	UpdateConnectUI();
}

void PeDollc::SLOT_onDollConnect()
{
	if (m_Host=="")
	{
		OUTPUT_Monitor_Message_Disply(QColor(255,0,0),tr("You must setup host before connect."));
		return;
	}
	OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Try to connect to doll :")+m_Host);

	if (m_Core.ConnectToDoll())
	 {
	 	OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Doll connected succeeded"));
	 }
	 else
	 {
	 	OUTPUT_Monitor_Message_Disply(QColor(255,0,0),tr("Doll monitor is not ready"));
	 }

	UpdateConnectUI();
}

void PeDollc::SLOT_onMonitorMessageDiaplay(QString st,QColor cl)
{
	OUTPUT_Monitor_Message_Disply(cl,st);
}

void PeDollc::SLOT_onRegistryMessageDiaplay(QString st,QColor cl)
{
	OUTPUT_Registry_Message_Disply(cl,st);
}

void PeDollc::SLOT_onFilesMessageDiaplay(QString st,QColor cl)
{
	OUTPUT_Files_Message_Disply(cl,st);
}

void PeDollc::SLOT_onWindowsOperatorMessageDiaplay(QString st,QColor cl)
{
	OUTPUT_WindowsOperator_Message_Disply(cl,st);
}

void PeDollc::SLOT_onProcessesDiaplay(QString st,QColor cl)
{
	OUTPUT_Processes_Message_Disply(cl,st);
}

void PeDollc::SLOT_onNetworkDiaplay(QString cl,QColor st)
{
	OUTPUT_Network_Disply(st,cl);
}

void PeDollc::SLOT_onQuery()
{
	PEDOLL_QUERY_STRUCT str=m_Core.getQuery();
	if (str.APIName=="")
	{
		ui.textEdit_Query->clear();
		ui.pushButton_QueryPass->setEnabled(false);
		ui.pushButton_QueryReject->setEnabled(false);
		ui.pushButton_QueryTerminate->setEnabled(false);
		return;
	}
	QString html=QString(\
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\
<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }\
</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; font-weight:600; color:#ff0000;\">API:</span></p>\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; color:#ffff7f;\">%1</span></p>\
<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#55ffff;\">MSDN Notes:</span></p>\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00ff00;\">%2</span></p>\
<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; color:#00ff00;\"><br /></p>\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; font-weight:600; color:#ffaa00;\">Behavior:</span></p>\
<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00ff00;\">%3</span></p></body></html>")\
.arg(str.APIName).arg(str.msdnNote).arg(str.Behavior);
		ui.textEdit_Query->setHtml(html);
		ui.pushButton_QueryPass->setEnabled(true);
		ui.pushButton_QueryReject->setEnabled(true);
		ui.pushButton_QueryTerminate->setEnabled(true);

		ui.tabWidget->setCurrentIndex(1);
}

void PeDollc::SLOT_onClearHookTable()
{
	OUTPUT_Monitor_Message_Disply(QColor(255,255,0),tr("Hook table clear"));
	ui.textEdit_Hook_Regular->clear();
}

void PeDollc::SLOT_onClearFilterTable()
{
	OUTPUT_Monitor_Message_Disply(QColor(255,255,0),tr("Filter table clear"));
	ui.textEdit_Functions_Filter->clear();
	m_Core.ClearFunctionFilter();
}

void PeDollc::SLOT_onClearQuery()
{
	m_Core.clearQueries();
	ui.textEdit_Query->clear();
	ui.pushButton_QueryPass->setEnabled(false);
	ui.pushButton_QueryReject->setEnabled(false);
	ui.pushButton_QueryTerminate->setEnabled(false);
}

void PeDollc::SLOT_PassCurrentQuery()
{
	PEDOLL_QUERY_STRUCT str=m_Core.getQuery();
	if (str.APIName=="")
	{
		return;
	}
	PEDOLL_API_ACK ack;
	ack.PacketID=str.packetID;
	ack.Exec=PEDOLL_EXECUTE_PASS;
	m_Core.GetDollNetWorkInterface()->Send((pt_byte *)&ack,sizeof ack);
	m_Core.popQuery();
	OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("Pass Query:")+str.Behavior);
	ui.tabWidget->setCurrentIndex(0);
	emit SIGNAL_NextQuery();
}

void PeDollc::SLOT_RejectCurrentQuery()
{
	PEDOLL_QUERY_STRUCT str=m_Core.getQuery();
	if (str.APIName=="")
	{
		return;
	}
	PEDOLL_API_ACK ack;
	ack.PacketID=str.packetID;
	ack.Exec=PEDOLL_EXECUTE_REJECT;
	m_Core.GetDollNetWorkInterface()->Send((pt_byte *)&ack,sizeof ack);
	m_Core.popQuery();
	OUTPUT_Monitor_Message_Disply(QColor(255,0,0),tr("Reject Query:")+str.Behavior);
	ui.tabWidget->setCurrentIndex(0);
	emit SIGNAL_NextQuery();
}

void PeDollc::SLOT_TerminateCurrentQuery()
{
	PEDOLL_QUERY_STRUCT str=m_Core.getQuery();
	if (str.APIName=="")
	{
		return;
	}
	PEDOLL_API_ACK ack;
	ack.PacketID=str.packetID;
	ack.Exec=PEDOLL_EXECUTE_TERMINATE;
	m_Core.GetDollNetWorkInterface()->Send((pt_byte *)&ack,sizeof ack);
	m_Core.popQuery();
	OUTPUT_Monitor_Message_Disply(QColor(125,125,255),tr("Terminate Query:")+str.Behavior);
	ui.tabWidget->setCurrentIndex(0);
	emit SIGNAL_NextQuery();
	
}

void PeDollc::SLOT_onExectueCommand(QString cmd)
{
	ui.statusBar->showMessage(tr("Instr:")+cmd,5000);
	m_Core.ExecuteCommand((char *)cmd.toLocal8Bit().data());
}

void PeDollc::SLOT_onCommandLineEnter()
{
	if (!m_OldInstr.empty())
	{
		if (m_OldInstr.size()>=12)
		{
			m_OldInstr.erase(m_OldInstr.begin());
		}

		if (m_OldInstr[m_OldInstr.size()-1]!=ui.lineEdit_CommandLine->text())
		{
			m_OldInstr.push_back(ui.lineEdit_CommandLine->text());
		}
	}
	else
	{
		m_OldInstr.push_back(ui.lineEdit_CommandLine->text());
	}
	m_TabIndex=0;

	SLOT_onExectueCommand(ui.lineEdit_CommandLine->text());
	ui.lineEdit_CommandLine->clear();
}

void PeDollc::SLOT_UpdateDumpPack()
{
	vector<PEDOLL_DUMP_PACK> SnapshotDumps=m_Core.GetDumpPacketsSnapshot();
	if (ui.listWidget_Dump->count()>SnapshotDumps.size())
	{
		ui.listWidget_Dump->clear();
		for (int i=0;i<SnapshotDumps.size();i++)
		{
			ui.listWidget_Dump->addItem(SnapshotDumps.at(i).Note+tr(" Size:")+QString::number(SnapshotDumps.at(i).Size)+tr("bytes"));
		}
	}
	else
	{
		unsigned int startIndex=ui.listWidget_Dump->count();
		for (unsigned int i=startIndex;i<SnapshotDumps.size();i++)
		{
			ui.listWidget_Dump->addItem(SnapshotDumps.at(i).Note+tr(" Size:")+QString::number(SnapshotDumps.at(i).Size)+tr("bytes"));
		}
	}

}

void PeDollc::SLOT_itemDoubleClicked(QListWidgetItem * item)
{
	int index=ui.listWidget_Dump->currentRow();
	if (index<0)
	{
		ui.textEdit_NetworkASC->clear();
		ui.textEdit_NetworkHex->clear();
		return;
	}
	vector<PEDOLL_DUMP_PACK> SnapshotDumps=m_Core.GetDumpPacketsSnapshot();
	if (index>SnapshotDumps.size()-1)
	{
		return;
	}
	this->DisplayANSIDump(SnapshotDumps[index].Dump,SnapshotDumps[index].Size);
	this->DisplayHexDump(SnapshotDumps[index].Dump,SnapshotDumps[index].Size);
}

void PeDollc::SLOT_RadioClicked()
{
	int index=ui.listWidget_Dump->currentRow();
	if (index<0)
	{
		ui.textEdit_NetworkASC->clear();
		ui.textEdit_NetworkHex->clear();
		return;
	}

	
	vector<PEDOLL_DUMP_PACK> SnapshotDumps=m_Core.GetDumpPacketsSnapshot();
	if (index>SnapshotDumps.size()-1)
	{
		return;
	}
	this->DisplayANSIDump(SnapshotDumps[index].Dump,SnapshotDumps[index].Size);
	this->DisplayHexDump(SnapshotDumps[index].Dump,SnapshotDumps[index].Size);
}

void PeDollc::SLOT_UpdateStack()
{
	if(m_Core.UpdateStack())
	{
		SLOT_UpdateStackUI();
	}
	else
	{
		QMessageBox::information(this,tr("error"),tr("Could not read stack,Please make sure program is on \"API breaking\" status."));
	}
}

void PeDollc::SLOT_UpdateMemory()
{
	int retry=10;
	m_DumpSize=ui.spinBox_Mem_Size->value();
	if (m_DumpBuffer)
	{
		delete [] m_DumpBuffer;
	}
	m_DumpBuffer=new char[m_DumpSize];

	int resSize=m_DumpSize;
	DWORD startAddr=hex_to_decimal(ui.lineEdit_Mem_Addr->text().toLocal8Bit().data());
	ui.progressBar_MemPro->setMaximum(m_DumpSize);
	while (resSize)
	{
		if (resSize>=PEDOLL_DUMP_MEMORY_LEN)
		{
			if (!m_Core.ReadAtomMemory(m_DumpBuffer+(m_DumpSize-resSize),startAddr+(m_DumpSize-resSize),PEDOLL_DUMP_MEMORY_LEN))
			{
				if (retry--)
				{
					continue;
				}
				QMessageBox::information(this,tr("error"),tr("Could not read memory,Please make sure target memories is read avaiable."));
				delete [] m_DumpBuffer;
				m_DumpBuffer=NULL;
				return;
			}
			retry=10;
			resSize-=PEDOLL_DUMP_MEMORY_LEN;
			ui.progressBar_MemPro->setValue(m_DumpSize-resSize);
			ui.progressBar_MemPro->update();
		}
		else
		{
			if (!m_Core.ReadAtomMemory(m_DumpBuffer+(m_DumpSize-resSize),startAddr+(m_DumpSize-resSize),resSize))
			{
				if (retry--)
				{
					continue;
				}
				QMessageBox::information(this,tr("error"),tr("Could not read memory,Please make sure target memories is read avaiable."));
				delete [] m_DumpBuffer;
				m_DumpBuffer=NULL;
				return;
			}
			retry=10;
			resSize=0;
			ui.progressBar_MemPro->setValue(m_DumpSize-resSize);
			ui.progressBar_MemPro->update();
		}

	}
	SLOT_UpdateMemoryUI();
	
}

void PeDollc::SLOT_UpdateSearchStack()
{
	bool transIsOk;
	unsigned int vesp=ui.lineEdit_ESP_Addr->text().toUInt(&transIsOk,16);
	if (!transIsOk)
	{
		return;
	}
	if(m_Core.UpdateStack(vesp))
	{
		SLOT_UpdateStackUI();
	}
	else
	{
		QMessageBox::information(this,tr("error"),tr("Could not read stack"));
	}
}

void PeDollc::SLOT_onStackClear()
{
	ui.textEdit_StackASC->clear();
	ui.textEdit_StackHex->clear();
	ui.tableWidget_Stack->clearContents();
	ui.lineEdit_ESP_Addr->clear();
}

void PeDollc::DisplayHexStack(pt_byte *dump,int Size)
{
	QString htmlCodes;
	QString format;
	for (int i=0;i<Size;i++)
	{
		if (i%2)
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0x00FF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0xFFFF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
	}

	ui.textEdit_StackHex->clear();
	ui.textEdit_StackHex->append(htmlCodes);
}

void PeDollc::DisplayHexMemory()
{
	QString htmlCodes;
	QString format;
	pt_byte *dump=m_DumpBuffer;
	int Size=m_DumpSize;
	if (Size>32*1024)
	{
		Size=32*1024;
	}
	if (!m_DumpBuffer)
	{
		return;
	}
	ui.progressBar_MemPro->setMaximum(Size);
	ui.progressBar_MemPro->setValue(0);
	for (int i=0;i<Size;i++)
	{
		if (i%2)
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0x00FF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">%02X </font>",0xFFFF00,(unsigned char)dump[i]);
			htmlCodes+=format;
		}
		ui.progressBar_MemPro->setValue(i);
		if (i%1024)
		{
			ui.progressBar_MemPro->update();
		}
	}

	ui.textEdit_MemHex->clear();
	ui.textEdit_MemHex->append(htmlCodes);
	ui.progressBar_MemPro->setValue(0);
	ui.progressBar_MemPro->update();
}

void PeDollc::DisplayANSIMemory()
{
#define ANSI_ENCODE_GBK		1
#define ANSI_ENCODE_UTF8	2
#define ANSI_ENCODE_UTF16	3
	if (!m_DumpBuffer)
	{
		return;
	}
	pt_byte *dump=m_DumpBuffer;
	int Size=m_DumpSize;
	if (Size>32*1024)
	{
		Size=32*1024;
	}
	ui.progressBar_MemPro->setMaximum(Size);
	ui.progressBar_MemPro->setValue(0);
	int Encode=ANSI_ENCODE_GBK;

	if (ui.radioButton_Mem_GBK->isChecked())
	{
		Encode=ANSI_ENCODE_GBK;
	}

	if (ui.radioButton_Mem_UTF8->isChecked())
	{
		Encode=ANSI_ENCODE_UTF8;
	}

	if (ui.radioButton_Mem_UTF16->isChecked())
	{
		Encode=ANSI_ENCODE_UTF16;
	}

	QString htmlCodes;
	QString format;
	QString local;
	for (int i=0;i<Size;i++)
	{
		ui.progressBar_MemPro->setValue(i);
		if (i%1024)
		{
			ui.progressBar_MemPro->update();
		}
		if(dump[i]>='!'&&dump[i]<='~')
		{
			format.sprintf("<font color=\"#%06X\">%c</font>",0x00FF00,dump[i]);
			htmlCodes+=format;
		}
		else if(dump[i]<0&&i<Size-1)
		{
			char _Encode4Bytesw[8]={0};
			_Encode4Bytesw[0]=dump[i];
			i++;
			_Encode4Bytesw[1]=dump[i];
			format.sprintf("<font color=\"#%06X\">",0x00FF00);
			if(Encode==ANSI_ENCODE_GBK)
				format+=QString::fromLocal8Bit(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF8)
				format+=QString::fromUtf8(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF16)
				format+=QString::fromUtf16((ushort *)_Encode4Bytesw);

			format+="</font>";
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">.</font>",0x00FF00);
			htmlCodes+=format;
		}
	}
	ui.textEdit_MemASC->clear();
	ui.textEdit_MemASC->append(htmlCodes);
	ui.progressBar_MemPro->setValue(0);
	ui.progressBar_MemPro->update();
}

void PeDollc::DisplayANSIStack(pt_byte *dump,int Size)
{
#define ANSI_ENCODE_GBK		1
#define ANSI_ENCODE_UTF8	2
#define ANSI_ENCODE_UTF16	3
	int Encode=ANSI_ENCODE_GBK;

	if (ui.radioButton_GBK->isChecked())
	{
		Encode=ANSI_ENCODE_GBK;
	}

	if (ui.radioButton_UTF8->isChecked())
	{
		Encode=ANSI_ENCODE_UTF8;
	}

	if (ui.radioButton_UTF16->isChecked())
	{
		Encode=ANSI_ENCODE_UTF16;
	}

	QString htmlCodes;
	QString format;
	QString local;
	for (int i=0;i<Size;i++)
	{
		if(dump[i]>='!'&&dump[i]<='~')
		{
			format.sprintf("<font color=\"#%06X\">%c</font>",0x00FF00,dump[i]);
			htmlCodes+=format;
		}
		else if(dump[i]<0&&i<Size-1)
		{
			char _Encode4Bytesw[8]={0};
			_Encode4Bytesw[0]=dump[i];
			i++;
			_Encode4Bytesw[1]=dump[i];
			format.sprintf("<font color=\"#%06X\">",0x00FF00);
			if(Encode==ANSI_ENCODE_GBK)
				format+=QString::fromLocal8Bit(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF8)
				format+=QString::fromUtf8(_Encode4Bytesw);

			if(Encode==ANSI_ENCODE_UTF16)
				format+=QString::fromUtf16((ushort *)_Encode4Bytesw);

			format+="</font>";
			htmlCodes+=format;
		}
		else
		{
			format.sprintf("<font color=\"#%06X\">.</font>",0x00FF00);
			htmlCodes+=format;
		}
	}
	ui.textEdit_StackASC->clear();
	ui.textEdit_StackASC->append(htmlCodes);
}

void PeDollc::SLOT_UpdateStackUI()
{

	ui.label_ESP->setText(QString("ESP:")+QString::number(m_Core.GetUpdateEsp(),16));
	ui.lineEdit_ESP_Addr->setText(QString::number(m_Core.GetUpdateEsp(),16));

	vector<PEDOLL_STACK_INFO> infos=m_Core.GetStackInfoVector();
	ui.tableWidget_Stack->clearContents();
	ui.tableWidget_Stack->setRowCount(PEDOLL_DEFAULT_STACKCOUNT);
	

	for (int i=0;i<infos.size();i++)
	{
		ui.tableWidget_Stack->setItem(i,0,new QTableWidgetItem(QString::number(infos[i].address,16)));
		ui.tableWidget_Stack->setItem(i,1,new QTableWidgetItem(QString::number(infos[i].dwordVal,16)));
		ui.tableWidget_Stack->setItem(i,2,new QTableWidgetItem(QString::number(infos[i].dwordVal,10)));
		infos[i].StringPointer[PEDOLL_MEMORY_LEN-1]='\0';infos[i].StringPointer[PEDOLL_MEMORY_LEN-2]='\0';
		if (ui.radioButton_Stack_GBK->isChecked())
		{
			ui.tableWidget_Stack->setItem(i,3,new QTableWidgetItem(QString::fromLocal8Bit(infos[i].StringPointer)));
		}

		if (ui.radioButton_Stack_UTF8->isChecked())
		{
			ui.tableWidget_Stack->setItem(i,2,new QTableWidgetItem(QString::fromUtf8(infos[i].StringPointer)));
		}

		if (ui.radioButton_Stack_UTF16->isChecked())
		{
			ui.tableWidget_Stack->setItem(i,2,new QTableWidgetItem(QString::fromUtf16((const ushort *)(infos[i].StringPointer))));
		}
	}
	DisplayANSIStack(m_Core.GetStackBuffer(),PEDOLL_DEFAULT_STACKCOUNT*sizeof(DWORD));
	DisplayHexStack(m_Core.GetStackBuffer(),PEDOLL_DEFAULT_STACKCOUNT*sizeof(DWORD));
}

void PeDollc::SLOT_UpdateMemoryUI()
{
	DisplayANSIMemory();
	DisplayHexMemory();
}

void PeDollc::SLOT_onToolDexTransfer()
{
	m_TransferDialog->show();
}

void PeDollc::SLOT_onConnectionStatusChanged()
{
	UpdateConnectUI();
}

void PeDollc::SLOT_ShowConnectorDialog()
{
	m_ConnectorDialog->show();
}

void PeDollc::SLOT_onSetupHost(QString st)
{
	m_Host=st;
	m_Core.SetupHost(st);
	OUTPUT_Monitor_Message_Disply(QColor(0,255,0),tr("set debugger host address : ")+st);
}


void PeDollc_RegisterDialog::SLOT_onBuy()
{
	QDesktopServices::openUrl(QUrl("http://jq.qq.com/?_wv=1027&k=2DeCpK7"));
}

void PeDollc_RegisterDialog::SLOT_OnConfirm()
{
	int Sum=0;
	int Xor=0;

	DWORD num=ui.lineEdit_Key->text().toUInt();

	char szBuffer[MAX_PATH];  
	if(!SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE))
		exit(0);

	strcat(szBuffer,"/PeDoll.reg");

	FILE *pReg=fopen(szBuffer,"wb");
	if (!pReg)
	{
		return;
	}
	fwrite(&num,4,1,pReg);
	fclose(pReg);
	QMessageBox::information(NULL,tr("register done"),tr("please restart this application for verify"));
	hide();
}

void PeDollc_TransferDialog::SLOT_OnGBKtoDex()
{
	char str[2048];
	ui.lineEdit_Dex->clear();
	ui.lineEdit_Hex->clear();
	QString GBK=ui.lineEdit_GBK->text();
	if(GBK.length()>=2047) return;
	strcpy(str,GBK.toLocal8Bit().data());
	unsigned char *p=(unsigned char *)str;
	while(*p)
	{
		ui.lineEdit_Dex->setText(ui.lineEdit_Dex->text()+" "+QString::number(*p,10));
		if(*p<16)
		ui.lineEdit_Hex->setText(ui.lineEdit_Hex->text()+" 0"+QString::number(*p,16));
		else
        ui.lineEdit_Hex->setText(ui.lineEdit_Hex->text()+" "+QString::number(*p,16));
		p++;
	}
}



void PeDollc_TransferDialog::SLOT_OnHextoDex()
{
	QString Hex=ui.lineEdit_Hex->text();
	char hex[3];
	Hex=Hex.replace(" ","");
	if (Hex.length()%1)
	{
		ui.lineEdit_Hex->setText(tr("Error"));
		return;
	}

	char str[2048];
	ui.lineEdit_Dex->clear();
	if(Hex.length()>=2047) return;
	strcpy(str,Hex.toLocal8Bit().data());

	char *pBinary=(char *)str;
	while (*pBinary)
	{
		hex[0]=*pBinary;
		hex[1]=*(pBinary+1);
		hex[2]=0;

		ui.lineEdit_Dex->setText(ui.lineEdit_Dex->text()+" "+QString::number(hex_to_decimal(hex),10));

		pBinary+=2;
	}
}
