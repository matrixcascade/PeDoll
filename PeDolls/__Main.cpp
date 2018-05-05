#include "../PainterEngineSubPacket/network/inc/Cube_TCPFastIO.h"
#include "../PeStructure/inc/PEStructure.h"
#include "../protocol.h"
#include "../Grammar/Cube_Grammar.h"

#include "resource.h"
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
using namespace std;

#define MESSAGEMAX_LEN 1024
extern "C"
{
	//////////////////////////////////////////////////////////////////////////
	//inject codes
	void LoadLibaryInjectCodes(DWORD *StartAddr, DWORD *EndAddr);
}
///////////////////////////////////////////////////////////////////////////
//Global various
//////////////////////////////////////////////////////////////////////////
HANDLE g_MainThreadHandle;
HANDLE g_ExecuteHandle;
char   g_Message[MESSAGEMAX_LEN];
string g_vCmdString;
#define  REMOTESHELL_PROCESS_GUID ("14B599DD-E9D3-4302-BF5D-4228B15441515")
HANDLE G_hMutex;

BOOL IsAlreadyRunning()
{
	G_hMutex = ::CreateMutex(NULL,TRUE,REMOTESHELL_PROCESS_GUID);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(G_hMutex);
		return TRUE;
	}
	return FALSE;
}

