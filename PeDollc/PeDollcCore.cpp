#include "PeDollcCore.h"
#define GRAMMAR_TOKEN_NUMBER 1

 

PeDollcCore::PeDollcCore(void)
{
	memset(m_ProcName,0,sizeof m_ProcName);
	CubeInitializeCriticalSection(&m_QueryCS);
	CubeInitializeCriticalSection(&m_DumpCS);
	m_ControllerNetWork.SetCore(this);
	m_ControllerNetWork.AsController();
	m_DollNetWork.SetCore(this);
	m_DollNetWork.AsDoll();
	m_MaxDumpCout=PEDOLL_DEFAULE_DUMPPACKET_MAXCOUNT;
	m_MaxDumpSize=PEDOLL_DEFAULE_DUMPPACKET_MAXSIZE;
	InitializeCommandGrammar();
	InitializeQueryGrammar();
	m_IsRegistied=FALSE;
	m_lastQueryESP=0;
}


PeDollcCore::~PeDollcCore(void)
{
	
}

BOOL PeDollcCore::Initialize()
{
	m_DollNetWork.SetPort(PEDOLL_DOLL_NETWORK_PORT);
	m_ControllerNetWork.SetPort(PEDOLL_CONTROLLER_NETWORK_PORT);
	return TRUE;
ERR:
	return FALSE;
}


BOOL PeDollcCore::IsReg()
{
	m_IsRegistied=TRUE;
	return TRUE;
	DWORD number, digit1, digit2, digit3, digit4, newnum=0;

	number=GetSerialDWORD();

	digit1 = number/1000;
	digit2 = (number - 1000 * digit1)/100;
	digit3 = (number - 1000 * digit1 - 100 * digit2)/10;
	digit4 = number - 1000 * digit1 - 100 * digit2 - 10 * digit3;

	digit1 += 9;
	digit1 %= 10;
	digit2 += 9;
	digit2 %= 10;
	digit3 += 9;
	digit3 %= 10;
	digit4 += 9;
	digit4 %= 10;

	newnum +=number+ digit3 * 1000 + digit4 * 100 + digit1 * 10 +digit2;

	newnum^=0;

	newnum|=0x80000000;

	char szBuffer[MAX_PATH];  
	if(!SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE))
		exit(0);

	strcat(szBuffer,"/PeDoll.reg");

	FILE *pReg=fopen(szBuffer,"rb");
	if (!pReg)
	{
		return FALSE;
	}
	DWORD readReg=0x0;
	fread(&readReg,4,1,pReg);
	fclose(pReg);
	if (readReg==0)
	{
		return FALSE;
	}
	if (readReg==newnum)
	{
		m_IsRegistied=TRUE;
		return TRUE;
	}
	else
	{
		m_IsRegistied=FALSE;
	}
	return FALSE;
}

void PeDollcCore::ShutDownNetWork()
{
	m_DollNetWork.Disconnect();
	m_ControllerNetWork.Disconnect();
}

void PeDollcCore::Free()
{
	ShutDownNetWork();
}


DWORD PeDollcCore::GetSerialDWORD()
{
	char szBuffer[MAX_PATH];  
	if(!SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE))
		exit(0);

	strcat(szBuffer,"/PeDoll.id");
	FILE *pf=fopen(szBuffer,"rb");
	DWORD MagicNumber;
	if (pf==NULL)
	{
		pf=fopen(szBuffer,"wb");
		if (pf==NULL)
		{
			exit(0);
		}
		srand((unsigned)time(NULL));
		MagicNumber=~(rand()<<16|rand());

		fwrite(&MagicNumber,4,1,pf);
		fclose(pf);

		return MagicNumber^0;
	}
	else
	{
		if(fread(&MagicNumber,4,1,pf)==0)
		{
			fclose(pf);
			DeleteFileA(szBuffer);
			exit(0);
		}
		fclose(pf);
		return MagicNumber^0;
	}
}

void PeDollcCore::OnDollDisconnect()
{
	SIGNAL_EmitMonitorMessage(tr("Doll has been disconnect"),QColor(255,255,0));	
	emit SIGNAL_ConnectionStatisChanged();
}

void PeDollcCore::OnControllerDisconnect()
{
	SIGNAL_EmitMonitorMessage(tr("Controller has been disconnect"),QColor(255,255,0));	
	emit SIGNAL_ConnectionStatisChanged();
}

void PeDollcCore::OnDollReceive(pt_byte *Buffer, IN int Size)
{

	switch (*(DWORD *)Buffer)
	{
	case PEDOLL_ACK_MAGICNUMBER:
		break;
	case PEDOLL_MESSAGE_MAGICNUMBER:
		{
			SIGNAL_EmitMonitorMessage(QString::fromLocal8Bit(((PEDOLL_MESSAGE_PACKET *)Buffer)->Messages),QColor(0x66b3ff));	
			if (strstr(((PEDOLL_MESSAGE_PACKET *)Buffer)->Messages,PEDOLL_CONTROLLER_PROCESSNAME_STR)==((PEDOLL_MESSAGE_PACKET *)Buffer)->Messages)
			{
				strcpy(m_ProcName,((PEDOLL_MESSAGE_PACKET *)Buffer)->Messages+sizeof(PEDOLL_CONTROLLER_PROCESSNAME_STR));
			}

		}
		break;
	case PEDOLL_HOOKINFO_MAGICNUMBER:
		{
			ParseDollQuery(*(PEDOLL_HOOKINFO_PACKET *)Buffer);
		}
		break;
	case PEDOLL_ACK_MEMORY:
		{
			PEDOLL_MEMORY_ACK_PACKET *pmp=(PEDOLL_MEMORY_ACK_PACKET *)Buffer;
			memcpy(m_LastReturnBuffer.Buffer,pmp->buffer,sizeof(m_LastReturnBuffer.Buffer));
			m_LastReturnBuffer.address=pmp->Address;
			m_LastReturnBuffer.len=pmp->size;
		}
		break;
	case PEDOLL_DUMP_MAGICNUMBER:
		{
			if (!IsRegMark())
			{
				return;
			}
			PEDOLL_DUMP_PACK DumpPack;
			PEDOLL_DUMP_PACKET_HEADER *pRecvDumpPack=(PEDOLL_DUMP_PACKET_HEADER *)Buffer;
			DumpPack.Note=QString::fromLocal8Bit(pRecvDumpPack->DestInfo);
			DumpPack.Size=Size-sizeof(PEDOLL_DUMP_PACKET_HEADER);
			if (DumpPack.Size<m_MaxDumpSize&&m_DumpPacket.size()<m_MaxDumpCout)
			{
				if (DumpPack.Size>0)
				{
					DumpPack.Dump=new pt_byte[DumpPack.Size];
					if (DumpPack.Dump!=NULL)
					{
						memcpy(DumpPack.Dump,Buffer+sizeof(PEDOLL_DUMP_PACKET_HEADER),DumpPack.Size);
						CubeEnterCriticalSection(&m_DumpCS);
						m_DumpPacket.push_back(DumpPack);
						CubeLeaveCriticalSection(&m_DumpCS);
						emit SIGNAL_EmitDumpTableUpdate();
						emit SIGNAL_EmitMonitorMessage(tr("Dump stream packet: Size ")+QString::number(Size),QColor(255,0x77,255));
						return;
					}
				}
			}
			emit SIGNAL_EmitMonitorMessage(tr("Dump skip : Size ")+QString::number(Size),QColor(255,0x77,255));
		}
		break;
	}

	PEDOLL_ACK_PACKET ack;
	m_DollNetWork.Send((pt_byte *)&ack,sizeof ack);
}

