#pragma once
//////////////////////////////////////////////////////////////////////////
// PE File Parser
// Code by DBinary
// Language : C++
// Code portability required:File operator in  C++ standard library 
// 2015-10-22
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <string>

#ifdef _MSC_VER
#include <Windows.h>
#endif

#ifdef __GNUC__
#endif

#define  IMPORT_TABLE_NAME_MAXLEN		256
#define  IMPORT_FUNCTION_NAME_MAXLEN	256
using namespace std;

#ifndef _WINDOWS_

typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef unsigned char  BYTE;

#define IMAGE_DIRECTORY_ENTRY_EXPORT    0

#define IMAGE_DIRECTORY_ENTRY_IMPORT    1

#define IMAGE_DIRECTORY_ENTRY_RESOURCE  2

#define IMAGE_DIRECTORY_ENTRY_EXCEPTION 3

#define IMAGE_DIRECTORY_ENTRY_SECURITY  4

#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5

#define IMAGE_DIRECTORY_ENTRY_DEBUG 6

#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT 7

#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE  7

#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 8

#define IMAGE_DIRECTORY_ENTRY_TLS   9

#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG   10

#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT  11

#define IMAGE_DIRECTORY_ENTRY_IAT   12

#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT  13

#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR    14

//
// Section characteristics.
//
//      IMAGE_SCN_TYPE_REG                   0x00000000  // Reserved.
//      IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
//      IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
//      IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
#define IMAGE_SCN_TYPE_NO_PAD                0x00000008  // Reserved.
//      IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.

#define IMAGE_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#define IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.
//                                           0x00002000  // Reserved.
//      IMAGE_SCN_MEM_PROTECTED - Obsolete   0x00004000
#define IMAGE_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define IMAGE_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
#define IMAGE_SCN_MEM_FARDATA                0x00008000
//      IMAGE_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
#define IMAGE_SCN_MEM_PURGEABLE              0x00020000
#define IMAGE_SCN_MEM_16BIT                  0x00020000
#define IMAGE_SCN_MEM_LOCKED                 0x00040000
#define IMAGE_SCN_MEM_PRELOAD                0x00080000

#define IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#define IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#define IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#define IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#define IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#define IMAGE_SCN_ALIGN_64BYTES              0x00700000  //
#define IMAGE_SCN_ALIGN_128BYTES             0x00800000  //
#define IMAGE_SCN_ALIGN_256BYTES             0x00900000  //
#define IMAGE_SCN_ALIGN_512BYTES             0x00A00000  //
#define IMAGE_SCN_ALIGN_1024BYTES            0x00B00000  //
#define IMAGE_SCN_ALIGN_2048BYTES            0x00C00000  //
#define IMAGE_SCN_ALIGN_4096BYTES            0x00D00000  //
#define IMAGE_SCN_ALIGN_8192BYTES            0x00E00000  //
// Unused                                    0x00F00000
#define IMAGE_SCN_ALIGN_MASK                 0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL            0x01000000  // Section contains extended relocations.
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define IMAGE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

struct IMAGE_DOS_HEADER  
{
	WORD			e_magic;			//Magic code "MZ"
	WORD			e_cblp;				//Size of last page
	WORD			e_cp;				//Count of page in file
	WORD		    e_crle;				//Pointer count of redirection table
	WORD			e_cparhdr;			//Header size(Segment)
	WORD			e_minalloc;			//Minimum additional sections be required 
	WORD			e_maxalloc;			//Maximum additional sections be required
	WORD			e_ss;				//SS(Relative offset)
	WORD			e_sp;				//SP(register)
	WORD			e_csum;				//Complement checksum
	WORD			e_ip;				//IP(register)
	WORD			e_cs;				//CS(register)
	WORD			e_lfarlc;			//Byte offset of redirection table
	WORD			e_ovno;				//Cover code
	WORD			e_res[4];			//Reserve
	WORD			e_oemid;			//OEM ID
	WORD			e_oeninfo;			//OEM informations;
	WORD			e_res2[10];			//Reserve2
	DWORD			e_lfanew;			//PE header offset address relative to the file
};

struct IMAGE_FILE_HEADER {
	WORD    Machine;					//MAC runtime code
	WORD    NumberOfSections;			//Number Of Sections
	DWORD   TimeDateStamp;				//Time Date Stamp
	DWORD   PointerToSymbolTable;		//Pointer To Symbol Table
	DWORD   NumberOfSymbols;			//Number Of Symbols
	WORD    SizeOfOptionalHeader;		//Size Of Optional Header
	WORD    Characteristics;			//Characteristics
};

struct IMAGE_DATA_DIRECTORY {
	DWORD   VirtualAddress;				//RVA
	DWORD   Size;						//Data size
};