HANDLE GetProcessHandleByName(string Name)
{
	PROCESSENTRY32 procEntry = { 0 };
	strcpy(procEntry.szExeFile,"");
	HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(procSnap == INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(procSnap,&procEntry);
	while(bRet)
	{
		if (procEntry.szExeFile==Name)
		{
			CloseHandle(procSnap);
			return OpenProcess( PROCESS_TERMINATE, FALSE, procEntry.th32ProcessID);
		}
		bRet = Process32Next(procSnap,&procEntry);
	}

	CloseHandle(procSnap);
	return INVALID_HANDLE_VALUE;
}
typedef BOOL (WINAPI* Proc_CreateProcess)(LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);

typedef HMODULE (WINAPI* Func_LoadLibraryW)(LPCWSTR lpLibFileName);


BYTE* mov_eax_xx(BYTE* lpCurAddres, DWORD eax)
{
	*lpCurAddres = 0xB8;
	*(DWORD*)(lpCurAddres + 1) = eax;
	return lpCurAddres + 5;
}

BYTE* mov_ebx_xx(BYTE* lpCurAddres, DWORD ebx)
{
	*lpCurAddres = 0xBB;
	*(DWORD*)(lpCurAddres + 1) = ebx;
	return lpCurAddres + 5;
}

BYTE* mov_ecx_xx(BYTE* lpCurAddres, DWORD ecx)
{
	*lpCurAddres = 0xB9;
	*(DWORD*)(lpCurAddres + 1) = ecx;
	return lpCurAddres + 5;
}

BYTE* mov_edx_xx(BYTE* lpCurAddres, DWORD edx)
{
	*lpCurAddres = 0xBA;
	*(DWORD*)(lpCurAddres + 1) = edx;
	return lpCurAddres + 5;
}

BYTE* mov_esi_xx(BYTE* lpCurAddres, DWORD esi)
{
	*lpCurAddres = 0xBE;
	*(DWORD*)(lpCurAddres + 1) = esi;
	return lpCurAddres + 5;
}

BYTE* mov_edi_xx(BYTE* lpCurAddres, DWORD edi)
{
	*lpCurAddres = 0xBF;
	*(DWORD*)(lpCurAddres + 1) = edi;
	return lpCurAddres + 5;
}

BYTE* mov_ebp_xx(BYTE* lpCurAddres, DWORD ebp)
{
	*lpCurAddres = 0xBD;
	*(DWORD*)(lpCurAddres + 1) = ebp;
	return lpCurAddres + 5;
}

BYTE* mov_esp_xx(BYTE* lpCurAddres, DWORD esp)
{
	*lpCurAddres = 0xBC;
	*(DWORD*)(lpCurAddres + 1) = esp;
	return lpCurAddres + 5;
}

BYTE* mov_eip_xx(BYTE* lpCurAddres, DWORD eip, DWORD newEip)
{
	if ( !newEip )
	{
		newEip = (DWORD)lpCurAddres;
	}

	*lpCurAddres = 0xE9;
	*(DWORD*)(lpCurAddres + 1) = eip - (newEip + 5);
	return lpCurAddres + 5;
}

BYTE* push_xx(BYTE* lpCurAddres, DWORD dwAdress)
{

	*lpCurAddres = 0x68;
	*(DWORD*)(lpCurAddres + 1) = dwAdress;

	return lpCurAddres + 5;
}

BYTE* Call_xx(BYTE* lpCurAddres, DWORD eip, DWORD newEip)
{
	if ( !newEip )
	{
		newEip = (DWORD)lpCurAddres;
	}

	*lpCurAddres = 0xE8;
	*(DWORD*)(lpCurAddres + 1) = eip - (newEip + 5);
	return lpCurAddres + 5;
}

BOOL SuspendTidAndInjectCode(HANDLE hProcess, HANDLE hThread, DWORD dwFuncAdress, const BYTE * lpShellCode, size_t uCodeSize)
{
	SIZE_T NumberOfBytesWritten = 0;
	BYTE ShellCodeBuf[0x480];
	CONTEXT Context;
	DWORD flOldProtect = 0;
	LPBYTE lpCurESPAddress = NULL;
	LPBYTE lpCurBufAdress = NULL;
	BOOL bResult = FALSE;

	SuspendThread(hThread);

	memset(&Context,0,sizeof(Context));
	Context.ContextFlags = CONTEXT_FULL;

	if ( GetThreadContext(hThread, &Context))
	{
		lpCurESPAddress = (LPBYTE)((Context.Esp - 0x480) & 0xFFFFFFE0);

		lpCurBufAdress = &ShellCodeBuf[0];

		if (lpShellCode)
		{
			memcpy(ShellCodeBuf + 128, lpShellCode, uCodeSize);
			lpCurBufAdress = push_xx(lpCurBufAdress, (DWORD)lpCurESPAddress + 128); // push
			lpCurBufAdress = Call_xx(lpCurBufAdress, dwFuncAdress, (DWORD)lpCurESPAddress + (DWORD)lpCurBufAdress - (DWORD)&ShellCodeBuf); //Call
		}

		lpCurBufAdress = mov_eax_xx(lpCurBufAdress, Context.Eax);
		lpCurBufAdress = mov_ebx_xx(lpCurBufAdress, Context.Ebx);
		lpCurBufAdress = mov_ecx_xx(lpCurBufAdress, Context.Ecx);
		lpCurBufAdress = mov_edx_xx(lpCurBufAdress, Context.Edx);
		lpCurBufAdress = mov_esi_xx(lpCurBufAdress, Context.Esi);
		lpCurBufAdress = mov_edi_xx(lpCurBufAdress, Context.Edi);
		lpCurBufAdress = mov_ebp_xx(lpCurBufAdress, Context.Ebp);
		lpCurBufAdress = mov_esp_xx(lpCurBufAdress, Context.Esp);
		lpCurBufAdress = mov_eip_xx(lpCurBufAdress, Context.Eip, (DWORD)lpCurESPAddress + (DWORD)lpCurBufAdress - (DWORD)&ShellCodeBuf);
		Context.Esp = (DWORD)(lpCurESPAddress - 4);
		Context.Eip = (DWORD)lpCurESPAddress;

		if ( VirtualProtectEx(hProcess, lpCurESPAddress, 0x480, PAGE_EXECUTE_READWRITE, &flOldProtect)
			&& WriteProcessMemory(hProcess, lpCurESPAddress, &ShellCodeBuf, 0x480, &NumberOfBytesWritten)
			&& FlushInstructionCache(hProcess, lpCurESPAddress, 0x480)
			&& SetThreadContext(hThread, &Context) )
		{
			bResult = TRUE;
		}

	}

	ResumeThread(hThread);

	return TRUE;
}

DWORD GetFuncAdress()
{
	return (DWORD)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA");
}

BOOL WINAPI CreateProcessWithDllA(  LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	LPWSTR lpDllFullPath,
	Proc_CreateProcess FuncAdress
	)
{
	BOOL bResult = FALSE;
	size_t uCodeSize = 0;
	DWORD dwCreaFlags;
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );

	if (FuncAdress == NULL)
	{
		FuncAdress = CreateProcessA;
	}

	dwCreaFlags = dwCreationFlags | CREATE_SUSPENDED;
	if (CreateProcessA(lpApplicationName, 
		lpCommandLine, 
		lpProcessAttributes, 
		lpThreadAttributes, 
		bInheritHandles, 
		dwCreaFlags, 
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		&pi
		))
	{
		if ( lpDllFullPath )
			uCodeSize = 2 * wcslen(lpDllFullPath) + 2;
		else
			uCodeSize = 0;
		DWORD dwLoadDllProc = GetFuncAdress();

		if (SuspendTidAndInjectCode(pi.hProcess, pi.hThread, dwLoadDllProc, (BYTE*)lpDllFullPath, uCodeSize))
		{
			if ( lpProcessInformation )
				memcpy(lpProcessInformation, &pi, sizeof(PROCESS_INFORMATION));

			if ( !(dwCreationFlags & CREATE_SUSPENDED) )
				ResumeThread(pi.hThread);

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL Setup()
{
	char CurrentPath[MAX_PATH];
	char DLL_Path[MAX_PATH];
	char DLL_CopyPath[MAX_PATH];

	GetModuleFileName(NULL,(LPSTR)CurrentPath,sizeof(CurrentPath));

	int len=strlen(CurrentPath);
	
	while (len>=0)
	{
		if (CurrentPath[len]=='\\'||CurrentPath[len]=='/')
		{
			CurrentPath[len]='\0';
			break;
		}
		len--;
	}
	if (len==-1)
	{
		CurrentPath[0]='\0';

	}
	sprintf(DLL_Path,"%s/PE.dll",CurrentPath);
	GetWindowsDirectory(DLL_CopyPath,MAX_PATH);
	strcat(DLL_CopyPath,"/PE.dll");

	if(GetFileAttributes(DLL_CopyPath)==INVALID_FILE_ATTRIBUTES)
	{
		if (GetFileAttributes(DLL_Path)==INVALID_FILE_ATTRIBUTES)
		{
			printf("pe.dll is lost,could not continue.\n");
			goto _ERR;
		}

		if (!CopyFile(DLL_Path,DLL_CopyPath,FALSE))
		{
			printf("PE.dll could not copy to %s,could not continue\n",DLL_CopyPath);
			goto _ERR;
		}
		if(GetFileAttributes(DLL_CopyPath)==INVALID_FILE_ATTRIBUTES)
		{
			printf("PE.dll could not copy to %s,could not continue\n",DLL_CopyPath);
			goto _ERR;
		}
	}
	return TRUE;
_ERR:
	return FALSE;
}


BYTE ShellCode[64]=
{
	0x60,
	0x9c,
	0x68,
	0xaa,0xbb,0xcc,0xdd,//dll path
	0xff,0x15,
	0xdd,0xcc,0xbb,0xaa,//call [xxxx]
	0x9d,
	0x61,
	0xff,0x25,
	0xaa,0xbb,0xcc,0xdd,// jmp [xxxxx]
	0xaa,0xaa,0xaa,0xaa,// loadaddr
	0xaa,0xaa,0xaa,0xaa//  jmpaddr
};

class PEDollController_NetWork:public Cube_TCPFastIO_Server
{
public:
	PEDollController_NetWork()
	{
		m_stID=0;
		m_Isconnect=FALSE;

		m_SendACKHandle=CreateEvent(NULL,TRUE,FALSE,NULL);

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

		CubeBlockType cbt_doll=CubeBlockType(PEDOLL_CONTROLLER_CMD_DOLL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_DOLL);
		m_Grammar.RegisterBlockType(cbt_doll);

		CubeBlockType cbt_dollshell=CubeBlockType(PEDOLL_CONTROLLER_CMD_SHELL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_SHELL);
		m_Grammar.RegisterBlockType(cbt_dollshell);

		CubeBlockType cbt_enumprocess=CubeBlockType(PEDOLL_CONTROLLER_CMD_ENUMPROCESSES,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_ENUMPROCESSES);
		m_Grammar.RegisterBlockType(cbt_enumprocess);

		CubeBlockType cbt_ls=CubeBlockType(PEDOLL_CONTROLLER_CMD_LS,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_LS);
		m_Grammar.RegisterBlockType(cbt_ls);

		CubeBlockType cbt_kill=CubeBlockType(PEDOLL_CONTROLLER_CMD_KILL,2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_KILL);
		m_Grammar.RegisterBlockType(cbt_kill);

		CubeGrammarSentence Sen;
		Sen.Reset();
		Sen.add(cbt_doll);
		Sen.add(Param);
		Sen.add(Contain);
		m_instr_doll=m_Grammar.RegisterSentence(Sen);

		Sen.Reset();
		Sen.add(cbt_dollshell);
		Sen.add(Contain);
		m_instr_shell=m_Grammar.RegisterSentence(Sen);

		Sen.Reset();
		Sen.add(cbt_enumprocess);
		m_instr_EnumProcess=m_Grammar.RegisterSentence(Sen);

		Sen.Reset();
		Sen.add(cbt_ls);
		Sen.add(Contain);
		m_instr_ls=m_Grammar.RegisterSentence(Sen);

		Sen.Reset();
		Sen.add(cbt_kill);
		Sen.add(Contain);
		m_instr_kill=m_Grammar.RegisterSentence(Sen);

	}
	~PEDollController_NetWork(){}
	/*
	{
	00973689 >    60                PUSHAD
	0097368A      9C                PUSHFD
	0097368B      68 50369700       PUSH 00973650
	00973690      FF15 70369700     CALL DWORD PTR DS:[973670]
	00973696      9D                POPFD
	00973697      61                POPAD
	00973698    - FF25 30369700     JMP DWORD PTR DS:[973630]


	}
	*/
	void Print_LAN_IP_Addr()
	{
		static char host_name[MAX_PATH];
		gethostname(host_name, sizeof(host_name));
		hostent *host_entry = gethostbyname(host_name);

		for(int i = 0; host_entry != NULL && host_entry->h_addr_list[i] != NULL; ++i)
		{
			// define pszAddr to record IP
			// inet_ntoa: Convert an IP into an Internet standard dotted format string
			const char *pszAddr = inet_ntoa (*(struct in_addr *)host_entry->h_addr_list[i]);
			printf("%d ----[LAN]\t%s\n",i+1, pszAddr);
		}
	}

	BYTE *DllPath;
	BOOL StartHook(HANDLE hProcess,HANDLE hThread)
	{


		CONTEXT ctx;
		ctx.ContextFlags=CONTEXT_ALL;
		if (!GetThreadContext(hThread,&ctx))
		{
			SendMessage("GetThreadContext Error");
			return FALSE;
		}
		LPVOID LpAddr=VirtualAllocEx(hProcess,NULL,64,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
		if (LpAddr==NULL)
		{
			SendMessage("VirtualAlloc Error");
			return FALSE;
		}
		DWORD LoadDllAAddr=(DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"LoadLibraryA");
		if (LoadDllAAddr==NULL)
		{
			SendMessage("LoadDllAddr error");
			return FALSE;
		}
		//printf("eip=0x%08x\n",ctx.Eip);
		//printf("lpAddr=0x%08x\n",LpAddr);
		//printf("LoadLibraryAAddr=0x%08x\n",LoadDllAAddr);

		/////////////
		_asm mov esp,esp
		DllPath=ShellCode+29;
		strcpy((char*)DllPath,"PE.dll");//这里是要注入的DLL名字
		*(DWORD*)(ShellCode+3)=(DWORD)LpAddr+29;
		////////////////
		*(DWORD*)(ShellCode+21)=LoadDllAAddr;
		*(DWORD*)(ShellCode+9)=(DWORD)LpAddr+21;
		//////////////////////////////////
		*(DWORD*)(ShellCode+25)=ctx.Eip;
		*(DWORD*)(ShellCode+17)=(DWORD)LpAddr+25;
		////////////////////////////////////
		if (!WriteProcessMemory(hProcess,LpAddr,ShellCode,64,NULL))
		{
			SendMessage("Write process memory error");
			return FALSE;
		}
		ctx.Eip=(DWORD)LpAddr;
		if (!SetThreadContext(hThread,&ctx))
		{
			SendMessage("Thread context error");
			return FALSE;
		}
		return TRUE;
	};


	BOOL EnableDebugPriv() 
	{
		HANDLE   hToken; 
		LUID   sedebugnameValue; 
		TOKEN_PRIVILEGES   tkp;
		if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) 
		{ 
			SendMessage("Open Process Token Error");
			return   FALSE; 
		} 

		if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&sedebugnameValue)) 
		{ 
			CloseHandle(hToken); 
			return   FALSE; 
		} 
		tkp.PrivilegeCount   =   1; 
		tkp.Privileges[0].Luid   =   sedebugnameValue; 
		tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED; 

		if(!AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(tkp),NULL,NULL)) 
		{ 
			return   FALSE; 
		}   
		CloseHandle(hToken); 
		return TRUE;

	} 

	BOOL InjectStartupDLL(char *ExeName)
	{
		EnableDebugPriv();
		STARTUPINFO sti;
		PROCESS_INFORMATION proci;
		memset(&sti,0,sizeof(STARTUPINFO));
		memset(&proci,0,sizeof(PROCESS_INFORMATION));
		sti.cb=sizeof(STARTUPINFO);

		DWORD valc=CreateProcess(ExeName,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL\
			,&sti,&proci);
		if (valc==NULL)
		{
			SendMessage("Could not open target PE file");
			return FALSE;
		}
		if (!StartHook(proci.hProcess,proci.hThread))
		{
			SendMessage("Startup failde");
			TerminateProcess(proci.hProcess,0);
			return FALSE;
		}
		ResumeThread(proci.hThread);
		CloseHandle(proci.hThread);
		return TRUE;
	}

	BOOL InjectStartupDLLEx(char *wszPath)
	{
		WCHAR wszDll[] = L"PE.dll";

		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		return CreateProcessWithDllA(NULL, wszPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi, wszDll, NULL);
	}

	void OnConnected(IN SOCKET socket,IN SOCKADDR_IN	addrin,OUT pt_int &Flag)
	{
		m_cursocket=socket;
		m_Isconnect=TRUE;
		printf("PeDoll Controller is connected\n");
	};


	void OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size)
	{
		if (Size==sizeof(PEDOLL_ACK_PACKET))
		{
			if (PEDOLL_ACK_MAGICNUMBER==*((DWORD*)Buffer))
			{
				SetEvent(m_SendACKHandle);
				return;
			}
		}
		if (Size!=sizeof(PEDOLL_COMMAND_PACKET))
		{
			return;
		}

		PEDOLL_ACK_PACKET ackpack;
		Send(m_cursocket,(pt_byte *)&ackpack,sizeof(PEDOLL_ACK_PACKET));

		PEDOLL_COMMAND_PACKET *cmdPack=(PEDOLL_COMMAND_PACKET *)Buffer;

		if(cmdPack->MagicNumber==PEDOLL_CMD_MAGICNUMBER)
		{
			g_vCmdString=cmdPack->Info;
			SetEvent(g_ExecuteHandle);
		}
	};

	void OnDisconnected(IN pt_int flag)
	{
		printf("PeDoll Controller is disconnected\n");
	}

	void SendMessage(char *Message)
	{
		if (!m_Isconnect)
		{
			return;
		}
		PEDOLL_MESSAGE_PACKET  pack;
		strcpy(pack.Messages,Message);
		SendMessagePack(pack);
	}

	void SendMessagePack(PEDOLL_MESSAGE_PACKET  pack)
	{
		int Timeout=100;

		if (m_SendACKHandle!=INVALID_HANDLE_VALUE)
		{
			ResetEvent(m_SendACKHandle);
		}

		while(Send(m_cursocket,(pt_byte *)&pack,sizeof(PEDOLL_MESSAGE_PACKET))==FALSE)
		{
			Timeout--;
			Sleep(10);
			if (Timeout<=0)
			{
				return;
			}
		}
		if (m_SendACKHandle!=INVALID_HANDLE_VALUE)
		{
			
			WaitForSingleObject(m_SendACKHandle,100);
		}

	}

	void ExecuteCommand(char *command)
	{
		
		CubeGrammarSentence sen;
		m_Lexer.SortText(command);
		unsigned int SenType;
		SenType=m_Grammar.GetNextInstr(sen);
		if (SenType==GRAMMAR_SENTENCE_UNKNOW||SenType==GRAMMAR_SENTENCE_END)
		{
			return;
		}
		if (SenType==m_instr_doll)
		{
			int opCode;
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
					if(InjectImportTable(sen.GetBlockString(2)))
					{
						SendMessage("Import Table inject succeeded.");
					}
				}
				break;
			case PEDOLL_CONTROLLER_CMD_DOLL_BININJECT:
				{
					if(InjectStaticBinaryCode(sen.GetBlockString(2)))
					{
						SendMessage("Binary codes inject succeeded.");
					}
				}
				break;
			case PEDOLL_CONTROLLER_CMD_DOLL_DLLINJECT:
				{
					if(InjectDll(sen.GetBlockInteger(2),"PE.dll"))
					{
						SendMessage("DLL inject succeeded.");
					}
				}
				break;
			case PEDOLL_CONTROLLER_CMD_DOLL_DEBUG:
				{
					if(InjectStartupDLL(sen.GetBlockString(2)))
					{
						SendMessage("Process is activating for debug.");
					}
					else
					{
						SendMessage("Invalid debug.");
					}
				}
				break;
			case PEDOLL_CONTROLLER_CMD_DOLL_ACTIVATEINJECT:
				{
					if(InjectStartupDLLEx(sen.GetBlockString(2)))
					{
						SendMessage("Process is activating for Monitor.");
					}
					else
					{
						SendMessage("Invalid debug.");
					}
				}
				break;
			}
		}

		if (SenType==m_instr_shell)
		{
			if(ShellPE(sen.GetBlockString(1)))
			SendMessage("Shell has executed.");
			else
			SendMessage("Shell target is not existed.");
		}

		if (SenType==m_instr_EnumProcess)
		{
			EnumProcesses();
		}

		if (SenType==m_instr_ls)
		{
			EnumDirectory(sen.GetBlockString(1));
		}
		if (SenType==m_instr_kill)
		{
			HANDLE procHandle=GetProcessHandleByName(sen.GetBlockString(1));
			if (procHandle!=INVALID_HANDLE_VALUE)
			{
				if (TerminateProcess(procHandle,0))
				{
					SendMessage("Terminate succeeded.");
				}
				else
				{
					SendMessage("Terminate failed.");
				}
				CloseHandle(procHandle);
			}
			else
			{
				char Message[MAX_PATH];
				sprintf(Message,"Could not open target process:%s",sen.GetBlockString(1));
				SendMessage(Message);
			}

		}
	}
	BOOL ShellPE(char *FileName)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);

		if (GetFileAttributes(FileName)==INVALID_FILE_ATTRIBUTES)
		{
			return FALSE;
		}
		if(CreateProcess(FileName, "", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle( pi.hThread );
		}
		return TRUE;
	}

	BOOL InjectImportTable(char *FileName)
	{
		PeFile PEFile;
		if (!PEFile.Load_PE_File(FileName))
		{
			sprintf(g_Message,"Could not load target file,May be it not a win32 PE file.");
			SendMessage(g_Message);
			return FALSE;
		}
		sprintf(g_Message,"Load succeeded...analyze...");
		SendMessage(g_Message);
		sprintf(g_Message,"---------------------------PE information-------------------------");
		SendMessage(g_Message);
		sprintf(g_Message,"Got sections: %d",PEFile.GetSectionCount());
		SendMessage(g_Message);
		for (int i = 0; i < PEFile.GetSectionCount(); i++)
		{
		sprintf(g_Message,"Section:%s RVA:%08X FOA:%08X",PEFile.GetSectionName(i),PEFile.GetSectionRVA(i),PEFile.RVA_To_FOA(PEFile.GetSectionRVA(i)));
		SendMessage(g_Message);
		}

		for (int i=0;i<PEFile.GetImportTableCount();i++)
		{
			if (strcmp(strupr((char *)PEFile.GetImportTableName(i)),"PE.DLL")==0)
			{
				sprintf(g_Message,"code has been rebuild");
				SendMessage(g_Message);
				return TRUE;
				break;
			}
		}

		__IMAGE_IMPORT_BY_NAME xdummyName;
		xdummyName.Hint=-1;
		xdummyName.Name="X_dummy";
		IMAGE_IMPORT_TABLE_INFO info;
		info.ImportName="PE.DLL";
		info.ImportCount=1;
		info.ImportTable=&xdummyName;


		if(!PEFile.AddImportTables(&info,1))
		{
			sprintf(g_Message,"Err：Could not modify import table.");
			SendMessage(g_Message);
			return FALSE;
		}
		sprintf(g_Message,"transfer.....");
		SendMessage(g_Message);
		if (!PEFile.Dump(FileName))
		{
			sprintf(g_Message,"could not dump to target.");
			SendMessage(g_Message);
			return FALSE;
		}
		sprintf(g_Message,"rebuild succeeded");
		SendMessage(g_Message);
		return TRUE;

	}

	BOOL InjectStaticBinaryCode(char *FileName)
	{
		DWORD InjectBin_St, InjectBin_Ed,EP_SectionIndex;
		DWORD InjectBin_RVA;
		PeFile PEFile;
		int CodesSize;
		char  *NewBuffer=NULL;
		if (!PEFile.Load_PE_File(FileName))
		{
			sprintf(g_Message,"Could not load target file,May be it not a win32 PE file.");
			SendMessage(g_Message);
			goto _ERR;
		}
		sprintf(g_Message,"loading succeeded...analyze...");
		SendMessage(g_Message);
		if(PEFile.GetImageNtHeaderPointer()->OptionalHeader.CheckSum==0x12312312)
		{
			sprintf(g_Message,"Target has been rebuild.");
			SendMessage(g_Message);
			return TRUE;
		}
		sprintf(g_Message,"---------------------------PEInfo-------------------------");
		SendMessage(g_Message);
		if (PEFile.IsDLL())
		{
			sprintf(g_Message,"DLL is not support!");
			SendMessage(g_Message);
			goto _ERR;
		}

		sprintf(g_Message,"Got sections: %d",PEFile.GetSectionCount());
		SendMessage(g_Message);
		for (int i = 0; i < PEFile.GetSectionCount(); i++)
		{
			sprintf(g_Message,"Section:%s RVA:%08X FOA:%08X",PEFile.GetSectionName(i),PEFile.GetSectionRVA(i),PEFile.RVA_To_FOA(PEFile.GetSectionRVA(i)));
			SendMessage(g_Message);
		}


		sprintf(g_Message,"Got OEP:%08X FOA:%08X",PEFile.GetEntryPoint(),PEFile.RVA_To_FOA(PEFile.GetEntryPoint()));
		SendMessage(g_Message);

		LoadLibaryInjectCodes(&InjectBin_St, &InjectBin_Ed);
		CodesSize = InjectBin_Ed - InjectBin_St + 1;

		NewBuffer = new char[CodesSize];
		memcpy(NewBuffer, (void *)InjectBin_St, CodesSize);

		EP_SectionIndex=PEFile.GetSectionIndexByRVA(PEFile.GetEntryPoint());
		sprintf(g_Message,"OEP in section： \"%s\" -- %d bytes ",
			PEFile.GetSectionName(EP_SectionIndex),
			PEFile.GetSectionRawSize(EP_SectionIndex));
		SendMessage(g_Message);
		sprintf(g_Message,"redirect code section:%s Virtual size:%d RAW size:%d",PEFile.GetSectionName(EP_SectionIndex),PEFile.GetSectionSize(EP_SectionIndex),PEFile.GetSectionRawSize(EP_SectionIndex));
		SendMessage(g_Message);
		sprintf(g_Message,"spacer:%d byes;checking....!",PEFile.GetSectionRawSize(EP_SectionIndex)-PEFile.GetSectionSize(EP_SectionIndex));
		SendMessage(g_Message);
		//Get the size of code for inject
		for (int i=0;i<CodesSize;i++)
		{
			char *pBuffer=(char *)PEFile.GetSectionBufferPointer(EP_SectionIndex)+PEFile.GetSectionRawSize(EP_SectionIndex)-1;
			if (*(pBuffer-i)!=0)
			{
				sprintf(g_Message,"Verify Error");
				SendMessage(g_Message);
				goto _ERR;
			}
		}
		sprintf(g_Message,"verify OK");
		SendMessage(g_Message);
		//OEP PARAMETER
		InjectBin_RVA=PEFile.GetSectionRVA(EP_SectionIndex)+PEFile.GetSectionSize(EP_SectionIndex);

		DWORD *param =(DWORD *)(NewBuffer+CodesSize-8);
		*param = PEFile.GetEntryPoint();
		*(param + 1) = InjectBin_RVA;

		//Inject codes
		sprintf(g_Message,"Injecting...");
		SendMessage(g_Message);
		memcpy(PEFile.ImagePointer(PEFile.RVA_To_FOA(InjectBin_RVA)),NewBuffer,CodesSize);

		sprintf(g_Message,"redirect OEP %08X",InjectBin_RVA);
		SendMessage(g_Message);
		PEFile.SetEntryPoint(InjectBin_RVA);

		//set Flag

		PEFile.GetImageNtHeaderPointer()->OptionalHeader.CheckSum=0x12312312;

		sprintf(g_Message,"transfer.....");
		SendMessage(g_Message);
		if (!PEFile.Dump(FileName))
		{
			sprintf(g_Message,"can not dump to target file.");
			SendMessage(g_Message);
			return FALSE;
		}
		sprintf(g_Message,"rebuild succeeded");
		SendMessage(g_Message);
		if (NewBuffer)
		{
			delete [] NewBuffer;
		}
		return TRUE;
_ERR:
		if (NewBuffer)
		{
			delete [] NewBuffer;
		}
		return FALSE;
	};

	void EnumDirectory(char *dir)
	{
		char szFind[MAX_PATH] ={0};
		char lpPath[MAX_PATH]={0};
		char lpfnPath[MAX_PATH]={0};
		WIN32_FIND_DATA findFileData;
		BOOL bRet;


		strcpy(szFind,dir);
		if (szFind[strlen(szFind)-1]=='\\'||szFind[strlen(szFind)-1]=='/')
		{
			szFind[strlen(szFind)-1]='\0';
		}
		sprintf(lpfnPath,"%s\\",szFind);
		strcat(szFind, ("\\*.*"));  

		HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return;
		}

		while (TRUE)
		{
			if (findFileData.cFileName[0] != ('.'))
			{

				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					sprintf(lpPath,("<FILE>%s%s"), lpfnPath, findFileData.cFileName);
					SendMessage(lpPath);
				}
				else
				{
					sprintf(lpPath,("<DIR>%s%s"), lpfnPath, findFileData.cFileName);
					SendMessage(lpPath);
				}
			}
			bRet = ::FindNextFile(hFind, &findFileData);
			if (!bRet)
			{
				break;
			}
		}

		::FindClose(hFind);
	}

	void EnumProcesses()
	{
		#define PROG_SHOW_ONE_PAGE 16
		int ShowIndex=0;
		HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(procSnap == INVALID_HANDLE_VALUE)
		{
			return;
		}
		//
		PROCESSENTRY32 procEntry = { 0 };
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		BOOL bRet = Process32First(procSnap,&procEntry);
		while(bRet)
		{
			sprintf(g_Message,"PID: %d (%s) ",procEntry.th32ProcessID, procEntry.szExeFile); 
			SendMessage(g_Message);
			bRet = Process32Next(procSnap,&procEntry);
		}

		CloseHandle(procSnap);
	}


	BOOL InjectDll(DWORD dwPID, LPCSTR szDllPath) {  
		HANDLE hProcess = NULL;  
		HANDLE hThread = NULL;  
		HMODULE hMod = NULL;  
		LPVOID pRemoteBuf = NULL; 
		DWORD dwBufSize = (DWORD)(strlen(szDllPath) + 1) * sizeof(TCHAR);  
		LPTHREAD_START_ROUTINE pThreadProc;  

		if( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) ) {  
			printf("Could not open target process(%d) error: [%d]", dwPID, GetLastError());  
			return FALSE;  
		}  

		pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);  
		WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL); 

		hMod = GetModuleHandle("kernel32.dll");  
		pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");  

		hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf,0, NULL);  
		CloseHandle(hThread);  
		CloseHandle(hProcess);  

		return TRUE;  
	}  
	void  InjectActivateThread(DWORD PID)
	{
		if (!InjectDll(PID,"PE.DLL"))
		{
			sprintf(g_Message,"could not inject to target.");
			SendMessage(g_Message);
		}
	}
	HANDLE				m_SendACKHandle;
	BOOL				m_Isconnect;
	SOCKET              m_cursocket;
	unsigned int		m_stID;
	HANDLE				m_NetWorkConnectEvent;
	CubeGrammar			m_Grammar;
	CubeLexer			m_Lexer;
	int					m_instr_doll,m_instr_shell,m_instr_EnumProcess,m_instr_ls,m_instr_kill;
};