void PeDollcCore::OnControllerReceive(pt_byte *Buffer, IN int Size)
{
	DWORD MagicNumber=*((DWORD *)Buffer);

	switch (MagicNumber)
	{
	case PEDOLL_ACK_MAGICNUMBER:
		break;
	case PEDOLL_MESSAGE_MAGICNUMBER:
		{
			PEDOLL_MESSAGE_PACKET *Pack=(PEDOLL_MESSAGE_PACKET *)Buffer;
			if (Pack->Messages[sizeof(Pack->Messages)-1]!=0)
			{
			if (strlen(Pack->Messages)>=sizeof(Pack->Messages))
			{
				return;
			}
			}
			SIGNAL_EmitMonitorMessage(QString::fromLocal8Bit(Pack->Messages),QColor(0,255,0));	
		}
		break;
	default:
		{
			SIGNAL_EmitMonitorMessage("UnknowPacket",QColor(0,255,0));
		}
		break;;
	}

	PEDOLL_ACK_PACKET ack;
	m_ControllerNetWork.Send((pt_byte *)&ack,sizeof ack);

}

void PeDollcCore::ExecuteCommand(char *Command)
{
	if(!ExecuteControllerCommand(Command)&&!ExecuteDollCommand(Command))
		SIGNAL_EmitMonitorMessage(QString::fromLocal8Bit(Command)+tr(" is not a valid command"),QColor(255,0,0));
}