struct IMAGE_OPTIONAL_HEADER {
	//
	// Standard fields.
	//
	WORD    Magic;				//107h as ROM Image,10Bh as exe Image
	BYTE    MajorLinkerVersion;
	BYTE    MinorLinkerVersion;
	DWORD   SizeOfCode;				//Size of all codes sections
	DWORD   SizeOfInitializedData;	
	DWORD   SizeOfUninitializedData;
	DWORD   AddressOfEntryPoint;	//EP (RVA)
	DWORD   BaseOfCode;				//RVA of code section 
	DWORD   BaseOfData;				//RVA of data section

	//
	// NT additional fields.
	//

	DWORD   ImageBase;
	DWORD   SectionAlignment;
	DWORD   FileAlignment;
	WORD    MajorOperatingSystemVersion;
	WORD    MinorOperatingSystemVersion;
	WORD    MajorImageVersion;
	WORD    MinorImageVersion;
	WORD    MajorSubsystemVersion;
	WORD    MinorSubsystemVersion;
	DWORD   Win32VersionValue;
	DWORD   SizeOfImage;
	DWORD   SizeOfHeaders;
	DWORD   CheckSum;				//check sum
	WORD    Subsystem;
	WORD    DllCharacteristics;
	DWORD   SizeOfStackReserve;
	DWORD   SizeOfStackCommit;
	DWORD   SizeOfHeapReserve;
	DWORD   SizeOfHeapCommit;
	DWORD   LoaderFlags;
	DWORD   NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[16];
	/* IMAGE_DATA_DIRECTORY Description
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	Index	Offset (PE/PE32+)							Description
	0		96/112								Export table address and size
	1		104/120								Import table address and size
	2		112/128								Resource table address and size
	3		120/136								Exception table address and size
	4		128/144								Certificate table address and size
	5		136/152								Base relocation table address and size
	6		144/160								Debugging information starting address and size
	7		152/168								Architecture-specific data address and size
	8		160/176								Global pointer register relative virtual address
	9		168/184								Thread local storage (TLS) table address and size
	10		176/192								Load configuration table address and size
	11		184/200								Bound import table address and size
	12		192/208								Import address table address and size(IAT)
	13		200/216								Delay import descriptor address and size
	14		208/224								The CLR header address and size
	15		216/232								Reserved
	/////////////////////////////////////////////////////////////////////////////////////////////////////
		*/
};

struct IMAGE_NT_HEADERS {
	DWORD Signature;				 //Must be 0x00004550
	IMAGE_FILE_HEADER FileHeader;	
	IMAGE_OPTIONAL_HEADER OptionalHeader;
};

struct IMAGE_SECTION_HEADER {
	BYTE    Name[8];
	union {
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc;
	DWORD   VirtualAddress;			//RVA
	DWORD   SizeOfRawData;			//Size after align
	DWORD   PointerToRawData;
	DWORD   PointerToRelocations;
	DWORD   PointerToLinenumbers;
	WORD    NumberOfRelocations;
	WORD    NumberOfLinenumbers;
	DWORD   Characteristics;
};

#define  DUMMYUNIONNAME
#define  DUMMYUNIONNAME2
struct IMAGE_IMPORT_DESCRIPTOR {
	union {
		DWORD   Characteristics;            // 0 for terminating null import descriptor
		DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	} DUMMYUNIONNAME;
	DWORD   TimeDateStamp;                  // 0 if not bound,
	// -1 if bound, and real date\time stamp
	//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)

	DWORD   ForwarderChain;                 // -1 if no forwarders
	DWORD   Name;
	DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
};

//Import functions
struct IMAGE_IMPORT_BY_NAME {
	WORD    Hint; //Function index 
	BYTE    Name[1];//Function name
};


struct IMAGE_THUNK_DATA{
	union {
		DWORD ForwarderString;      // PBYTE 
		DWORD Function;             // PDWORD
		DWORD Ordinal;
		DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
	} u1;
};


//Resource directory
struct IMAGE_RESOURCE_DIRECTORY {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	WORD    NumberOfNamedEntries;
	WORD    NumberOfIdEntries;
	//  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
};

struct IMAGE_RESOURCE_DIRECTORY_ENTRY {
	union {
		struct {
			DWORD NameOffset:31;
			DWORD NameIsString:1;
		} DUMMYSTRUCTNAME;
		DWORD   Name;
		WORD    Id;
	} DUMMYUNIONNAME;
	union {
		DWORD   OffsetToData;
		struct {
			DWORD   OffsetToDirectory:31;
			DWORD   DataIsDirectory:1;
		} DUMMYSTRUCTNAME2;
	} DUMMYUNIONNAME2;
};

struct IMAGE_RESOURCE_DIR_STRING_U {
	WORD    Length;
	wchar_t  NameString[ 1 ];
};


struct IMAGE_RESOURCE_DATA_ENTRY {
	DWORD   OffsetToData;
	DWORD   Size;
	DWORD   CodePage;
	DWORD   Reserved;
};


#endif


struct IMAGE_IMPORT_FUNCTIONSMAP
{
	DWORD				 addr;