PEDollController_NetWork g_NetWork;
  

// IsInsideVPC's exception filter    
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)    
{    
	PCONTEXT ctx = ep->ContextRecord;    

	ctx->Ebx = -1; // Not running VPC    
	ctx->Eip += 4; // skip past the "call VPC" opcodes    
	return EXCEPTION_CONTINUE_EXECUTION; // we can safely resume execution since we skipped faulty instruction    
}    

// high level language friendly version of IsInsideVPC()    
bool IsInsideVPC()    
{    
	bool rc = false;    

	__try    
	{    
		_asm push ebx    
		_asm mov  ebx, 0 // Flag    
		_asm mov  eax, 1 // VPC function number    

		// call VPC     
		_asm __emit 0Fh    
		_asm __emit 3Fh    
		_asm __emit 07h    
		_asm __emit 0Bh    

		_asm test ebx, ebx    
		_asm setz [rc]    
		_asm pop ebx    
	}    
	// The except block shouldn't get triggered if VPC is running!!    
	__except(IsInsideVPC_exceptionFilter(GetExceptionInformation()))    
	{    
	}    

	return rc;    
}    

bool IsInsideVMWare()    
{    
	bool rc = true;    

	__try    
	{    
		__asm    
		{    
			push   edx    
				push   ecx    
				push   ebx    

				mov    eax, 'VMXh'    
				mov    ebx, 0 // any value but not the MAGIC VALUE    
				mov    ecx, 10 // get VMWare version    
				mov    edx, 'VX' // port number    

				in     eax, dx // read port    
				// on return EAX returns the VERSION    
				cmp    ebx, 'VMXh' // is it a reply from VMWare?    
				setz   [rc] // set return value    

			pop    ebx    
				pop    ecx    
				pop    edx    
		}    
	}    
	__except(EXCEPTION_EXECUTE_HANDLER)    
	{    
		rc = false;    
	}    

	return rc;    
}
bool IsVirtualMachine()    
{    
	if(IsInsideVPC())    
		return true;    

	if(IsInsideVMWare())    
		return true;    

	return false;    
}  
int main()
{
	if (IsAlreadyRunning())
	{
		return 0;
	}
	g_MainThreadHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_ExecuteHandle=CreateEvent(NULL,TRUE,FALSE,NULL);

	if (INVALID_HANDLE_VALUE==g_MainThreadHandle)
	{
		printf("Initialize failed(Could not create event).");
		return 0;
	}
	if(!Setup())
	{
			printf("Initialize failed(Could not setup PE.dll).\n");
			system("pause");
			return 0;
	}
	

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
		printf("========================================================================\n");
		printf("===================PeDoll Controller（Behavoir Monitor）================\n");
		printf("============DBinary (PaintEngine Sub Project) All rights reserved========\n");
		printf("========================================================================\n");

		if (!IsVirtualMachine())
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
			printf("WARNING：PeDoll should be run inside virtual machine!!\n");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
		}

		g_NetWork.Start(PEDOLL_CONTROLLER_NETWORK_PORT,1,1);
		g_NetWork.Print_LAN_IP_Addr();
		while (TRUE)
		{
			WaitForSingleObject(g_ExecuteHandle,INFINITE);
			g_NetWork.ExecuteCommand((char *)g_vCmdString.c_str());
			ResetEvent(g_ExecuteHandle);
		}

		WaitForSingleObject(g_MainThreadHandle,INFINITE);
		return 0;
}