BOOL PeDollcCore::ExecuteControllerCommand(char *Command)
{
	char Path[MAX_PATH];
	CubeGrammarSentence sen;
	m_CommandLexer.SortText(Command);
	unsigned int SenType;
	SenType=m_CommandGrammar.GetNextInstr(sen);
	if (SenType==GRAMMAR_SENTENCE_UNKNOW||SenType==GRAMMAR_SENTENCE_END)
	{	
		return FALSE;
	}


	if (SenType==m_instr_doll)
	{
		if (!m_ControllerNetWork.IsConnecting()) goto DISCONNECT;
		
		if (m_DollNetWork.IsConnecting())
		{
			SIGNAL_EmitMonitorMessage(tr("Only one doll should be run"),QColor(255,0,0));
			return FALSE;
		}

		
		strcpy(Path,sen.GetBlockString(2));
		int oft=strlen(Path);

		while(TRUE)
		{
			if (Path[oft]=='/'||Path[oft]=='\\')
			{
				strcpy(m_ProcName,Path+oft+1);
				break;
			}
			if (oft==0)
			{
				strcpy(m_ProcName,Path+oft);
				break;
			}
			oft--;
		}
		

		int opCode=0;
		if (strcmp(sen.GetBlockString(1),PEDOLL_CONTROLLER_CMD_DOLL_IMPORTTABLE_STR)==0)
		{
			opCode=PEDOLL_CONTROLLER_CMD_DOLL_IMPORTTABLE;
		}
		if (strcmp(sen.GetBlockString(1),PEDOLL_CONTROLLER_CMD_DOLL_BININJECT_STR)==0)
		{
			opCode=PEDOLL_CONTROLLER_CMD_DOLL_BININJECT;
		}
		if (strcmp(sen.GetBlockString(1),PEDOLL_CONTROLLER_CMD_DOLL_DLLINJECT_STR)==0)
		{
			opCode=PEDOLL_CONTROLLER_CMD_DOLL_DLLINJECT;
		}
		if (strcmp(sen.GetBlockString(1),PEDOLL_CONTROLLER_CMD_DOLL_DEBUG_STR)==0)
		{
			opCode=PEDOLL_CONTROLLER_CMD_DOLL_DEBUG;
		}
		if (strcmp(sen.GetBlockString(1),PEDOLL_CONTROLLER_CMD_DOLL_ACTIVATEINJECT_STR)==0)
		{
			opCode=PEDOLL_CONTROLLER_CMD_DOLL_ACTIVATEINJECT;
		}

		switch (opCode)
		{
		case PEDOLL_CONTROLLER_CMD_DOLL_IMPORTTABLE:
			{
				SIGNAL_EmitMonitorMessage(tr("Inject import table to target Remote://")+QString::fromLocal8Bit(sen.GetBlockString(2)),QColor(0,255,0));
			}
			break;
		case PEDOLL_CONTROLLER_CMD_DOLL_BININJECT:
			{
				SIGNAL_EmitMonitorMessage(tr("Inject binary codes to target Remote://")+QString::fromLocal8Bit(sen.GetBlockString(2)),QColor(0,255,0));
			}
			break;
		case PEDOLL_CONTROLLER_CMD_DOLL_DLLINJECT:
			{
				SIGNAL_EmitMonitorMessage(tr("Inject binary codes to target Remote://Process ID-")+QString::fromLocal8Bit(sen.GetBlockString(2)),QColor(0,255,0));
			}
			break;
		case PEDOLL_CONTROLLER_CMD_DOLL_DEBUG:
			{
				SIGNAL_EmitMonitorMessage(tr("Debug mode to target Remote://Process ID-")+QString::fromLocal8Bit(sen.GetBlockString(2)),QColor(0,255,0));
			}
			break;

		case PEDOLL_CONTROLLER_CMD_DOLL_ACTIVATEINJECT:
			{
				SIGNAL_EmitMonitorMessage(tr("Activate mode to target Remote://Process ID-")+QString::fromLocal8Bit(sen.GetBlockString(2)),QColor(0,255,0));
			}
			break;

		default:
			{
				SIGNAL_EmitMonitorMessage(tr("Not Supported code"),QColor(0,255,0));
				return FALSE;
			}
			break;
		}
		m_ControllerNetWork.SendCommand(Command);
	}

	if (SenType==m_instr_shell)
	{
		if (!m_ControllerNetWork.IsConnecting()) goto DISCONNECT;
		SIGNAL_EmitMonitorMessage(tr("Shell Remote://Process ID-")+sen.GetBlockString(1),QColor(0,255,0));

		if (m_DollNetWork.IsConnecting())
		{
			SIGNAL_EmitMonitorMessage(tr("Only one doll should be run"),QColor(255,0,0));
			return FALSE;
		}

		char Path[MAX_PATH];
		strcpy(Path,sen.GetBlockString(1));
		int oft=strlen(Path);
		while(TRUE)
		{
			if (Path[oft]=='/'||Path[oft]=='\\')
			{
				strcpy(m_ProcName,Path+oft+1);
				break;
			}
			if (oft==0)
			{
				strcpy(m_ProcName,Path+oft);
				break;
			}
			oft--;
		}

		m_ControllerNetWork.SendCommand(Command);
	}

	if (SenType==m_instr_EnumProcess)
	{
		if (!m_ControllerNetWork.IsConnecting()) goto DISCONNECT;
		SIGNAL_EmitMonitorMessage(tr("Enum Processes:"),QColor(0,255,0));
		m_ControllerNetWork.SendCommand(Command);
	}

	if (SenType==m_instr_ls)
	{
		if (!m_ControllerNetWork.IsConnecting()) goto DISCONNECT;
		SIGNAL_EmitMonitorMessage(tr("Enum Directories:"),QColor(0,255,0));
		m_ControllerNetWork.SendCommand(Command);
	}

	if (SenType==m_instr_kill)
	{
		if (!m_ControllerNetWork.IsConnecting()) goto DISCONNECT;
		SIGNAL_EmitMonitorMessage(tr("Kill Process"),QColor(0,255,0));
		m_ControllerNetWork.SendCommand(Command);
	}


	if(SenType==m_Instr_hook)
	{
		char CMD[PEDOLL_EXECUTE_REGULAR_CMD_LEN];
		sprintf(CMD,"%s %s",sen.GetBlockString(0),sen.GetBlockString(1));
		unsigned int Ret;
		if(m_DollNetWork.SendCommand(CMD,&Ret))
		{
			if(Ret!=0)
			SIGNAL_EmitMonitorMessage(tr("Hook Function succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
			else
			SIGNAL_EmitMonitorMessage(tr("Hook Function failed:")+sen.GetBlockString(1),QColor(255,255,0));
		}
		else
		{
			SIGNAL_EmitMonitorMessage(tr("Hook Function failed:")+sen.GetBlockString(1),QColor(255,255,0));
		}
	}
	if(SenType==m_Instr_unhook)
	{
		char CMD[PEDOLL_EXECUTE_REGULAR_CMD_LEN];
		sprintf(CMD,"%s %s",sen.GetBlockString(0),sen.GetBlockString(1));
		if(m_DollNetWork.SendCommand(CMD))
		{
			SIGNAL_EmitMonitorMessage(tr("Unhook Function succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
		}
		else
		{
			SIGNAL_EmitMonitorMessage(tr("Unhook Function failed:")+sen.GetBlockString(1),QColor(255,255,0));
		}
	}
	if (SenType==m_Instr_binary)
	{
		if(m_DollNetWork.SendCommand(Command))
		{
			SIGNAL_EmitMonitorMessage(tr("hook binary succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
		}
		else
		{
			SIGNAL_EmitMonitorMessage(tr("hook binary failed:")+sen.GetBlockString(1),QColor(255,255,0));
		}
	}
	if (SenType==m_Instr_unbinary)
	{
		if(m_DollNetWork.SendCommand(Command))
		{
			SIGNAL_EmitMonitorMessage(tr("Unhook binary succeeded:")+sen.GetBlockString(1),QColor(0,255,0));
		}
		else
		{
			SIGNAL_EmitMonitorMessage(tr("Unhook binary failed:")+sen.GetBlockString(1),QColor(255,255,0));
		}
	}

	return TRUE;
DISCONNECT:
		SIGNAL_EmitMonitorMessage(tr("Controller is not connected"),QColor(255,0,0));
	return TRUE;
}

BOOL PeDollcCore::ExecuteDollCommand(char *Command)
{
	CubeGrammarSentence sen;
	m_CommandLexer.SortText(Command);
	unsigned int SenType;
	SenType=m_CommandGrammar.GetNextInstr(sen);
	if (SenType==GRAMMAR_SENTENCE_UNKNOW||SenType==GRAMMAR_SENTENCE_END)
	{
		return FALSE;
	}
	return TRUE;
	if (!m_DollNetWork.IsConnecting())
	{
		SIGNAL_EmitMonitorMessage(tr("Doll is not connected"),QColor(255,0,0));
		return TRUE;
	}

	m_DollNetWork.SendCommand(Command);
}

void PeDollcCore::addQuery(PEDOLL_QUERY_STRUCT query)
{
	CubeEnterCriticalSection(&m_QueryCS);
	m_QueryQueue.push(query);
	CubeLeaveCriticalSection(&m_QueryCS);
	emit SIGNAL_onQuery();
}

PEDOLL_QUERY_STRUCT PeDollcCore::getQuery()
{
	if(m_QueryQueue.size()!=0)
	return m_QueryQueue.front();
	return PEDOLL_QUERY_STRUCT();
}

void PeDollcCore::popQuery()
{
	CubeEnterCriticalSection(&m_QueryCS);
	m_QueryQueue.pop();
	CubeLeaveCriticalSection(&m_QueryCS);
}

void PeDollcCore::clearQueries()
{
	CubeEnterCriticalSection(&m_QueryCS);
	while (!m_QueryQueue.empty())
		m_QueryQueue.pop();
	CubeLeaveCriticalSection(&m_QueryCS);
}

void PeDollcCore::SetupHost(QString host)
{
	m_Host=host;
}

void PeDollcCore::ParseDollQuery(PEDOLL_HOOKINFO_PACKET Query)
{
	CubeGrammarSentence sen;
	PEDOLL_API_ACK	apiAck;
	PEDOLL_QUERY_STRUCT INFORMATION_Query;
	m_QueryLexer.SortText(Query.APIInfo);
	unsigned int SenType;
	bool		 bMatch;
	bool		 bShow;
	apiAck.Exec=PEDOLL_EXECUTE_PASS;
	apiAck.PacketID=Query.PacketID;
	SenType=m_QueryGrammar.GetNextInstr(sen);
	if (SenType==GRAMMAR_SENTENCE_UNKNOW||SenType==GRAMMAR_SENTENCE_END)
	{	
		QString Info=Query.APIInfo;
		Info.replace('<',"\"");
		Info.replace('>',"\"");
		emit SIGNAL_EmitMonitorMessage(tr("Not Parsed API:")+Info,QColor(0,255,255));
		goto _ACK_RETRUN;
	}

	#define SEN_STRING_LOCAL(x) QString::fromLocal8Bit(sen.GetBlockString(x))

	if (SenType==m_Reply_REGOPENKEYEXA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegOpenKeyExA";
				INFORMATION_Query.msdnNote=tr("Opens the specified registry key. Note that key names are not case sensitive.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				m_lastQueryESP=Query.esp;
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
				break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Open registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGSETKEYVALUEA)
	{	
		switch(SenQuery(sen,3,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(0xc0,255,0x3e));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegSetKeyValueA";
				INFORMATION_Query.msdnNote=tr("Sets the data for the specified value in the specified registry key and subkey.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(255,0,0));
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow)
			{
					emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(128,128,255));
					emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3)+"\\"+SEN_STRING_LOCAL(4),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGSETVALUEEXA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(0xc0,255,0x3e));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegSetValueExA";
				INFORMATION_Query.msdnNote=tr("Sets the data and type of a specified value under a registry key.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGSETVALUEEXW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegSetValueExW";
				INFORMATION_Query.msdnNote=tr("Sets the data and type of a specified value under a registry key.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2)+"\\"+SEN_STRING_LOCAL(3),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGSETVALUEA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegSetValueA";
				INFORMATION_Query.msdnNote=tr("Sets the data for the default or unnamed value of a specified registry key. The data must be a text string.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Set registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGCREATEKEYA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegCreateKeyA";
				INFORMATION_Query.msdnNote=tr("Creates the specified registry key. If the key already exists in the registry, the function opens it.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_REGCREATEKEYEXA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegCreateKeyExA";
				INFORMATION_Query.msdnNote=tr("Creates the specified registry key. If the key already exists, the function opens it. Note that key names are not case sensitive.<br>\
To perform transacted registry operations on a key, call the RegCreateKeyTransacted function.<br>\
Applications that back up or restore system state including system files and registry hives should use the Volume Shadow Copy Service instead of the registry functions.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_REGCREATEKEYEXW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitRegistryMessage(tr("PASSED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="RegCreateKeyExW";
				INFORMATION_Query.msdnNote=tr("Creates the specified registry key. If the key already exists, the function opens it. Note that key names are not case sensitive.<br>\
To perform transacted registry operations on a key, call the RegCreateKeyTransacted function.<br>\
Applications that back up or restore system state including system files and registry hives should use the Volume Shadow Copy Service instead of the registry functions.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitRegistryMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitMonitorMessage(tr("REJECTED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATED:Create registry key ")+SEN_STRING_LOCAL(1)+"\\"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_FINDWINDOWEXA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="FindWindowExA";
				INFORMATION_Query.msdnNote=tr("Retrieves a handle to a window whose class name and window name match the specified strings. The function<br> \
searches child windows, beginning with the one following the specified child window. This function does not perform a case-sensitive search.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_FINDWINDOWEXW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="FindWindowExW";
				INFORMATION_Query.msdnNote=tr("Retrieves a handle to a window whose class name and window name match the specified strings. The function<br> \
 searches child windows, beginning with the one following the specified child window. This function does not perform a case-sensitive search.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitRegistryMessage(tr("TERMINATE:Find Window Class:")+SEN_STRING_LOCAL(1)+tr(" Name:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_CREATEFILEA)
	{	
		QString op=tr("UNKNOW");
		QString par=SEN_STRING_LOCAL(1);
		QColor color;
		if (par=="UNKNOW")
		{
			op=tr("UNKNOW");
			color=QColor(255,0x7f,0);
		}

		if (par=="CREATE")
		{
			op=tr("Create");
			color=QColor(0x7f,255,0xd4);
		}

		if (par=="OPEN")
		{
			color=QColor(0,255,0);
			op=tr("Open");
		}

		if (par=="RESET")
		{
			op=tr("Reset");
			color=QColor(75,255,88);
		}

		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			emit SIGNAL_EmitMonitorMessage(tr("PASSED:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			emit SIGNAL_EmitFilesMessage(tr("PASSED:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),color);
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateFileA";
				INFORMATION_Query.msdnNote=tr("Creates or opens a file or I/O device. The most commonly used I/O devices are as follows:<br>\
file, file stream, directory, physical disk, volume, console buffer, tape drive, communications resource, mailslot, and pipe.<br>\
<br>The function returns a handle that can be used to access the file or device for various types of I/O depen\
ding on the file or device and the flags and attributes specified.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("%1 the file").arg(op)+":"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			emit SIGNAL_EmitFilesMessage(tr("REJECT:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			emit SIGNAL_EmitMonitorMessage(tr("REJECT:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			break;
		case QUERY_RETURN_TERMINATE:
			emit SIGNAL_EmitFilesMessage(tr("TERMINATE:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
	}

	if (SenType==m_Reply_CREATEFILEW)
	{	
		QString op=tr("UNKNOW");
		QString par=SEN_STRING_LOCAL(1);
		QColor color;
		if (par=="UNKNOW")
		{
			op=tr("UNKNOW");
			color=QColor(255,0x7f,0);
		}

		if (par=="CREATE")
		{
			op=tr("Create");
			color=QColor(0x7f,255,0xd4);
		}

		if (par=="OPEN")
		{
			op=tr("Open");
			color=QColor(0,255,0);
		}

		if (par=="RESET")
		{
			op=tr("Reset");
			color=QColor(75,255,88);
		}

		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			emit SIGNAL_EmitMonitorMessage(tr("PASSED:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(0,255,0));
			emit SIGNAL_EmitFilesMessage(tr("PASSED:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),color);
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateFileA";
				INFORMATION_Query.msdnNote=tr("Creates or opens a file or I/O device. The most commonly used I/O devices are as follows:<br>\
file, file stream, directory, physical disk, volume, console buffer, tape drive, communications resource, mailslot, and pipe.<br>\
<br>The function returns a handle that can be used to access the file or device for various types of I/O depen\
 ding on the file or device and the flags and attributes specified.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("%1 the file").arg(op)+":"+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			emit SIGNAL_EmitFilesMessage(tr("REJECT:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			emit SIGNAL_EmitMonitorMessage(tr("REJECT:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(255,0,0));
			break;
		case QUERY_RETURN_TERMINATE:
			emit SIGNAL_EmitFilesMessage(tr("TERMINATE:%1 File").arg(op)+":"+SEN_STRING_LOCAL(2),QColor(128,128,255));
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
	}

	if (SenType==m_Reply_CREATEPROCESSINTERNALW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitProcessesMessage(tr("PASSED:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateProcessInternalW";
				INFORMATION_Query.msdnNote=tr("unSpecific");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitProcessesMessage(tr("REJECT:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitProcessesMessage(tr("TERMINATE:Create process(internal) Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_CREATEPROCESSW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitProcessesMessage(tr("PASSED:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateProcessW";
				INFORMATION_Query.msdnNote=tr("Creates a new process and its primary thread. The new process runs in the security context of the calling process.<br>\
If the calling process is impersonating another user, the new process uses the token for the calling process, not the impersonation token.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitProcessesMessage(tr("REJECT:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitProcessesMessage(tr("TERMINATE:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_CREATEPROCESSA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitProcessesMessage(tr("PASSED:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateProcessA";
				INFORMATION_Query.msdnNote=tr("Creates a new process and its primary thread. The new process runs in the security context of the calling process.<br>\
If the calling process is impersonating another user, the new process uses the token for the calling process, not the impersonation token.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitProcessesMessage(tr("REJECT:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitProcessesMessage(tr("TERMINATE:Create process Name:")+SEN_STRING_LOCAL(1)+tr(" CommandLine:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_CONNECT)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitNetworkMessage(tr("PASSED:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="connect";
				INFORMATION_Query.msdnNote=tr("The connect function establishes a connection to a specified socket.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitNetworkMessage(tr("REJECT:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitNetworkMessage(tr("TERMINATE:connect to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SEND)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitNetworkMessage(tr("PASSED:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="send";
				INFORMATION_Query.msdnNote=tr("The send function sends data on a connected socket.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitNetworkMessage(tr("REJECT:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitNetworkMessage(tr("TERMINATE:send(TCP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SENDTO)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:sendto(UDP) to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitNetworkMessage(tr("PASSED:sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="sendto";
				INFORMATION_Query.msdnNote=tr("The sendto function sends data to a specific destination.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitNetworkMessage(tr("REJECT:sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitNetworkMessage(tr("TERMINATE:sendto(UDP)  to :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_RECV)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitNetworkMessage(tr("PASSED:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="recv";
				INFORMATION_Query.msdnNote=tr("The recv function receives data from a connected socket or a bound connectionless socket.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitNetworkMessage(tr("REJECT:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitNetworkMessage(tr("TERMINATE:Recv(TCP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_RECVFROM)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitNetworkMessage(tr("PASSED:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="recvfrom";
				INFORMATION_Query.msdnNote=tr("The recvfrom function receives a datagram and stores the source address.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitNetworkMessage(tr("REJECT:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitNetworkMessage(tr("TERMINATE:Recv(UDP) from :")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}
	

	if (SenType==m_Reply_DELETEFILEA)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Delete file :")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitFilesMessage(tr("PASSED:Delete file :")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="DeleteFileA";
				INFORMATION_Query.msdnNote=tr("To perform this operation as a transacted operation, use the DeleteFileTransacted function.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Delete file :")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Delete file :")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Delete file :")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Delete file :")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Delete file :")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_DELETEFILEW)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Delete file :")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitFilesMessage(tr("PASSED:Delete file :")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="DeleteFileW";
				INFORMATION_Query.msdnNote=tr("To perform this operation as a transacted operation, use the DeleteFileTransacted function.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Delete file :")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Delete file :")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Delete file :")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Delete file :")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Delete file :")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_EXITWINDOWSEX)
	{	
		switch(SenQuery(sen,0,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Exit Windows"),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Exit Windows"),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="ExitWindowsEx";
				INFORMATION_Query.msdnNote=tr("Logs off the interactive user, shuts down the system, or shuts down and restarts the system.<br>\
It sends the WM_QUERYENDSESSION message to all applications to determine if they can be terminated.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Exit Windows");
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Exit Windows"),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Exit Windows"),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Exit Windows"),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Exit Windows"),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_TERMINATEPROCESS)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Terminate process:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Terminate process:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="TerminateProcess";
				INFORMATION_Query.msdnNote=tr("Terminates the specified process and all of its threads.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Terminate process:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Terminate process:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Terminate process:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Terminate process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Terminate process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_PROCESS32FIRSTW)
	{	
		switch(SenQuery(sen,0,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Enum processes(first)"),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Enum processes(first)"),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="Process32FirstW";
				INFORMATION_Query.msdnNote=tr("Retrieves information about the first process encountered in a system snapshot.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Enum processes(first)");
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Enum processes(first)"),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Enum processes(first)"),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Enum processes(first)"),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Enum processes(first)"),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_PROCESS32NEXTW)
	{	
		switch(SenQuery(sen,0,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Enum processes"),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Enum processes"),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="Process32NextW";
				INFORMATION_Query.msdnNote=tr("Retrieves information about the next process recorded in a system snapshot.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Enum processes");
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Enum processes"),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Enum processes"),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Enum processes"),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Enum processes"),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_OPENPROCESS)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Open process:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Open process:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="OpenProcess";
				INFORMATION_Query.msdnNote=tr("Opens an existing local process object.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Open process:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Open process:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Open process:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Open process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Open process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_LOADLIBRARYEXW)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Load library:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Load library:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="LoadLibraryexW";
				INFORMATION_Query.msdnNote=tr("Loads the specified module into the address space of the calling process.<br>\
The specified module may cause other modules to be loaded.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Load library:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Load library:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Load library:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Open process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Open process:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_CREATEWINDOWEXW)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Create Window:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Create Window:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="CreateWindowExW";
				INFORMATION_Query.msdnNote=tr("Creates an overlapped, pop-up, or child window with an extended window style;<br>\
otherwise, this function is identical to the CreateWindow function.<br>\
For more information about creating a window and for full descriptions of the other parameters of CreateWindowEx,<br>\
see CreateWindow.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Create Window:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Create Window:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Create Window:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Create Window:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Create Window:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SETWINDOWPOS)
	{	
		switch(SenQuery(sen,6,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="SetWindowPos";
				INFORMATION_Query.msdnNote=tr("Changes the size, position, and Z order of a child, pop-up, or top-level window.<br>\
These windows are ordered according to their appearance on the screen.<br>\
 The topmost window receives the highest rank and is the first window in the Z order.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6));

				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("REJECT:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("TERMINATE:Set Window Position:hwnd:%1 hwndAfter:%2 X:%3 Y:%4 cx:%5 cy:%6").\
					arg(SEN_STRING_LOCAL(1))\
					.arg(SEN_STRING_LOCAL(2))\
					.arg(SEN_STRING_LOCAL(3))\
					.arg(SEN_STRING_LOCAL(4))\
					.arg(SEN_STRING_LOCAL(5))\
					.arg(SEN_STRING_LOCAL(6)),\
					QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SETFILEATTRIBUTESA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitFilesMessage(tr("PASSED:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="SetFileAttributesA";
				INFORMATION_Query.msdnNote=tr("Sets the attributes for a file or directory.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Set File Attributes")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Set File Attributes")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SETFILEATTRIBUTESW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitFilesMessage(tr("PASSED:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="SetFileAttributesW";
				INFORMATION_Query.msdnNote=tr("Sets the attributes for a file or directory.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Set File Attributes")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Set File Attributes")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Set File Attributes:")+SEN_STRING_LOCAL(1)+tr(":")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_SETWINDOWTEXTA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="SetWindowTextA";
				INFORMATION_Query.msdnNote=tr("Changes the text of the specified window's title bar (if it has one). <br>\
If the specified window is a control, the text of the control is changed.<br>\
However, SetWindowText cannot change the text of a control in another application.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_SETWINDOWTEXTW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="SetWindowTextW";
				INFORMATION_Query.msdnNote=tr("Changes the text of the specified window's title bar (if it has one). <br>\
If the specified window is a control, the text of the control is changed.<br>\
 However, SetWindowText cannot change the text of a control in another application.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Set Window Text hwnd:")+SEN_STRING_LOCAL(1)+tr(" Text:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}
	
	if (SenType==m_Reply_MESSAGEBOXA)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="MessageBoxA";
				INFORMATION_Query.msdnNote=tr("Displays a modal dialog box that contains a system icon, a set of buttons,<br>\
and a brief application-specific message, such as status or error information.<br>\
The message box returns an integer value that indicates which button the user clicked.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_MESSAGEBOXW)
	{	
		switch(SenQuery(sen,2,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="MessageBoxW";
				INFORMATION_Query.msdnNote=tr("Displays a modal dialog box that contains a system icon, a set of buttons,<br>\
											  and a brief application-specific message, such as status or error information.<br>\
											  The message box returns an integer value that indicates which button the user clicked.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:Message Box Text:")+SEN_STRING_LOCAL(1)+tr(" Caption:")+SEN_STRING_LOCAL(2),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}



	if (SenType==m_Reply_WRITEPROCESSMEMORY)
	{	
		switch(SenQuery(sen,4,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Write Process Memory:")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="WriteProcessMemory";
				INFORMATION_Query.msdnNote=tr("Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("Write Process Memory:Target-")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:Write Process Memory:")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4),QColor(255,0,0));
				emit SIGNAL_EmitProcessesMessage(tr("REJECT:Write Process Memory:")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:Write Process Memory:")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4),QColor(255,0,0));
				emit SIGNAL_EmitProcessesMessage(tr("TERMINATE:Write Process Memory:")+SEN_STRING_LOCAL(2)+tr(" Address:")+SEN_STRING_LOCAL(3)+tr(" Size:")+SEN_STRING_LOCAL(4),QColor(255,0,0));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}


	if (SenType==m_Reply_WRITEFILE)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:WriteFile:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitFilesMessage(tr("PASSED:WriteFile:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="WriteFile";
				INFORMATION_Query.msdnNote=tr("Writes data to the specified file or input/output (I/O) device.<br>\
It reports its completion status asynchronously, calling the specified completion routine when writing is completed or canceled and the calling thread is in an alertable wait state.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("WriteFile:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("REJECT:WriteFile:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitFilesMessage(tr("REJECT:WriteFile:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("TERMINATE:WriteFile:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitFilesMessage(tr("TERMINATE:WriteFile:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}

	if (SenType==m_Reply_DEVICEIOCONTROL)
	{	
		switch(SenQuery(sen,1,bShow))
		{
		case QUERY_RETURN_PASS:
			apiAck.Exec=PEDOLL_EXECUTE_PASS;
			if(bShow) 
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(0,255,0));
			}
			break;
		case QUERY_RETURN_QUERY:
			{
				m_lastQueryESP=Query.esp;
				INFORMATION_Query.APIName="DeviceIOControl";
				INFORMATION_Query.msdnNote=tr("Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.");
				INFORMATION_Query.packetID=Query.PacketID;
				INFORMATION_Query.Behavior=tr("PASSED:Device IO:")+SEN_STRING_LOCAL(1);
				addQuery(INFORMATION_Query);
				emit SIGNAL_onQuery();
				return;
			}
			break;
		case QUERY_RETURN_REJECT:
			apiAck.Exec=PEDOLL_EXECUTE_REJECT;
			if(bShow)
			{
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(255,0,0));
			}
			break;
		case QUERY_RETURN_TERMINATE:
			if(bShow){
				emit SIGNAL_EmitMonitorMessage(tr("PASSED:PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
				emit SIGNAL_EmitWindowsOperatorMessage(tr("PASSED:PASSED:Device IO:")+SEN_STRING_LOCAL(1),QColor(128,128,255));
			}
			apiAck.Exec=PEDOLL_EXECUTE_TERMINATE;
			break;
		}
		goto _ACK_RETRUN;
	}
_ACK_RETRUN:
	GetDollNetWorkInterface()->Send((pt_byte *)&apiAck,sizeof apiAck);
}

QUERY_RETURN PeDollcCore::SenQuery(CubeGrammarSentence &sen,int MatchParamCount,bool &bShow)
{
	vector<int> m_MatchInt=FindFilterQueryString(sen.GetBlockString(0));
	QUERY_RETURN retType=QUERY_RETURN_PASS;
	bool HasFullMatch=false;
	bShow=true;
	if(m_MatchInt.size()==0)
	{
		return QUERY_RETURN_PASS;
	}

	for (unsigned int i=0;i<m_MatchInt.size();i++)
	{
		HasFullMatch=true;
		m_QueryLexer.SortText((char *)m_FunctionFilter[m_MatchInt[i]].c_str());
		m_QueryLexer.GetNextLexeme();

		for (int j=0;j<MatchParamCount;j++)
		{
			if(m_QueryLexer.GetNextLexeme()==CUBE_LEXER_LEXEME_TYPE_END)
			{
				HasFullMatch=false;
				break;
			}
			if (m_QueryLexer.GetCurrentLexeme()==CUBE_LEXER_LEXEME_TYPE_SPACER)
			{
				if(m_QueryLexer.GetNextLexeme()==CUBE_LEXER_LEXEME_TYPE_END)
				{
					HasFullMatch=false;
					break;
				}
			}
			if (m_QueryLexer.GetCurrentLexeme()!=CUBE_LEXER_LEXEME_TYPE_CONATINER)
			{
				HasFullMatch=false;
				break;
			}
			if (!QueryParamMatch(sen.GetBlockString(1+j),m_QueryLexer.GetIncludedString()))
			{
				HasFullMatch=false;
				break;
			}
		}

		if (!HasFullMatch)
		{
			continue;
		}
		retType=QUERY_RETURN_PASS;
		bShow=true;
		if(m_QueryLexer.GetNextLexeme()!=CUBE_LEXER_LEXEME_TYPE_END)
		{
			if (m_QueryLexer.GetCurrentLexeme()==CUBE_LEXER_LEXEME_TYPE_SPACER)
			{
				if(m_QueryLexer.GetNextLexeme()==CUBE_LEXER_LEXEME_TYPE_END)
				{
					return retType;
				}
			}
			if (strcmp(m_QueryLexer.GetLexemeString(),"REJECT")==0)
			{
				retType=QUERY_RETURN_REJECT;
			}
		
			if (strcmp(m_QueryLexer.GetLexemeString(),"QUERY")==0)
			{
				retType=QUERY_RETURN_QUERY;
			}

			if (strcmp(m_QueryLexer.GetLexemeString(),"TERMINATE")==0)
			{
				retType=QUERY_RETURN_TERMINATE;
			}

			if (m_QueryLexer.GetNextLexeme()!=CUBE_LEXER_LEXEME_TYPE_END)
			{
				if (m_QueryLexer.GetCurrentLexeme()==CUBE_LEXER_LEXEME_TYPE_SPACER)
				{
					if(m_QueryLexer.GetNextLexeme()==CUBE_LEXER_LEXEME_TYPE_END)
					{
						return retType;
					}
				}
				if (strcmp(m_QueryLexer.GetLexemeString(),"HIDE")==0)
				{
					bShow=false;
				}
			}
		}

	}
	return retType;
}

bool IsCharNumeric(char ch)
{
	return ch>='0'&&ch<='9';
}

bool IsNumeric(char *Mnemonic)
{

	bool Dot=false;
	if (Mnemonic==NULL)
	{
		return false;
	}

	if (strlen(Mnemonic)==0)
	{
		return false;
	}
	unsigned int CurrentCharIndex;

	if (!IsCharNumeric(Mnemonic[0])&&!(Mnemonic[0]==('-')))
	{
		if(Mnemonic[0]!='.')
			return false;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<strlen(Mnemonic);
		CurrentCharIndex++)
	{
		if (!IsCharNumeric(Mnemonic[CurrentCharIndex]))
		{
			if (Mnemonic[CurrentCharIndex]=='#')
			{
				return true;
			}
			if (Mnemonic[CurrentCharIndex]=='e'&&(Mnemonic[CurrentCharIndex+1]=='+'||Mnemonic[CurrentCharIndex+1]=='-'))
			{
				CurrentCharIndex++;
				continue;
			}

			if (Mnemonic[CurrentCharIndex]=='.')
			{
				if (Dot)
				{
					return false;
				}
				else
				{
					Dot=true;
					continue;
				}
			}
			return false;
		}
	}
	return true;
}

bool PeDollcCore::QueryParamMatch(char *strQueryParam,char *Match)
{
	if (Match==NULL||Match[0]=='\0')
	{
		return false;
	}
	
	if (strQueryParam==NULL)
	{
		return false;
	}
	if (Match[0]=='*'&&Match[1]=='\0')
	{
		return true;
	}

	if (Match[0]=='='||Match[0]=='>'||Match[0]=='<'||Match[0]=='&'||Match[0]=='|')
	{
		if (!IsNumeric(strQueryParam))
		{
			return false;
		}
		else
		{
			switch (Match[0])
			{
			case '=':
				return(atoi(strQueryParam)==atoi(Match+1));
				break;
			case '>':
				return(atoi(strQueryParam)>atoi(Match+1));
				break;
			case '<':
				return(atoi(strQueryParam)<atoi(Match+1));
				break;
			case '&':
				return(atoi(strQueryParam)&atoi(Match+1));
				break;
			case '!':
				return(atoi(strQueryParam)!=atoi(Match+1));
				break;
			}
		}
	}
	else
	{
		if(Match[0]=='*')
		return strstr(strQueryParam,Match+1)!=NULL;
		else
		{
			if (Match[0]=='\\')
			{
				Match=Match+1;
			}
			return !strcmp(strQueryParam,Match);
		}
	}
	return false;
}


vector<int> PeDollcCore::FindFilterQueryString(char *strQueryHeader)
{
	char Lexeme[256]={0};
	vector<int> m_vMatchInt;
	for (int i=0;i<m_FunctionFilter.size();i++)
	{
		int index=0;
		char *pFilterString=((char *)m_FunctionFilter[i].c_str());
		while(pFilterString[index]&&index<256&&pFilterString[index]!=' ')
		{
			Lexeme[index]=pFilterString[index];
			index++;
		}
		if (strcmp(strQueryHeader,strupr(Lexeme))==0)
		{
			m_vMatchInt.push_back(i);
		}
	}
	return m_vMatchInt;
}

void PeDollcCore::InitializeCommandGrammar()
{

	m_CommandLexer.RegisterSpacer(' ');
	m_CommandLexer.RegisterSpacer('\r');
	m_CommandLexer.RegisterSpacer('\n');
	unsigned int ContainerIndex=m_CommandLexer.RegisterContainer('<','>');

	m_CommandGrammar.SetLexer(&m_CommandLexer);

	CubeBlockType Contain=CubeBlockType(NULL,2,GRAMMAR_CONATINER,ContainerIndex);
	CubeBlockType Token=CubeBlockType(NULL,2,GRAMMAR_TOKEN);
	CubeBlockType Param=CubeBlockType(NULL,1,GRAMMAR_TYPE_ANY);
	CubeBlockType Spacer=CubeBlockType(NULL,1,GRAMMAR_SPACER);
	CubeBlockType Number=CubeBlockType("[0-9]+",2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_NUMBER);
	Number.AsRegex();

	m_CommandGrammar.RegisterBlockType(Contain);
	m_CommandGrammar.RegisterBlockType(Number);
	m_CommandGrammar.RegisterBlockType(Param);
	m_CommandGrammar.RegisterDiscard(Spacer);

	CubeBlockType cbt_doll=CubeBlockType(PEDOLL_CONTROLLER_CMD_DOLL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_DOLL);
	m_CommandGrammar.RegisterBlockType(cbt_doll);

	CubeBlockType cbt_dollshell=CubeBlockType(PEDOLL_CONTROLLER_CMD_SHELL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_SHELL);
	m_CommandGrammar.RegisterBlockType(cbt_dollshell);

	CubeBlockType cbt_enumprocess=CubeBlockType(PEDOLL_CONTROLLER_CMD_ENUMPROCESSES,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_ENUMPROCESSES);
	m_CommandGrammar.RegisterBlockType(cbt_enumprocess);

	CubeBlockType cbt_ls=CubeBlockType(PEDOLL_CONTROLLER_CMD_LS,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_LS);
	m_CommandGrammar.RegisterBlockType(cbt_ls);

	CubeBlockType cbt_kill=CubeBlockType(PEDOLL_CONTROLLER_CMD_KILL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_KILL);
	m_CommandGrammar.RegisterBlockType(cbt_kill);

	//
	CubeBlockType cbt_hook=CubeBlockType(PEDOLL_DOOL_CMD_HOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_HOOK);
	m_CommandGrammar.RegisterBlockType(cbt_hook);

	CubeBlockType cbt_unhook=CubeBlockType(PEDOLL_DOOL_CMD_UNHOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_UNHOOK);
	m_CommandGrammar.RegisterBlockType(cbt_unhook);

	CubeBlockType cbt_binary=CubeBlockType(PEDOLL_DOLL_CMD_HOOK_BIN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_BINARY);
	m_CommandGrammar.RegisterBlockType(cbt_binary);

	CubeBlockType cbt_unbinary=CubeBlockType(PEDOLL_DOLL_CMD_HOOK_UNBIN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_UNBINARY);
	m_CommandGrammar.RegisterBlockType(cbt_unbinary);

	CubeGrammarSentence Sen;
	Sen.Reset();
	Sen.add(cbt_doll);
	Sen.add(Param);
	Sen.add(Contain);
	m_instr_doll=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_dollshell);
	Sen.add(Contain);
	m_instr_shell=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_enumprocess);
	m_instr_EnumProcess=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_ls);
	Sen.add(Contain);
	m_instr_ls=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_kill);
	Sen.add(Contain);
	m_instr_kill=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_hook);
	Sen.add(Param);
	m_Instr_hook=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_unhook);
	Sen.add(Param);
	m_Instr_unhook=m_CommandGrammar.RegisterSentence(Sen);
	Sen.Reset();

	Sen.add(cbt_binary);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	m_Instr_binary=m_CommandGrammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_unbinary);
	m_Instr_unbinary=m_CommandGrammar.RegisterSentence(Sen);
}

void PeDollcCore::InitializeQueryGrammar()
{
	
	int TokenIndex=1;
	CubeGrammarSentence Sen;
	m_FilterLexer.RegisterSpacer(' ');
	m_FilterLexer.RegisterSpacer('\r');
	m_FilterLexer.RegisterSpacer('\n');
	m_FilterLexer.RegisterContainer('<','>');

	m_QueryLexer.RegisterSpacer(' ');
	m_QueryLexer.RegisterSpacer('\r');
	m_QueryLexer.RegisterSpacer('\n');

	unsigned int ContainerIndex=m_QueryLexer.RegisterContainer('<','>');

	m_QueryGrammar.SetLexer(&m_QueryLexer);

	CubeBlockType Contain=CubeBlockType(NULL,2,GRAMMAR_CONATINER,ContainerIndex);
	CubeBlockType Token=CubeBlockType(NULL,2,GRAMMAR_TOKEN);
	CubeBlockType Param=CubeBlockType(NULL,1,GRAMMAR_TYPE_ANY);
	CubeBlockType Spacer=CubeBlockType(NULL,1,GRAMMAR_SPACER);
	CubeBlockType Number=CubeBlockType("[0-9]+",2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_NUMBER);
	Number.AsRegex();

	m_QueryGrammar.RegisterBlockType(Contain);
	m_QueryGrammar.RegisterBlockType(Number);
	m_QueryGrammar.RegisterBlockType(Param);
	m_QueryGrammar.RegisterDiscard(Spacer);

	int __i;
	#define REG_QUERY_CBT(x)		CubeBlockType cbt_##x=CubeBlockType(PEDOLL_QUERY_##x,2,GRAMMAR_TOKEN,TokenIndex++);m_QueryGrammar.RegisterBlockType(cbt_##x);
	#define REG_QUERY_SENTENCE(z,t) Sen.Reset();Sen.add(cbt_##z);__i=t;while(__i--)Sen.add(Contain);m_Reply_##z=m_QueryGrammar.RegisterSentence(Sen);

	
	REG_QUERY_CBT(REGOPENKEYEXA)
	REG_QUERY_CBT(REGSETKEYVALUEA)
	REG_QUERY_CBT(REGSETVALUEEXW)
	REG_QUERY_CBT(REGSETVALUEEXA)
	REG_QUERY_CBT(REGSETVALUEA)
	REG_QUERY_CBT(REGCREATEKEYA)
	REG_QUERY_CBT(FINDWINDOWEXA)
	REG_QUERY_CBT(FINDWINDOWEXW)
	REG_QUERY_CBT(REGCREATEKEYEXW)
	REG_QUERY_CBT(REGCREATEKEYEXA)
	REG_QUERY_CBT(CREATEFILEA)
	REG_QUERY_CBT(CREATEFILEW)
	REG_QUERY_CBT(CREATEPROCESSINTERNALW)
	REG_QUERY_CBT(CREATEPROCESSW)
	REG_QUERY_CBT(CREATEPROCESSA)
	REG_QUERY_CBT(CONNECT)
	REG_QUERY_CBT(SEND)
	REG_QUERY_CBT(SENDTO)
	REG_QUERY_CBT(RECV)
	REG_QUERY_CBT(RECVFROM)
	REG_QUERY_CBT(DELETEFILEA)
	REG_QUERY_CBT(DELETEFILEW)
	REG_QUERY_CBT(EXITWINDOWSEX)
	REG_QUERY_CBT(TERMINATEPROCESS)
	REG_QUERY_CBT(PROCESS32FIRSTW)
	REG_QUERY_CBT(PROCESS32NEXTW)
	REG_QUERY_CBT(OPENPROCESS)
	REG_QUERY_CBT(LOADLIBRARYEXW)
	REG_QUERY_CBT(CREATEWINDOWEXW)
	REG_QUERY_CBT(SETWINDOWPOS)
	REG_QUERY_CBT(SETFILEATTRIBUTESA)
	REG_QUERY_CBT(SETFILEATTRIBUTESW)
	REG_QUERY_CBT(SETWINDOWTEXTA)
	REG_QUERY_CBT(SETWINDOWTEXTW)
	REG_QUERY_CBT(WRITEPROCESSMEMORY)
	REG_QUERY_CBT(MESSAGEBOXA)
	REG_QUERY_CBT(MESSAGEBOXW)
	REG_QUERY_CBT(WRITEFILE)
	REG_QUERY_CBT(DEVICEIOCONTROL)


	REG_QUERY_SENTENCE(REGOPENKEYEXA,2)
	REG_QUERY_SENTENCE(REGSETKEYVALUEA,4)
	REG_QUERY_SENTENCE(REGSETVALUEEXW,4)
	REG_QUERY_SENTENCE(REGSETVALUEEXA,2)
	REG_QUERY_SENTENCE(REGSETVALUEA,3)
	REG_QUERY_SENTENCE(REGCREATEKEYA,2)
	REG_QUERY_SENTENCE(FINDWINDOWEXA,2)
	REG_QUERY_SENTENCE(FINDWINDOWEXW,2)
	REG_QUERY_SENTENCE(REGCREATEKEYEXW,2)
	REG_QUERY_SENTENCE(REGCREATEKEYEXA,2)
	REG_QUERY_SENTENCE(CREATEFILEA,2)
	REG_QUERY_SENTENCE(CREATEFILEW,2)
	REG_QUERY_SENTENCE(CREATEPROCESSINTERNALW,2)
	REG_QUERY_SENTENCE(CREATEPROCESSW,2)
	REG_QUERY_SENTENCE(CREATEPROCESSA,2)
	REG_QUERY_SENTENCE(CONNECT,2)
	REG_QUERY_SENTENCE(SEND,2)
	REG_QUERY_SENTENCE(SENDTO,2)
	REG_QUERY_SENTENCE(RECV,2)
	REG_QUERY_SENTENCE(RECVFROM,2)
	REG_QUERY_SENTENCE(DELETEFILEA,1)
	REG_QUERY_SENTENCE(DELETEFILEW,1)
	REG_QUERY_SENTENCE(EXITWINDOWSEX,0)
	REG_QUERY_SENTENCE(TERMINATEPROCESS,1)
	REG_QUERY_SENTENCE(PROCESS32FIRSTW,0)
	REG_QUERY_SENTENCE(PROCESS32NEXTW,0)
	REG_QUERY_SENTENCE(OPENPROCESS,1)
	REG_QUERY_SENTENCE(LOADLIBRARYEXW,1)
	REG_QUERY_SENTENCE(CREATEWINDOWEXW,1)
	REG_QUERY_SENTENCE(SETWINDOWPOS,6)
	REG_QUERY_SENTENCE(SETFILEATTRIBUTESA,2)
	REG_QUERY_SENTENCE(SETFILEATTRIBUTESW,2)
	REG_QUERY_SENTENCE(SETWINDOWTEXTA,2)
	REG_QUERY_SENTENCE(SETWINDOWTEXTW,2)
	REG_QUERY_SENTENCE(MESSAGEBOXA,2)
	REG_QUERY_SENTENCE(MESSAGEBOXW,2)
	REG_QUERY_SENTENCE(WRITEFILE,1)
	REG_QUERY_SENTENCE(WRITEPROCESSMEMORY,4)
	REG_QUERY_SENTENCE(DEVICEIOCONTROL,1)
}

void PeDollcCore::RemoveDumpPack(unsigned int index)
{
	if(index>=m_DumpPacket.size()) return;
	CubeEnterCriticalSection(&m_DumpCS);
	delete [] m_DumpPacket[index].Dump;
	m_DumpPacket.erase(m_DumpPacket.begin()+index);
	CubeLeaveCriticalSection(&m_DumpCS);
	emit SIGNAL_EmitDumpTableUpdate();
}

void PeDollcCore::ClearDumpPack()
{
	if(0==m_DumpPacket.size()) return;
	CubeEnterCriticalSection(&m_DumpCS);
	for(int i=0;i<m_DumpPacket.size();i++)
	delete [] m_DumpPacket[i].Dump;
	m_DumpPacket.clear();
	CubeLeaveCriticalSection(&m_DumpCS);
	emit SIGNAL_EmitDumpTableUpdate();
}

void PeDollcCore::AddFunctionFilter(string st)
{
	m_FunctionFilter.push_back(st);
}

void PeDollcCore::ClearFunctionFilter()
{
	m_FunctionFilter.clear();
}


BOOL PeDollcCore::ReadStackMemory(DWORD address,int size)
{
	return m_DollNetWork.GetAtomBuffer(address,size);
}


int PeDollcCore::ReadAtomMemory(char buffer[PEDOLL_MEMORY_LEN],DWORD address,int size)
{
	if(m_DollNetWork.GetAtomBuffer(address,size))
	{
		memcpy(buffer,m_LastReturnBuffer.Buffer,m_LastReturnBuffer.len);
		return m_LastReturnBuffer.len==size;
	}
	return 0;
}

BOOL PeDollcCore::ReadMemory(DWORD address,int size)
{
	PEDOLL_MEMORYDUMP_INFO info;
	m_memSerial.clear();
	int retryTimes=10;
	int resSize=size;
	while (resSize)
	{
		if (resSize>=PEDOLL_DUMP_MEMORY_LEN)
		{
			if (m_DollNetWork.GetAtomBuffer(address+(size-resSize),PEDOLL_DUMP_MEMORY_LEN))
			{
				memcpy(info.buffer,m_LastReturnBuffer.Buffer,PEDOLL_DUMP_MEMORY_LEN);
				info.size=PEDOLL_DUMP_MEMORY_LEN;
				m_memSerial.push_back(info);
			}
			else
			{
				if (retryTimes--)
				{
					continue;
				}
				return FALSE;
			}
			resSize-=PEDOLL_DUMP_MEMORY_LEN;
		}
		else
		{
			if (m_DollNetWork.GetAtomBuffer(address+(size-resSize),resSize))
			{
				memcpy(info.buffer,m_LastReturnBuffer.Buffer,resSize);
				info.size=resSize;
				m_memSerial.push_back(info);
			}
			else
			{
				if (retryTimes--)
				{
					continue;
				}
				return FALSE;
			}
			resSize=0;
		}
		
	}
	return TRUE;
}

BOOL PeDollcCore::UpdateStack(unsigned int dfesp)
{
	m_StackInfo.clear();
	unsigned int esp;
	if (dfesp==0)
	{
		esp=m_lastQueryESP;
	}
	else
	{
		esp=dfesp;
	}
	m_UpdateESP=esp;
	if (esp==0)
	{
		return FALSE;
	}

	if(!ReadStackMemory(esp,PEDOLL_DEFAULT_STACKCOUNT*sizeof(DWORD)))
		return FALSE;

	
	memcpy(m_dwordstack,m_LastReturnBuffer.Buffer,sizeof m_dwordstack);

	PEDOLL_STACK_INFO info;
	for (int i=0;i<PEDOLL_DEFAULT_STACKCOUNT;i++)
	{
		info.address=esp+i*sizeof(DWORD);
		info.dwordVal=m_dwordstack[i];

		if(info.dwordVal!=0&&info.dwordVal!=(DWORD)-1)
		{
			if (!ReadStackMemory(m_dwordstack[i],PEDOLL_STACK_MEMORY_LEN))
			{
				m_StackInfo.clear();
				return FALSE;
			}
			memcpy(info.StringPointer,m_LastReturnBuffer.Buffer,PEDOLL_STACK_MEMORY_LEN);
			m_StackInfo.push_back(info);
		}
		else
		{
			info.StringPointer[0]=0;
			m_StackInfo.push_back(info);
		}
	}
	return TRUE;
}

BOOL PeDollcCore::ConnectToController()
{
	return m_ControllerNetWork.ConnectServer(m_Host.toStdString().c_str());
}

BOOL PeDollcCore::ConnectToDoll()
{
	return m_DollNetWork.ConnectServer(m_Host.toStdString().c_str());
}

BOOL PEDollc_NetWork::ConnectServer(const char *Host)
{
	if (Host==NULL)
	{
		return FALSE;
	}
	return Connect(Host,m_Port,1);
}


void PEDollc_NetWork::SetCore(PeDollcCore *core)
{
	m_pCore=core;
}

void PEDollc_NetWork::OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size)
{
	if(Size==sizeof(PEDOLL_ACK_PACKET))
	{
		PEDOLL_ACK_PACKET *pack=(PEDOLL_ACK_PACKET *)Buffer;
		if (pack->MagicNumber==PEDOLL_ACK_MAGICNUMBER)
		{
			SetEvent(m_CommandAckHandle);
			m_LastReturn=pack->Return;
		}
	}

	if (Size==sizeof(PEDOLL_MEMORY_ACK_PACKET))
	{
		PEDOLL_MEMORY_ACK_PACKET *pack=(PEDOLL_MEMORY_ACK_PACKET *)Buffer;
		if (pack->MagicNumber==PEDOLL_ACK_MEMORY)
		{
			SetEvent(m_CommandAckHandle);
			m_LastReturn=pack->size;
		}
	}
	
	if (m_IsDoll)
	{
		m_pCore->OnDollReceive(Buffer,Size);
	}
	else
	{
		if (m_IsController)
		{
			m_pCore->OnControllerReceive(Buffer,Size);
		}
	}
}

void PEDollc_NetWork::OnDisconnected(IN pt_int flag)
{
	if (m_IsDoll)
	{
		m_pCore->OnDollDisconnect();
	}
	else
	{
		if (m_IsController)
		{
			m_pCore->OnControllerDisconnect();  
		}
	}
}




BOOL PEDollc_NetWork::SendCommand(char*cmd,unsigned int *ret)
{
	if (m_IsConnecting==false)
	{
		return FALSE;
	}
	PEDOLL_COMMAND_PACKET pack;
	strcpy(pack.Info,cmd);
	if (m_CommandAckHandle!=INVALID_HANDLE_VALUE)
	{
		ResetEvent(m_CommandAckHandle);
	}
	if(!Send((pt_byte *)&pack,sizeof pack))
	{
		Disconnect();
	}
	if (m_CommandAckHandle!=INVALID_HANDLE_VALUE)
	{  
		if(WaitForSingleObject(m_CommandAckHandle,500)!=WAIT_OBJECT_0)
			return FALSE;
		if(ret)
		*ret=m_LastReturn;
		return TRUE;
	}
	return FALSE;
}

BOOL PEDollc_NetWork::GetAtomBuffer(unsigned int address,int size)
{
	if (m_IsConnecting==false)
	{
		return FALSE;
	}
	PEDOLL_COMMAND_PACKET pack;
	sprintf(pack.Info,"%s %u %u",PEDOLL_DOLL_CMD_READ_MEMORY_STR,address,size);
	if (m_CommandAckHandle!=INVALID_HANDLE_VALUE)
	{
		ResetEvent(m_CommandAckHandle);
	}
	Send((pt_byte *)&pack,sizeof pack);
	if (m_CommandAckHandle!=INVALID_HANDLE_VALUE)
	{  
		if(WaitForSingleObject(m_CommandAckHandle,200)!=WAIT_OBJECT_0)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}