	IMAGE_IMPORT_FUNCTIONSMAP()
	{
		addr=0;
	}
};

struct IMAGE_IMPORT_DESCRIPTOR_MAP_FUNCTIONS
{
	vector<IMAGE_IMPORT_FUNCTIONSMAP> m_ImportFunctions;
};


//Import functions
struct __IMAGE_IMPORT_BY_NAME {
	WORD    Hint; //Function index 
	char    *Name;//Function name
};

struct IMAGE_IMPORT_TABLE_INFO
{
	char *ImportName;
	__IMAGE_IMPORT_BY_NAME *ImportTable;
	int ImportCount;
};

class PeFile
{
public:
	PeFile()
	{
		m_pf=NULL;
		m_Image=NULL;
	};
	~PeFile()
	{
		free();
	};

	

	bool		Load_PE_File(const char *fileName);
	bool		IsDLL();
	bool		IsExec();
	bool		IsImportFunctionRVA(int Tableindex,int FuncIndex);
	bool		Dump(const char *pDumpFileName);
	bool		DumpMemoryToFile(const char *pDumpFileName,void *BufferPointer,size_t Size);
	bool		UpdateNtHeader(IMAGE_NT_HEADERS ntHeader);
	bool		AddSection(DWORD Characteristics,char Name[8],DWORD Size,DWORD &RVA,void *CopyBuffer=NULL);
	bool		RemoveLastSection();
	bool		AddImportTables(IMAGE_IMPORT_TABLE_INFO ImportTables[],int Count);
	bool        HasRelocationTable();
	void		free();
	void        EnumImageResourceData(IMAGE_RESOURCE_DIRECTORY *Rootdir,void (*callBackFunction)(WORD *id,wchar_t *Name,DWORD length,DWORD *offsetToDataEnter,DWORD *size1,DWORD *DataRVA,void *Buffer));
	wchar_t    *GetResourceWchar(DWORD offset,DWORD &length);

	DWORD		&GetEntryPoint();
	DWORD		GetCheckSum();
	DWORD       GetImportFunctionHint(int Tableindex,int FuncIndex);
	DWORD       GetImportFunctionRVA(int Tableindex,int FuncIndex);
	DWORD		GetSectionCharacter(int index);
	DWORD		GetDirectoryRVA(int index);

	int			GetImportTableCount(){return m_ImageImportDescriptors.size();}
	int			GetImportFunctionsCount(int TableIndex);
	int			GetImageResourceDirectoryCount(IMAGE_RESOURCE_DIRECTORY *pdir);
	int			GetSectionCount(){return m_ImageSectionHeaders.size();}
	int			GetSectionIndexByRVA(DWORD RVA);

	int			GetResourceDirctorySectionIndex();
	int			GetImportTableDirectorySectionIndex();
	int			GetExportTableDirectorySectionIndex();


	const char *GetImportTableName(int Tableindex);
	const char *GetImportFunctionName(int Tableindex,int FuncIndex);
	const char *GetSectionName(int index);
	

	IMAGE_DOS_HEADER			*GetImageDosHeaderPointer();
	IMAGE_NT_HEADERS			*GetImageNtHeaderPointer();
	IMAGE_SECTION_HEADER		*GetSectionHeaderPointer(int Index);
	IMAGE_RESOURCE_DIRECTORY	*GetImageRootResourceDirectoryPointer();
	IMAGE_DATA_DIRECTORY		*GetImageDirectory(int Index);



	size_t		GetFileSize();
	size_t		GetImageSize(){return m_ImageSize;}
	size_t      RVA_To_FOA(DWORD RVA);
	size_t      ResourceOffset_To_FOA(DWORD Oft);
	size_t		GetFileAlignmentSize();
	size_t		GetSectionAlignmentSize();
	size_t		GetSectionRawSize(int index);
	size_t		GetSectionSize(int index);
	size_t		GetSectionRVA(int index);
	size_t		GetDirectorySize(int Index);


	//////////////////////////////////////////////////////////////////////////
	//Image operate functions
	void        SetSectionCharacteristic(int SectionIndex,DWORD characteristics);
	void        SetSectionRawSize(int SectionIndex,DWORD newRawSize);
	void		SetSectionVirtualSize(int SectionIndex,DWORD newVSize);
	void		SetEntryPoint(DWORD RVA);
	void		RemoveDataDirectory(int Directory);
	void		UpdateCheckSum();
	bool		ImageSolve(size_t ImageSize=0);
	bool		ImageRead(void *Dest,size_t Size);
	bool		ImageSeek(size_t Oft);
	size_t		ImageTell();

	void *		GetSectionBufferPointer(int index);
	void *		GetDirectoryDataBufferPointer(int index);
	void *		ImagePointer(size_t Offset);


private:

	PeFile(PeFile&){};

	static int												st_Reference;

	vector<IMAGE_SECTION_HEADER>							m_ImageSectionHeaders;
	vector<IMAGE_IMPORT_DESCRIPTOR>							m_ImageImportDescriptors;
	vector<IMAGE_IMPORT_DESCRIPTOR_MAP_FUNCTIONS>			m_ImageImportDescrtptorsMapFunctions;
		 
	size_t													m_FileSize;
	size_t													m_ImageSize;
	size_t													m_ImageSeek;

	FILE													*m_pf;

	//Image buffer
	unsigned char											*m_Image;
};


class PeMap
{
public:
	PeMap()
	{
		m_Image = NULL;
		m_ImageSeek=0;
	};
	~PeMap()
	{
	};

	bool		Map(unsigned char *address);
	bool		IsDLL();
	bool		IsExec();
	bool		IsAddrCallRVA(DWORD addr);
	bool		Dump(const char *FileName,const char *address,unsigned int dumpSize);
	bool        HasRelocationTable();
	void        EnumImageResourceData(IMAGE_RESOURCE_DIRECTORY *Rootdir, void(*callBackFunction)(WORD *id, wchar_t *Name, DWORD length, DWORD *offsetToDataEnter, DWORD *size1, DWORD *DataRVA, void *Buffer));
	wchar_t    *GetResourceWchar(DWORD offset, DWORD &length);

	DWORD		&GetEntryPoint();
	DWORD       GetImportFunctionHint(int Tableindex,int FuncIndex);
	DWORD       GetImportFunctionRVA(int Tableindex,int FuncIndex);
	DWORD		GetSectionCharacter(int index);
	DWORD		GetDirectoryRVA(int index);

	int			GetImportTableCount();
	int			GetImportFunctionsCount(int TableIndex);
	int			GetImageResourceDirectoryCount(IMAGE_RESOURCE_DIRECTORY *pdir);
	int			GetSectionCount();
	int			GetSectionIndexByRVA(DWORD RVA);

	int			GetResourceDirctorySectionIndex();
	int			GetImportTableDirectorySectionIndex();
	int			GetExportTableDirectorySectionIndex();


	const char *GetImportTableName(int Tableindex);
	const char *GetImportFunctionName(int Tableindex, int FuncIndex);
	const char *GetSectionName(int index);


	IMAGE_DOS_HEADER			*GetImageDosHeaderPointer();
	IMAGE_NT_HEADERS			*GetImageNtHeaderPointer();
	IMAGE_SECTION_HEADER		*GetSectionHeaderPointer(int Index);
	IMAGE_RESOURCE_DIRECTORY	*GetImageRootResourceDirectoryPointer();
	IMAGE_DATA_DIRECTORY		*GetImageDirectory(int Index);
	IMAGE_IMPORT_DESCRIPTOR     *GetImportDescriptor(int Index);
	IMAGE_THUNK_DATA			*GetImportFunctionThunkData(int TableIndex, int FuncIndex);

	size_t      RVA_To_FOA(DWORD RVA);
	size_t      ResourceOffset_To_FOA(DWORD Oft);
	size_t      ResourceOffset(DWORD Oft);
	size_t		GetFileAlignmentSize();
	size_t		GetSectionAlignmentSize();
	size_t		GetSectionSize(int index);
	size_t		GetSectionRVA(int index);
	size_t		GetDirectorySize(int Index);


	//////////////////////////////////////////////////////////////////////////
	//Image operate functions
	DWORD	    SetIAT(int TableIndex,int FuncIndex,DWORD newCallRVA);
	DWORD		SetIAT(const char *FunctionName,DWORD newCallRVA);



	void *		GetSectionBufferPointer(int index);
	void *		GetDirectoryDataBufferPointer(int index);
	void *		ImagePointer(size_t Offset);
	void		ImageRead(void *Dest, size_t Size);
	void		ImageSeek(size_t Oft);
	size_t		ImageTell();

private:
	size_t													m_ImageSeek;
	unsigned char											*m_Image;
};

class APIHooker
{
public:
	APIHooker()
	{m_pfnOrig=NULL;m_bHookbin=false;};
	~APIHooker();
	void Unhook();
	void Rehook();
	bool Hook(char * pszModName, char * pszFuncName, DWORD pfnHook);
	bool Hook(char *bin,unsigned int size,DWORD pfnHook);
	unsigned char	m_btNewBytes[8], m_btOldBytes[8];
	HMODULE			m_hModule;
	DWORD			m_pfnOrig;
	bool			m_bHookbin;
};