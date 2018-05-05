#include "../inc/PEStructure.h"



size_t PeFile::GetFileSize()
{
	return m_FileSize;
}


bool PeFile::Load_PE_File( const char *fileName )
{
	free();

	if (!(m_pf=fopen(fileName,"rb")))
	{
		return false;
	}

	//Get file size;
	fseek(m_pf,0,SEEK_END);
	m_FileSize=ftell(m_pf);
	fseek(m_pf,0,SEEK_SET);

	if (!(m_FileSize%2))
	{
		m_Image=new unsigned char[m_FileSize];
		m_ImageSize=m_FileSize;
	}
	else
	{
		//ERROR Check Code
		m_Image=new unsigned char[m_FileSize+1];
		m_ImageSize=m_FileSize+1;
	}
	
	memset(m_Image,0,m_ImageSize);

	m_ImageSeek=0;
	if (!m_Image)
	{
		m_ImageSize=0;
		goto _ERROR;
	}

	

	//Read file to image
	if(fread(m_Image,1,m_FileSize,m_pf)!=m_FileSize)
	{
		fclose(m_pf);
		goto _ERROR;
	}
	//Close file
	fclose(m_pf);
	m_pf=NULL;

	return ImageSolve(m_ImageSize);

_ERROR:
	return false;
}

void PeFile::free()
{
		if (m_Image)
		{
			delete [] m_Image;
			m_Image=NULL;
		}                                                                                                                                                                                                                                                                                                                                                                                                                                          
}
int PeFile::GetSectionIndexByRVA(DWORD RVA)
{
	unsigned int StartSection;
	unsigned int i=0;

	//Calculate start section
	for (i=0;i<m_ImageSectionHeaders.size()-1;i++)
	{
		if (m_ImageSectionHeaders[i].VirtualAddress<=RVA&&RVA<m_ImageSectionHeaders[i+1].VirtualAddress)
		{
			StartSection=i;
			break;
		}
	}
	if (i==m_ImageSectionHeaders.size()-1)
	{
		if (m_ImageSectionHeaders[i].VirtualAddress<=RVA&&
			m_ImageSectionHeaders[i].VirtualAddress+m_ImageSectionHeaders[i].SizeOfRawData>=RVA)
		{
			StartSection=i;
		}
		else
		{
			//Error while 0
			return 0;
		}
	}
	return StartSection;
}


size_t PeFile::RVA_To_FOA(DWORD RVA)
{
	unsigned int StartSection;
	unsigned int i;

	//Calculate start section
	for (i=0;i<m_ImageSectionHeaders.size()-1;i++)
	{
		if (m_ImageSectionHeaders[i].VirtualAddress<=RVA&&RVA<m_ImageSectionHeaders[i+1].VirtualAddress)
		{
			StartSection=i;
			break;
		}
	}
	if (i==m_ImageSectionHeaders.size()-1)
	{
		if (m_ImageSectionHeaders[i].VirtualAddress<=RVA&&
			m_ImageSectionHeaders[i].VirtualAddress+m_ImageSectionHeaders[i].SizeOfRawData>=RVA)
		{
			StartSection=i;
		}
		else
		{
			//Error while 0xffffffff
			return 0xffffffff;
		}
	}
	
	//Calculate offset from section start address to RVA 
	size_t Oft=RVA-m_ImageSectionHeaders[StartSection].VirtualAddress;

	//return FOA(file of address)
	return m_ImageSectionHeaders[StartSection].PointerToRawData+Oft;
}

//Get Check Sum (WORD) of PE file
DWORD PeFile::GetCheckSum()
{

		DWORD CheckSum=0;
	
		IMAGE_NT_HEADERS *pNT=(IMAGE_NT_HEADERS *)(m_Image+GetImageDosHeaderPointer()->e_lfanew);

		pNT->OptionalHeader.CheckSum=0;

		WORD *pWdBuffer=(WORD *)m_Image;

		for (unsigned int i=0;i<m_ImageSize/2;i++)
		{
			CheckSum+=pWdBuffer[i];
			CheckSum=(CheckSum>>16)+(CheckSum&0xffff);
		}

		return CheckSum+GetFileSize();

}

int PeFile::GetImportFunctionsCount(int TableIndex)
{
	return m_ImageImportDescrtptorsMapFunctions.at(TableIndex).m_ImportFunctions.size();
}

const char * PeFile::GetImportTableName(int Tableindex)
{
	IMAGE_IMPORT_DESCRIPTOR iid;
	iid=m_ImageImportDescriptors.at(Tableindex);

	static char ImportTableName[IMPORT_TABLE_NAME_MAXLEN];

		DWORD oft=RVA_To_FOA(iid.Name);
		if (oft==0xffffffff)
		{
			return NULL;
		}
		if(!ImageSeek(oft))
		{
			ImportTableName[0]='\0';
			return NULL;
		}
		int Index=0;
		char ch;

		while (ImageRead(&ch,1))
		{
			if (ch!='\0')
			{
				ImportTableName[Index]=ch;
				Index++;
			}
			else
			{
				ImportTableName[Index]='\0';
				return ImportTableName;
				break;
			}
			if (Index>=IMPORT_TABLE_NAME_MAXLEN-1)
			{
				ImportTableName[0]='\0';
				return NULL;
			}
		}
		return NULL;

}

const char * PeFile::GetImportFunctionName(int Tableindex,int FuncIndex)
{
	static char FunctionName[IMPORT_FUNCTION_NAME_MAXLEN];
	if ((unsigned int)Tableindex>=m_ImageImportDescrtptorsMapFunctions.size())
	{
		return NULL;
	}

	if ((unsigned int)FuncIndex>=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions.size())
	{
		return NULL;
	}
	IMAGE_IMPORT_FUNCTIONSMAP iinoa=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions[FuncIndex];
	
	if (iinoa.addr&(~((iinoa.addr|~iinoa.addr)>>1)))
	{
		//RVA
		return "";
	}
	else
	{
		size_t DestOft=RVA_To_FOA(iinoa.addr);
		if (DestOft==0xffffffff)
		{
			return "";
		}
		IMAGE_IMPORT_BY_NAME *piibn=(IMAGE_IMPORT_BY_NAME *)ImagePointer(DestOft);
		//Make sure name is valid
		char *pstr=(char *)piibn->Name;
		size_t Index=0;
		while (pstr[Index]!='\0')
		{
			FunctionName[Index]=pstr[Index];
			Index++;
			if (Index>=IMPORT_FUNCTION_NAME_MAXLEN-1||Index>=m_ImageSize-DestOft-4)
			{
				return NULL;
			}
		}

		return (const char *)piibn->Name;
	}
	
	return NULL;
}



bool PeFile::ImageRead(void *Dest,size_t Size)
{
	if (m_ImageSize-m_ImageSeek<Size)
	{
		return false;
	}
	memcpy(Dest,m_Image+m_ImageSeek,Size);
	m_ImageSeek+=Size;
	return true;
}

bool PeFile::ImageSeek(size_t Oft)
{
	if (Oft>m_ImageSize-1)
	{
		return false;
	}
	m_ImageSeek=Oft;
	return true;
}

void * PeFile::ImagePointer(size_t Offset)
{
	if (Offset>m_ImageSize-1)
	{
		return NULL;
	}
	return m_Image+Offset;
}

size_t PeFile::ImageTell()
{
	return m_ImageSeek;
}

DWORD PeFile::GetImportFunctionHint(int Tableindex,int FuncIndex)
{
	if ((unsigned int)Tableindex>=m_ImageImportDescrtptorsMapFunctions.size())
	{
		return -1;
	}

	if ((unsigned int)FuncIndex>=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions.size())
	{
		return -1;
	}
	IMAGE_IMPORT_FUNCTIONSMAP iinoa=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions[FuncIndex];

	if (iinoa.addr&((iinoa.addr|~iinoa.addr)>>1))
	{
		//RVA
		return -1;
	}
	else
	{
		size_t DestOft=RVA_To_FOA(iinoa.addr);
		if (DestOft==0xffffffff)
		{
			return -1;
		}
		IMAGE_IMPORT_BY_NAME *piibn=(IMAGE_IMPORT_BY_NAME *)ImagePointer(DestOft);
		return piibn->Hint;
	}

	return -1;
}

DWORD PeFile::GetImportFunctionRVA(int Tableindex,int FuncIndex)
{
	if ((unsigned int)Tableindex>=m_ImageImportDescrtptorsMapFunctions.size())
	{
		return -1;
	}

	if ((unsigned int)FuncIndex>=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions.size())
	{
		return -1;
	}
	IMAGE_IMPORT_FUNCTIONSMAP iinoa=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions[FuncIndex];

	if (iinoa.addr&((iinoa.addr|~iinoa.addr)>>1))
	{
		//RVA
		return iinoa.addr;
	}
	else
	{
		return -1;
	}

	return -1;
}

bool PeFile::IsImportFunctionRVA(int Tableindex,int FuncIndex)
{
	if ((unsigned int)Tableindex>=m_ImageImportDescrtptorsMapFunctions.size())
	{
		return false;
	}

	if ((unsigned int)FuncIndex>=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions.size())
	{
		return false;
	}
	IMAGE_IMPORT_FUNCTIONSMAP iinoa=m_ImageImportDescrtptorsMapFunctions[Tableindex].m_ImportFunctions[FuncIndex];

	if (iinoa.addr&((iinoa.addr|~iinoa.addr)>>1))
	{
		//RVA
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool PeFile::ImageSolve(size_t ImageSize)
{
	//Structures clean up
	m_ImageSectionHeaders.clear();
	m_ImageImportDescriptors.clear();
	m_ImageImportDescrtptorsMapFunctions.clear();

	m_ImageSeek=0;
	m_ImageSize=ImageSize;

	IMAGE_DOS_HEADER TempDosHeader;
	//Read DOS header
	if(!ImageRead(&TempDosHeader,sizeof(TempDosHeader)))
		goto _ERROR;

	if (TempDosHeader.e_magic!=0x5a4d)
	{
		//Signature error
		goto _ERROR;
	}


	//Seek to PE Header
	if(!ImageSeek(GetImageDosHeaderPointer()->e_lfanew))
		goto _ERROR;

	//Read NT Headers
	IMAGE_NT_HEADERS TempNtHeader;

	if (!ImageRead(&TempNtHeader,sizeof(TempNtHeader)))
		goto _ERROR;

	if (TempNtHeader.Signature!=0x00004550)
	{
		//Signature error
		goto _ERROR;
	}


	//Load sections from file
	for (int i=0;i<TempNtHeader.FileHeader.NumberOfSections;i++)
	{
		IMAGE_SECTION_HEADER ImageSectionHeader;
		if (!ImageRead(&ImageSectionHeader,sizeof(IMAGE_SECTION_HEADER)))
		{
			goto _ERROR;
		}
		m_ImageSectionHeaders.push_back(ImageSectionHeader);
	}

	//Get import tables
	size_t TableFOA=RVA_To_FOA(TempNtHeader.OptionalHeader.DataDirectory[1].VirtualAddress);
	if (TableFOA==0xffffffff)
	{
		goto _ERROR;
	}

	if(!ImageSeek(TableFOA))
		goto _ERROR;

	while (true)
	{
		IMAGE_IMPORT_DESCRIPTOR iid;
		if(!ImageRead(&iid,sizeof(IMAGE_IMPORT_DESCRIPTOR)))
			goto _ERROR;

		if(iid.Name!=0)
		{
			m_ImageImportDescriptors.push_back(iid);
			IMAGE_IMPORT_DESCRIPTOR_MAP_FUNCTIONS FuncMap;
			size_t Resoft=ImageTell();
			size_t Funcoft=RVA_To_FOA(iid.FirstThunk);
			if (Funcoft==0xffffffff)
			{
				goto _ERROR;
			}
			if(!ImageSeek(Funcoft))
				goto _ERROR;


			IMAGE_THUNK_DATA tkDat;

			while (true)
			{
				if(!ImageRead(&tkDat,sizeof(IMAGE_THUNK_DATA)))
					goto _ERROR;

				if (tkDat.u1.AddressOfData==0)
				{
					break;
				}

				IMAGE_IMPORT_FUNCTIONSMAP iinoa;
				iinoa.addr=tkDat.u1.Function;
				FuncMap.m_ImportFunctions.push_back(iinoa);

			}
			m_ImageImportDescrtptorsMapFunctions.push_back(FuncMap);

			ImageSeek(Resoft);
		}
		else
			break;
	}



	return true;

_ERROR:
	return false;
}

bool PeFile::Dump(const char *pDumpFileName)
{
	return DumpMemoryToFile(pDumpFileName,m_Image,m_ImageSize);
}


bool PeFile::DumpMemoryToFile(const char *pDumpFileName,void *BufferPointer,size_t Size)
{
	FILE *pf=NULL;
	size_t	WriteSize=0,SizeSum=Size;
	unsigned char *pBufferOft=(unsigned char *)BufferPointer;
	if ((pf=fopen(pDumpFileName,"wb"))!=NULL)
	{
		while (SizeSum)
		{
			if(SizeSum>256)
			{
				WriteSize=fwrite(pBufferOft,1,256,pf);
				SizeSum-=WriteSize;
			}
			else
			{
				WriteSize=fwrite(pBufferOft,1,SizeSum,pf);
				SizeSum-=WriteSize;
			}
			if (!WriteSize)
			{
				fclose(pf);
				return false;
			}
			pBufferOft+=WriteSize;
		}
		fclose(pf);
		
		return true;
	}
	return false;
}


bool PeFile::AddSection(DWORD Characteristics,char Name[8],DWORD Size,DWORD &RVA,void *CopyBuffer/*=NULL*/)
{

	IMAGE_SECTION_HEADER newSectionHeader;
	newSectionHeader.Characteristics=Characteristics;
	memcpy(newSectionHeader.Name,Name,8);

	size_t FileAlign=GetImageNtHeaderPointer()->OptionalHeader.FileAlignment;
	size_t SectionAlign=GetImageNtHeaderPointer()->OptionalHeader.SectionAlignment;

	//Checkout the PE space for insert section header
	if (FileAlign-(GetImageDosHeaderPointer()->e_lfanew
		+sizeof(IMAGE_NT_HEADERS)+sizeof(IMAGE_SECTION_HEADER)*GetImageNtHeaderPointer()->FileHeader.NumberOfSections)
		%FileAlign<=sizeof(IMAGE_SECTION_HEADER))
	{
		return false;
	}



	DWORD RawAllocSize;
	DWORD DummyFillSize;
	if (Size%FileAlign)
	{
		RawAllocSize=(Size/FileAlign+1)*FileAlign;
	}
	else
	{
		RawAllocSize=Size;
	}

	if (m_ImageSize%FileAlign)
	{
		//Resource file can not be file alignment
		//Fill dummy space for image
		DummyFillSize=(m_ImageSize/FileAlign+1)*FileAlign-m_ImageSize;
	}
	else
	{
		DummyFillSize=0;
	}

	unsigned char *NewImage=new unsigned char[m_ImageSize+RawAllocSize+DummyFillSize];
	if (NewImage==NULL)
	{
		return false;
	}
	memset(NewImage,0,m_ImageSize+RawAllocSize+DummyFillSize);

	memcpy(NewImage,m_Image,m_ImageSize);

	if (CopyBuffer)
	{
		memcpy(NewImage+DummyFillSize+m_ImageSize,CopyBuffer,Size);
	}
	//Replace old image
	delete [] m_Image;
	m_Image=NewImage;
	
	
	//Setup header
	if(RawAllocSize%SectionAlign)
	newSectionHeader.Misc.VirtualSize=SectionAlign*(RawAllocSize/SectionAlign+1);
	else
	newSectionHeader.Misc.VirtualSize=RawAllocSize;

	//Virtual address=Last section+size of last section
	int index=m_ImageSectionHeaders.size()-1;
	int ResSectionSize=m_ImageSectionHeaders[index].Misc.VirtualSize;

	if(ResSectionSize%SectionAlign)
	newSectionHeader.VirtualAddress=m_ImageSectionHeaders[index].VirtualAddress+SectionAlign*(ResSectionSize/SectionAlign+1);
	else
	newSectionHeader.VirtualAddress=m_ImageSectionHeaders[index].VirtualAddress+ResSectionSize;

	//Return RVA
	RVA=newSectionHeader.VirtualAddress;
	newSectionHeader.SizeOfRawData=RawAllocSize;
	
	//End of file
	newSectionHeader.PointerToRawData=m_ImageSize+DummyFillSize;

	newSectionHeader.PointerToLinenumbers=0;
	newSectionHeader.PointerToRelocations=0;
	newSectionHeader.NumberOfRelocations=0;
	newSectionHeader.NumberOfLinenumbers=0;
	
	//completed
	IMAGE_SECTION_HEADER *pNewSectionHeader=(IMAGE_SECTION_HEADER *)ImagePointer(GetImageDosHeaderPointer()->e_lfanew
		+sizeof(IMAGE_NT_HEADERS)+sizeof(IMAGE_SECTION_HEADER)*GetImageNtHeaderPointer()->FileHeader.NumberOfSections);

	*pNewSectionHeader=newSectionHeader;

	//Modify header
	IMAGE_NT_HEADERS *pModifyNtHeader=GetImageNtHeaderPointer();
	if (!pModifyNtHeader)
	{
		goto _ERROR;
	}
	pModifyNtHeader->FileHeader.NumberOfSections++;
	//Modify Image size
	if(Size%SectionAlign)
	pModifyNtHeader->OptionalHeader.SizeOfImage=newSectionHeader.VirtualAddress+SectionAlign*(Size/SectionAlign+1);
	else
	pModifyNtHeader->OptionalHeader.SizeOfImage=newSectionHeader.VirtualAddress+Size;
	
	//Fix check code
	pModifyNtHeader->OptionalHeader.CheckSum=GetCheckSum();

	//Resolve
	ImageSolve(m_ImageSize+RawAllocSize+DummyFillSize);
	return true;

_ERROR:
	delete [] NewImage;
	return false;
}


bool PeFile::RemoveLastSection()
{
	if (GetSectionCount()<=1)
	{
		return false;
	}

	size_t LastSectionRawSize;
	IMAGE_SECTION_HEADER *pLastSectionHeader=GetSectionHeaderPointer(GetSectionCount()-1);
	if (pLastSectionHeader==NULL)
	{
		return false;
	}
	LastSectionRawSize=pLastSectionHeader->SizeOfRawData;
	//Delete last SectionHeader
	memset(pLastSectionHeader,0,sizeof(IMAGE_SECTION_HEADER));

	//Resize Image
	unsigned char *pNewImage=new unsigned char[m_ImageSize-LastSectionRawSize];
	if (pNewImage==NULL)
	{
		return false;
	}
	memcpy(pNewImage,m_Image,m_ImageSize-LastSectionRawSize);
	delete [] m_Image;
	m_Image=pNewImage;


	//Modify header
	IMAGE_NT_HEADERS *pModifyNtHeader=GetImageNtHeaderPointer();
	if (!pModifyNtHeader)
	{
		return false;
	}
	pModifyNtHeader->FileHeader.NumberOfSections--;

	//Modify Image size
	pModifyNtHeader->OptionalHeader.SizeOfImage=GetSectionHeaderPointer(GetSectionCount()-2)->VirtualAddress+GetSectionHeaderPointer(GetSectionCount()-2)->Misc.VirtualSize;

	m_ImageSize-=LastSectionRawSize;
	//Fix check code
	pModifyNtHeader->OptionalHeader.CheckSum=GetCheckSum();

	//Resolve
	if(!ImageSolve(m_ImageSize))
	{
		free();
		return false;
	}
	return true;
}




bool PeFile::UpdateNtHeader( IMAGE_NT_HEADERS ntHeader )
{
	if (m_Image)
	{
		IMAGE_NT_HEADERS * pNtHeader=(IMAGE_NT_HEADERS *) (m_Image+GetImageDosHeaderPointer()->e_lfanew);
		*pNtHeader=ntHeader;
		return true;
	}
	return false;
}

IMAGE_DOS_HEADER	* PeFile::GetImageDosHeaderPointer()
{
	if (!m_Image)
	{
		return NULL;
	}
	return (IMAGE_DOS_HEADER	*)m_Image;
}

IMAGE_NT_HEADERS	* PeFile::GetImageNtHeaderPointer()
{
	if(!m_Image)
	{
		return NULL;
	}
	return (IMAGE_NT_HEADERS *) (m_Image+GetImageDosHeaderPointer()->e_lfanew);
}

DWORD		&PeFile::GetEntryPoint()
{
	return GetImageNtHeaderPointer()->OptionalHeader.AddressOfEntryPoint;
}

bool PeFile::IsDLL()
{
	//Is Dynamic Link Library 
	return (GetImageNtHeaderPointer()->FileHeader.Characteristics&0x2002)==0x2002;
}

bool PeFile::IsExec()
{
	if(!IsDLL())
		return (GetImageNtHeaderPointer()->FileHeader.Characteristics&0x0002)!=0;
	return false;
}

size_t GetImportByNameStructureSize(__IMAGE_IMPORT_BY_NAME *piibn)
{
	size_t externsize=0;
	while(*(piibn->Name+externsize))
	{
		externsize++;
	}
	return 3+externsize;
}

size_t GetImportTableStructureSize(IMAGE_IMPORT_TABLE_INFO *iiti)
{
	size_t size=0;
	size+=strlen(iiti->ImportName)+1;
	for (int i=0;i<iiti->ImportCount;i++)
	{
		size+=GetImportByNameStructureSize(iiti->ImportTable+i)+sizeof(DWORD)+sizeof(DWORD);
	}
	return size+sizeof(DWORD)*2;
}

bool PeFile::AddImportTables(IMAGE_IMPORT_TABLE_INFO ImportTables[],int Count)
{
	if (Count==0)
	{
		return true;
	}
	size_t newSectionSize=0;
	newSectionSize=(m_ImageImportDescriptors.size()+Count)*sizeof(IMAGE_IMPORT_DESCRIPTOR);
	for (int i=0;i<Count;i++)
	{
		newSectionSize+=GetImportTableStructureSize(ImportTables+i);
	}

	newSectionSize+=/*Zero structure*/sizeof(IMAGE_IMPORT_DESCRIPTOR);

	unsigned char *newBuffer=new unsigned char[newSectionSize];
	if (newBuffer==NULL)
	{
		return false;
	}

	//Copy old importTable to buffer
	for (unsigned int i=0;i<m_ImageImportDescriptors.size();i++)
	{
		memcpy(newBuffer+i*sizeof(IMAGE_IMPORT_DESCRIPTOR),&m_ImageImportDescriptors.at(i),sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}

	DWORD RVA;

	if (!AddSection(0xC0000040,".rtab",newSectionSize,RVA))
	{
		delete [] newBuffer;
		return false;
	}
	//Add new import table

	//Locate new import table first

	//Offset positions
	DWORD IDescOffset;
	DWORD INTIATOffset;
	DWORD IByNameOffset;
	DWORD NameOffset;


	IDescOffset=sizeof(IMAGE_IMPORT_DESCRIPTOR)*(m_ImageImportDescriptors.size());
	INTIATOffset=sizeof(IMAGE_IMPORT_DESCRIPTOR)*(m_ImageImportDescriptors.size()+Count+1);
	IByNameOffset=INTIATOffset;
	for (int i=0;i<Count;i++)
	{
		IByNameOffset+=(ImportTables[i].ImportCount+1)*sizeof(DWORD)*2;
	}
	NameOffset=newSectionSize;
	for (int i=0;i<Count;i++)
	{
		NameOffset-=(strlen(ImportTables[i].ImportName)+1);
	}
	
	//////////////////////////////////////////////////////////////
	//
	//  IMAGE_IMPORT_DESCRIPTOR [MULT]
	//  INT/IAT [MULT]
	//  IMAGE_IMPORT_DESCRIPTOR [MULT]
	/////////////////////////////////////////////////////////////

	//Redirection
	for (int i=0;i<Count;i++)
	{

		IMAGE_IMPORT_DESCRIPTOR *pdesc=(IMAGE_IMPORT_DESCRIPTOR *)(newBuffer+IDescOffset);
		//Build IMAGE_IMPORT_DESCRIPTOR
		//Pointer to INT
		pdesc->OriginalFirstThunk=RVA+INTIATOffset;
		
		//Pointer to IAT
		pdesc->FirstThunk=RVA+INTIATOffset+(ImportTables[i].ImportCount+1)*sizeof(DWORD);
		pdesc->ForwarderChain=0;
		pdesc->TimeDateStamp=0;
		pdesc->Name=RVA+NameOffset;
		
		
		for (int j=0;j<ImportTables[i].ImportCount;j++)
		{
			//Build INT
			DWORD *pINT=(DWORD *)(newBuffer+INTIATOffset);
			*pINT=IByNameOffset+RVA;
			//Build IAT
			DWORD *pIAT=pINT+ImportTables[i].ImportCount+1;
			*pIAT=IByNameOffset+RVA;

			//INT/IAT increase
			INTIATOffset+=sizeof(DWORD);
			
			//Build IMPORT_BY_NAME
			IMAGE_IMPORT_BY_NAME *piibn=(IMAGE_IMPORT_BY_NAME *)(newBuffer+IByNameOffset);
			piibn->Hint=ImportTables[i].ImportTable[j].Hint;
			strcpy((char *)piibn->Name,ImportTables[i].ImportTable[j].Name);

			//IByNameOffset increase
			IByNameOffset+=strlen(ImportTables[i].ImportTable[j].Name)+1;
			IByNameOffset+=sizeof(WORD);

		}

		//Build name
		strcpy((char *)(newBuffer+NameOffset),ImportTables[i].ImportName);
		//NameOffset increase
		NameOffset+=strlen(ImportTables[i].ImportName)+1;


		//End INT
		DWORD *pINT=(DWORD *)(newBuffer+INTIATOffset);
		*pINT=0;
		//End IAT
		DWORD *pIAT=pINT+(ImportTables[i].ImportCount+1);
		*pIAT=0;

		//Skip IAT&END
		INTIATOffset+=sizeof(DWORD)*(ImportTables[i].ImportCount+1);

		//Descriptor increase
		IDescOffset+=sizeof(IMAGE_IMPORT_DESCRIPTOR);

	}
	//End Descriptor
	IMAGE_IMPORT_DESCRIPTOR *pdesc=(IMAGE_IMPORT_DESCRIPTOR *)(newBuffer+IDescOffset);
	memset(pdesc,0,sizeof(IMAGE_IMPORT_DESCRIPTOR));

	//Copy to section
	memcpy(m_Image+this->RVA_To_FOA(RVA),newBuffer,newSectionSize);

	//free 
	delete [] newBuffer;

	//Redirection PE header
	IMAGE_NT_HEADERS *pNtHeader=GetImageNtHeaderPointer();
	//Modify ImportTable Section characters
	SetSectionCharacteristic(GetSectionIndexByRVA(pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress),0xC0000040);
	pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress=RVA;
	pNtHeader->OptionalHeader.DataDirectory[1].Size=sizeof(IMAGE_IMPORT_DESCRIPTOR)*(m_ImageImportDescriptors.size()+Count);
	pNtHeader->OptionalHeader.DataDirectory[11].VirtualAddress=0;
	pNtHeader->OptionalHeader.DataDirectory[11].Size=0;
	pNtHeader->OptionalHeader.DataDirectory[12].VirtualAddress=0;
	pNtHeader->OptionalHeader.DataDirectory[12].Size=0;
	pNtHeader->OptionalHeader.DataDirectory[12].VirtualAddress=0;
	pNtHeader->OptionalHeader.DataDirectory[12].Size=0;


	GetImageNtHeaderPointer()->OptionalHeader.CheckSum=GetCheckSum();


	GetSectionHeaderPointer(1)->Characteristics=GetSectionHeaderPointer(1)->Characteristics|0x40000000;
	ImageSolve(m_ImageSize);

	return true;
}

IMAGE_SECTION_HEADER * PeFile::GetSectionHeaderPointer(int Index)
{
	if (Index>GetImageNtHeaderPointer()->FileHeader.NumberOfSections-1)
	{
		return NULL;
	}
	return (IMAGE_SECTION_HEADER *)ImagePointer(GetImageDosHeaderPointer()->e_lfanew
		+sizeof(IMAGE_NT_HEADERS)+sizeof(IMAGE_SECTION_HEADER)*Index);
}

IMAGE_RESOURCE_DIRECTORY	* PeFile::GetImageRootResourceDirectoryPointer()
{
	DWORD offset=RVA_To_FOA(GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress);
	if (offset==0xffffffff)
	{
		return NULL;
	}
	return (IMAGE_RESOURCE_DIRECTORY *)(m_Image+offset);

}

int PeFile::GetImageResourceDirectoryCount(IMAGE_RESOURCE_DIRECTORY *pdir)
{
	return pdir->NumberOfIdEntries+pdir->NumberOfNamedEntries;
}

size_t PeFile::ResourceOffset_To_FOA(DWORD Oft)
{
	DWORD ResRVA=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress+Oft;
	return RVA_To_FOA(ResRVA);
}

void PeFile::EnumImageResourceData(IMAGE_RESOURCE_DIRECTORY *Rootdir,void (*callBackFunction)(WORD *id,wchar_t *Name,DWORD length,DWORD *offsetToDataEnter,DWORD *size1,DWORD *DataRVA,void *Buffer))
{
	WORD   *v_id=NULL;
	wchar_t *v_Name=NULL;
	DWORD   v_length;
	DWORD   *v_offsetToDataEnter;
	DWORD   *v_size1;
	DWORD   *v_DataRVA;
	void	*v_Buffer=NULL;

	if (callBackFunction==NULL)
	{
		return;
	}
	//Skip IMAGE_RESOURCE_DIRECTORY
	IMAGE_RESOURCE_DIRECTORY_ENTRY *pirde=(IMAGE_RESOURCE_DIRECTORY_ENTRY *)(Rootdir+1);

	for (int i=0;i<Rootdir->NumberOfIdEntries+Rootdir->NumberOfNamedEntries;i++)
	{
		if (!pirde[i].DataIsDirectory)
		{
			IMAGE_RESOURCE_DATA_ENTRY *pirdate=(IMAGE_RESOURCE_DATA_ENTRY *)(ImagePointer(ResourceOffset_To_FOA(pirde[i].OffsetToData&0x7fffffff)));
			if (pirdate==NULL)
			{
				return;
			}

			if (pirde[i].NameIsString)
			{
				v_Name=GetResourceWchar(pirde[i].Name&0x7fffffff,v_length);
				if (v_Name==NULL)
				{
					return;
				}
			}
			else
			{
				v_length=NULL;
				v_id=&pirde[i].Id;
			}

			v_offsetToDataEnter=&pirde[i].OffsetToData;
			v_size1=&pirdate->Size;
			v_DataRVA=&pirdate->OffsetToData;

			if(ImagePointer(RVA_To_FOA(pirdate->OffsetToData)))
			v_Buffer=ImagePointer(RVA_To_FOA(pirdate->OffsetToData));
			else
			return;

			if(callBackFunction)
			callBackFunction(v_id,v_Name,v_length,v_offsetToDataEnter,v_size1,v_DataRVA,v_Buffer);

		}
		else
		{
			EnumImageResourceData((IMAGE_RESOURCE_DIRECTORY *)(m_Image+ResourceOffset_To_FOA(pirde[i].OffsetToData&0x7fffffff)),callBackFunction);
		}
	}


}


wchar_t * PeFile::GetResourceWchar(DWORD offset,DWORD &length)
{
	size_t _offset=ResourceOffset_To_FOA(offset);
	if (_offset==0xffffffff)
	{
		return NULL;
	}
	IMAGE_RESOURCE_DIR_STRING_U *pStrU=(IMAGE_RESOURCE_DIR_STRING_U *)(ImagePointer(_offset));
	if (!pStrU)
	{
		return NULL;
	}
	length=pStrU->Length;

	return (pStrU->NameString);
}

const char * PeFile::GetSectionName(int index)
{
	if ((unsigned int)index>=m_ImageSectionHeaders.size())
	{
		return NULL;
	}
	return (const char *)m_ImageSectionHeaders.at(index).Name;
}

int PeFile::GetResourceDirctorySectionIndex()
{
	DWORD RVA=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

int PeFile::GetImportTableDirectorySectionIndex()
{
	DWORD RVA=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[1].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

int PeFile::GetExportTableDirectorySectionIndex()
{
	DWORD RVA=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[0].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

size_t PeFile::GetFileAlignmentSize()
{
	return GetImageNtHeaderPointer()->OptionalHeader.FileAlignment;
}

size_t PeFile::GetSectionAlignmentSize()
{
	return GetImageNtHeaderPointer()->OptionalHeader.SectionAlignment;
}



size_t PeFile::GetSectionRawSize(int index)
{
	return m_ImageSectionHeaders[index].SizeOfRawData;
}

size_t PeFile::GetSectionSize(int index)
{
	return m_ImageSectionHeaders[index].Misc.VirtualSize;
}

DWORD PeFile::GetSectionCharacter(int index)
{
	return m_ImageSectionHeaders[index].Characteristics;
}

size_t PeFile::GetSectionRVA(int index)
{
	return m_ImageSectionHeaders[index].VirtualAddress;
}

void * PeFile::GetSectionBufferPointer(int index)
{
	return m_Image+RVA_To_FOA(GetSectionRVA(index));
}

size_t PeFile::GetDirectorySize(int Index)
{
	return GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Index].Size;
}

void * PeFile::GetDirectoryDataBufferPointer(int index)
{
	return m_Image+RVA_To_FOA(GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[index].VirtualAddress);
}

DWORD PeFile::GetDirectoryRVA(int index)
{
	return GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[index].VirtualAddress;
}

IMAGE_DATA_DIRECTORY		* PeFile::GetImageDirectory(int Index)
{
	return &GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Index];
}

void PeFile::SetSectionCharacteristic( int SectionIndex,DWORD characteristics )
{
	if (SectionIndex>GetSectionCount()-1)
	{
		return;
	}
	if(GetSectionHeaderPointer(SectionIndex)!=NULL)
	{
		GetSectionHeaderPointer(SectionIndex)->Characteristics=characteristics;
	}
}

void PeFile::SetSectionRawSize( int SectionIndex,DWORD newRawSize )
{
	if (SectionIndex>GetSectionCount()-1)
	{
		return;
	}
	if(GetSectionHeaderPointer(SectionIndex)!=NULL)
	{
		GetSectionHeaderPointer(SectionIndex)->SizeOfRawData=newRawSize;
	}
}

void PeFile::SetSectionVirtualSize( int SectionIndex,DWORD newVSize )
{
	if (SectionIndex>GetSectionCount()-1)
	{
		return;
	}
	if(GetSectionHeaderPointer(SectionIndex)!=NULL)
	{
		GetSectionHeaderPointer(SectionIndex)->Misc.VirtualSize=newVSize;
	}
}

void PeFile::UpdateCheckSum()
{
	GetImageNtHeaderPointer()->OptionalHeader.CheckSum=GetCheckSum();
}

void PeFile::SetEntryPoint( DWORD RVA )
{
	GetImageNtHeaderPointer()->OptionalHeader.AddressOfEntryPoint=RVA;
}

bool PeFile::HasRelocationTable()
{
	if (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress!=0)
	{
		return true;
	}
	return false;
}

void PeFile::RemoveDataDirectory( int Directory )
{
	DWORD Rva=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Directory].VirtualAddress;
	DWORD Size=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Directory].Size;
	if (RVA_To_FOA(Rva)==0xffffffff)
	{
		return;
	}
	memset(m_Image+RVA_To_FOA(Rva),0,Size);
	GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Directory].VirtualAddress=0;
}


bool PeMap::Map(unsigned char *address)
{
	m_Image = address;
	m_ImageSeek=0;
	IMAGE_DOS_HEADER TempDosHeader;

	//Read DOS header
	ImageRead(&TempDosHeader, sizeof(TempDosHeader));

	if (TempDosHeader.e_magic!=0x5a4d)
	{
		//Signature error
		goto _ERROR;
	}


	//Seek to PE Header
	ImageSeek(GetImageDosHeaderPointer()->e_lfanew);

	//Read NT Headers
	IMAGE_NT_HEADERS TempNtHeader;

	ImageRead(&TempNtHeader, sizeof(TempNtHeader));

	if (TempNtHeader.Signature!=0x00004550)
	{
		//Signature error
		goto _ERROR;
	}
	return true;
_ERROR:
	return false;
}

bool PeMap::IsDLL()
{
	return (GetImageNtHeaderPointer()->FileHeader.Characteristics & 0x2002) == 0x2002;
}

bool PeMap::IsExec()
{
	if (!IsDLL())
		return (GetImageNtHeaderPointer()->FileHeader.Characteristics & 0x0002) != 0;
	return false;
}

bool PeMap::IsAddrCallRVA(DWORD addr)
{

	if (addr&((addr | ~addr) >> 1))
	{
		//RVA
		return true;
	}
	else
	{
		return false;
	}

	return false;
}



bool PeMap::Dump(const char *FileName, const char *address, unsigned int dumpSize)
{
	FILE *pf = NULL;
	size_t	WriteSize = 0, SizeSum = dumpSize;
	unsigned char *pBufferOft = (unsigned char *)address;
	if ((pf = fopen(FileName, "wb")) != NULL)
	{
		while (SizeSum)
		{
			if (SizeSum > 256)
			{
				WriteSize = fwrite(pBufferOft, 1, 256, pf);
				SizeSum -= WriteSize;
			}
			else
			{
				WriteSize = fwrite(pBufferOft, 1, SizeSum, pf);
				SizeSum -= WriteSize;
			}
			if (!WriteSize)
			{
				fclose(pf);
				return false;
			}
			pBufferOft += WriteSize;
		}
		fclose(pf);

		return true;
	}
	return false;
}

bool PeMap::HasRelocationTable()
{
	if (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress != 0)
	{
		return true;
	}
	return false;
}

void PeMap::EnumImageResourceData(IMAGE_RESOURCE_DIRECTORY *Rootdir, void(*callBackFunction)(WORD *id, wchar_t *Name, DWORD length, DWORD *offsetToDataEnter, DWORD *size1, DWORD *DataRVA, void *Buffer))
{
	WORD   *v_id = NULL;
	wchar_t *v_Name = NULL;
	DWORD   v_length;
	DWORD   *v_offsetToDataEnter;
	DWORD   *v_size1;
	DWORD   *v_DataRVA;
	void	*v_Buffer = NULL;

	if (callBackFunction == NULL)
	{
		return;
	}
	//Skip IMAGE_RESOURCE_DIRECTORY
	IMAGE_RESOURCE_DIRECTORY_ENTRY *pirde = (IMAGE_RESOURCE_DIRECTORY_ENTRY *)(Rootdir + 1);

	for (int i = 0; i < Rootdir->NumberOfIdEntries + Rootdir->NumberOfNamedEntries; i++)
	{
		if (!pirde[i].DataIsDirectory)
		{
			IMAGE_RESOURCE_DATA_ENTRY *pirdate = (IMAGE_RESOURCE_DATA_ENTRY *)(ImagePointer(ResourceOffset(pirde[i].OffsetToData & 0x7fffffff)));
			if (pirdate == NULL)
			{
				return;
			}

			if (pirde[i].NameIsString)
			{
				v_Name = GetResourceWchar(pirde[i].Name & 0x7fffffff, v_length);
				if (v_Name == NULL)
				{
					return;
				}
			}
			else
			{
				v_length = NULL;
				v_id = &pirde[i].Id;
			}

			v_offsetToDataEnter = &pirde[i].OffsetToData;
			v_size1 = &pirdate->Size;
			v_DataRVA = &pirdate->OffsetToData;

			if (ImagePointer(ResourceOffset(pirdate->OffsetToData)))
				v_Buffer = ImagePointer(ResourceOffset(pirdate->OffsetToData));
			else
				return;

			if (callBackFunction)
				callBackFunction(v_id, v_Name, v_length, v_offsetToDataEnter, v_size1, v_DataRVA, v_Buffer);

		}
		else
		{
			EnumImageResourceData((IMAGE_RESOURCE_DIRECTORY *)(m_Image + (pirde[i].OffsetToData & 0x7fffffff)), callBackFunction);
		}
	}

}

wchar_t * PeMap::GetResourceWchar(DWORD offset, DWORD &length)
{
	size_t _offset = (offset);
	if (_offset == 0xffffffff)
	{
		return NULL;
	}
	IMAGE_RESOURCE_DIR_STRING_U *pStrU = (IMAGE_RESOURCE_DIR_STRING_U *)(ImagePointer(_offset));
	if (!pStrU)
	{
		return NULL;
	}
	length = pStrU->Length;

	return (pStrU->NameString);
}

DWORD &PeMap::GetEntryPoint()
{
	return GetImageNtHeaderPointer()->OptionalHeader.AddressOfEntryPoint;
}


DWORD PeMap::GetImportFunctionHint(int Tableindex,int FuncIndex)
{
	if ((int)Tableindex>=GetImportFunctionsCount(Tableindex))
	{
		return -1;
	}

	if ((int)FuncIndex>=GetImportFunctionsCount(Tableindex))
	{
		return -1;
	}
	DWORD addr=GetImportFunctionRVA(Tableindex,FuncIndex);

	if (addr&((addr|~addr)>>1))
	{
		//RVA
		return -1;
	}
	else
	{
		size_t DestOft=(addr);
		IMAGE_IMPORT_BY_NAME *piibn=(IMAGE_IMPORT_BY_NAME *)ImagePointer(DestOft);
		return piibn->Hint;
	}

	return -1;
}

DWORD PeMap::GetImportFunctionRVA(int Tableindex,int FuncIndex)
{
	if ((int)Tableindex>=GetImportFunctionsCount(Tableindex))
	{
		return -1;
	}

	if ((int)FuncIndex>=GetImportFunctionsCount(Tableindex))
	{
		return -1;
	}
	
	return GetImportFunctionThunkData(Tableindex,FuncIndex)->u1.Function;
}

DWORD PeMap::GetSectionCharacter(int index)
{
	if (index>=GetSectionCount())
	{
		return 0;
	}
	return GetSectionHeaderPointer(index)->Characteristics;
}

DWORD PeMap::GetDirectoryRVA(int index)
{
	return GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[index].VirtualAddress;
}

int PeMap::GetImportTableCount()
{
	size_t VA = (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[1].VirtualAddress);
	ImageSeek(VA);
	int count = 0;
	while (true)
	{
		IMAGE_IMPORT_DESCRIPTOR iid;
		ImageRead(&iid, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		if (iid.Name != 0)
			count++;
		else
			break;
	}
	return count;
}

int PeMap::GetImportFunctionsCount(int TableIndex)
{
	int count = 0;
	IMAGE_IMPORT_DESCRIPTOR *pdesc = GetImportDescriptor(TableIndex);

	if (pdesc!=NULL)
	{
		IMAGE_IMPORT_DESCRIPTOR_MAP_FUNCTIONS FuncMap;
		size_t Resoft = ImageTell();
		size_t Funcoft = (pdesc->OriginalFirstThunk);
		ImageSeek(Funcoft);

		IMAGE_THUNK_DATA tkDat;
		while (true)
		{
			ImageRead(&tkDat, sizeof(IMAGE_THUNK_DATA));

			if (tkDat.u1.AddressOfData == 0)
			{
				break;
			}

			IMAGE_IMPORT_FUNCTIONSMAP iinoa;
			iinoa.addr = tkDat.u1.Function;
			count++;
		}
	}
	return count;
}

int PeMap::GetImageResourceDirectoryCount(IMAGE_RESOURCE_DIRECTORY *pdir)
{
	return pdir->NumberOfIdEntries + pdir->NumberOfNamedEntries;
}

int PeMap::GetSectionCount()
{
	//Load sections from file
	return GetImageNtHeaderPointer()->FileHeader.NumberOfSections;
}

int PeMap::GetSectionIndexByRVA(DWORD RVA)
{
	unsigned int StartSection;
	int i;

	//Calculate start section
	for (i = 0; i < GetSectionCount() - 1; i++)
	{
		if (GetSectionHeaderPointer(i)->VirtualAddress <= RVA&&RVA < GetSectionHeaderPointer(i+1)->VirtualAddress)
		{
			StartSection = i;
			break;
		}
	}
	if (i == GetSectionCount() - 1)
	{
		if (GetSectionHeaderPointer(i)->VirtualAddress <= RVA&&
			GetSectionHeaderPointer(i)->VirtualAddress + GetSectionHeaderPointer(i)->SizeOfRawData >= RVA)
		{
			StartSection = i;
		}
		else
		{
			//Error while 0
			return 0xffffffff;
		}
	}
	return StartSection;
}

int PeMap::GetResourceDirctorySectionIndex()
{
	DWORD RVA = GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

int PeMap::GetImportTableDirectorySectionIndex()
{
	DWORD RVA = GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[1].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

int PeMap::GetExportTableDirectorySectionIndex()
{
	DWORD RVA = GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[0].VirtualAddress;
	return GetSectionIndexByRVA(RVA);
}

const char * PeMap::GetImportTableName(int Tableindex)
{
	IMAGE_IMPORT_DESCRIPTOR iid;
	iid = *GetImportDescriptor(Tableindex);

	static char ImportTableName[IMPORT_TABLE_NAME_MAXLEN];

	DWORD oft = (iid.Name);
	if (oft == 0)
	{
		return NULL;
	}
	ImageSeek(oft);

	int Index = 0;
	char ch;

	while (true)
	{
		ImageRead(&ch, 1);
		if (ch != '\0')
		{
			ImportTableName[Index] = ch;
			Index++;
		}
		else
		{
			ImportTableName[Index] = '\0';
			return ImportTableName;
			break;
		}
		if (Index >= IMPORT_TABLE_NAME_MAXLEN - 1)
		{
			ImportTableName[0] = '\0';
			return NULL;
		}
	}
	return NULL;
}

const char * PeMap::GetImportFunctionName(int Tableindex, int FuncIndex)
{
	static char FunctionName[IMPORT_FUNCTION_NAME_MAXLEN];
	if ((int)Tableindex >= GetImportTableCount())
	{
		return NULL;
	}

	if ((int)FuncIndex >= GetImportFunctionsCount(Tableindex))
	{
		return NULL;
	}

	IMAGE_IMPORT_FUNCTIONSMAP iinoa;
	IMAGE_THUNK_DATA *pthunk = GetImportFunctionThunkData(Tableindex, FuncIndex);
	if (pthunk==NULL)
	{
		return NULL;
	}
	iinoa.addr = pthunk->u1.Function;


	if (iinoa.addr&(~((iinoa.addr | ~iinoa.addr) >> 1)))
	{
		//RVA
		return "";
	}
	else
	{
		size_t DestOft = (iinoa.addr);

		IMAGE_IMPORT_BY_NAME *piibn = (IMAGE_IMPORT_BY_NAME *)ImagePointer(DestOft);
		//Make sure name is valid
		char *pstr = (char *)piibn->Name;
		size_t Index = 0;
		while (pstr[Index] != '\0')
		{
			FunctionName[Index] = pstr[Index];
			Index++;
			if (Index >= IMPORT_FUNCTION_NAME_MAXLEN - 1)
			{
				return NULL;
			}
		}

		return (const char *)piibn->Name;
	}

	return NULL;
}

const char * PeMap::GetSectionName(int index)
{
	if ((int)index>=GetSectionCount())
	{
		return "";
	}
	return (const char *)GetSectionHeaderPointer(index)->Name;
}

size_t PeMap::GetFileAlignmentSize()
{
	return GetImageNtHeaderPointer()->OptionalHeader.FileAlignment;
}

size_t PeMap::GetSectionAlignmentSize()
{
	return GetImageNtHeaderPointer()->OptionalHeader.SectionAlignment;
}


size_t PeMap::GetSectionSize(int index)
{
	if ((int)index >= GetSectionCount())
	{
		return 0;
	}
	return GetSectionHeaderPointer(index)->Misc.VirtualSize;
}

size_t PeMap::GetSectionRVA(int index)
{
	if ((int)index >= GetSectionCount())
	{
		return 0;
	}
	return GetSectionHeaderPointer(index)->VirtualAddress;
}

size_t PeMap::GetDirectorySize(int Index)
{
	return GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Index].Size;
}

IMAGE_DOS_HEADER			* PeMap::GetImageDosHeaderPointer()
{
	if (!m_Image)
	{
		return NULL;
	}
	return (IMAGE_DOS_HEADER	*)m_Image;
}

IMAGE_NT_HEADERS			* PeMap::GetImageNtHeaderPointer()
{
	if (!m_Image)
	{
		return NULL;
	}
	return (IMAGE_NT_HEADERS *)(m_Image + GetImageDosHeaderPointer()->e_lfanew);
}

IMAGE_SECTION_HEADER		* PeMap::GetSectionHeaderPointer(int Index)
{
	if (Index > GetImageNtHeaderPointer()->FileHeader.NumberOfSections - 1)
	{
		return NULL;
	}
	return (IMAGE_SECTION_HEADER *)ImagePointer(GetImageDosHeaderPointer()->e_lfanew
		+ sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER)*Index);
}

IMAGE_RESOURCE_DIRECTORY	* PeMap::GetImageRootResourceDirectoryPointer()
{
	DWORD offset = (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress);
	return (IMAGE_RESOURCE_DIRECTORY *)(m_Image + offset);
}

IMAGE_DATA_DIRECTORY		* PeMap::GetImageDirectory(int Index)
{
	return &GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[Index];
}


IMAGE_IMPORT_DESCRIPTOR * PeMap::GetImportDescriptor(int Index)
{
	if (Index>GetImportTableCount()-1)
	{
		return NULL;
	}
	size_t VA = (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[1].VirtualAddress);
	
	IMAGE_IMPORT_DESCRIPTOR *piid=(IMAGE_IMPORT_DESCRIPTOR *)(m_Image+VA);

	return piid + Index;

}

IMAGE_THUNK_DATA			* PeMap::GetImportFunctionThunkData(int TableIndex, int FuncIndex)
{
	if (TableIndex>=GetImportTableCount())
	{
		return NULL;
	}
	if (FuncIndex>=GetImportFunctionsCount(TableIndex))
	{
		return NULL;
	}
	IMAGE_IMPORT_DESCRIPTOR *pDesc = GetImportDescriptor(TableIndex);
	size_t Funcoft = (pDesc->OriginalFirstThunk);

	IMAGE_THUNK_DATA *ptkDat=(IMAGE_THUNK_DATA *)ImagePointer(Funcoft);

	return ptkDat + FuncIndex;
	
}

size_t PeMap::RVA_To_FOA(DWORD RVA)
{
	unsigned int StartSection;

	//Calculate start section
	StartSection=GetSectionIndexByRVA(RVA);
	if (StartSection==0xffffffff)
	{
		return 0;
	}
	//return FOA(file of address)
	return GetSectionHeaderPointer(StartSection)->PointerToRawData+(RVA-GetSectionHeaderPointer(StartSection)->VirtualAddress);
}

size_t PeMap::ResourceOffset_To_FOA(DWORD Oft)
{
	DWORD ResRVA=GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress+Oft;
	return RVA_To_FOA(ResRVA);
}

size_t PeMap::ResourceOffset(DWORD Oft)
{
	return GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[2].VirtualAddress+Oft;
}

DWORD PeMap::SetIAT(int TableIndex,int FuncIndex,DWORD newCallRVA)
{
	IMAGE_IMPORT_DESCRIPTOR *pdesc=GetImportDescriptor(TableIndex);
	if (!pdesc)
	{
		return 0;
	}
	//IAT table addr;
	DWORD *pIAT=((DWORD *)ImagePointer(pdesc->FirstThunk))+ FuncIndex;
	DWORD resAddr=*(pIAT);
	DWORD oldProtect;
	VirtualProtect(pIAT, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect);
	(*pIAT)=newCallRVA;
	return resAddr;
}

DWORD PeMap::SetIAT(const char *FunctionName,DWORD newCallRVA)
{
	for (int i=0;i<GetImportTableCount();i++)
	{
		for(int j=0;j<GetImportFunctionsCount(i);j++)
		{
			const char *Name=GetImportFunctionName(i,j);
			if (Name!=NULL)
			{
				if (strcmp(Name,FunctionName)==0)
				{
					return SetIAT(i,j,newCallRVA);
				}
			}
		}
	}
	return 0;
}

void * PeMap::GetSectionBufferPointer(int index)
{
	return m_Image + (GetSectionRVA(index));
}

void * PeMap::GetDirectoryDataBufferPointer(int index)
{
	return m_Image + (GetImageNtHeaderPointer()->OptionalHeader.DataDirectory[index].VirtualAddress);
}

void * PeMap::ImagePointer(size_t Offset)
{
	return m_Image + Offset;
}

void PeMap::ImageRead(void *Dest, size_t Size)
{
	memcpy(Dest, m_Image + m_ImageSeek, Size);
	m_ImageSeek += Size;
}

void PeMap::ImageSeek(size_t Oft)
{
	m_ImageSeek = Oft;
}

size_t PeMap::ImageTell()
{
	return m_ImageSeek;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// API Hooker

bool APIHooker::Hook(char * pszModName, char * pszFuncName, DWORD pfnHook)
{
	if (m_pfnOrig!=NULL)
	{
		Unhook();
		m_bHookbin=false;
	}

	unsigned char btNewBytes[8] = { 0xB8, 0x00, 0x00, 0x40, 0x00, 0xFF, 0xE0, 0x00 };
	memcpy(m_btNewBytes, btNewBytes, 8);
	*(DWORD *)(m_btNewBytes + 1) = (DWORD)pfnHook;

	m_hModule = ::LoadLibrary(pszModName);
	if (m_hModule == NULL)
	{
		m_pfnOrig = NULL;
		return false;
	}
	m_pfnOrig = (DWORD)GetProcAddress(m_hModule, pszFuncName);

	if (m_pfnOrig != NULL)
	{
		DWORD dwOldProtect;
		VirtualProtect((LPVOID)m_pfnOrig, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		memcpy(m_btOldBytes, (LPVOID)m_pfnOrig, 8);

		*((DWORD *)m_pfnOrig)=*((DWORD *)m_btNewBytes);

		*(((DWORD *)m_pfnOrig)+1)=*(((DWORD *)m_btNewBytes)+1);

	//	WriteProcessMemory(::GetCurrentProcess(), (void *)m_pfnOrig,
	//		m_btNewBytes, sizeof(DWORD) * 2, NULL);

	}
	else
		return false;

	m_bHookbin=false;
	return true;
}

bool APIHooker::Hook(char *bin,unsigned int size,DWORD pfnHook)
{
	bool hasHook=false;
	if (m_pfnOrig)
	{
		m_bHookbin=false;
		Unhook();
	}
	unsigned char* Addr=(unsigned char *)GetModuleHandle(NULL);
	while ((DWORD)Addr<0x07ffffff-size)
	{
		if (memcmp(bin,Addr,size)==0)
		{
			unsigned char btNewBytes[8] = { 0xB8, 0x00, 0x00, 0x40, 0x00, 0xFF, 0xE0, 0x00 };
			memcpy(m_btNewBytes, btNewBytes, 8);
			*(DWORD *)(m_btNewBytes + 1) = (DWORD)pfnHook;
				
			DWORD dwOldProtect;
			VirtualProtect((LPVOID)Addr, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect);

			memcpy(m_btOldBytes, (LPVOID)Addr, 8);

			//*((DWORD *)m_pfnOrig)=*((DWORD *)m_btNewBytes);

			//*(((DWORD *)m_pfnOrig)+1)=*(((DWORD *)m_btNewBytes)+1);

			WriteProcessMemory(::GetCurrentProcess(), (void *)Addr,m_btNewBytes, sizeof(DWORD) * 2, NULL);

			hasHook=true;
			m_pfnOrig=(DWORD)Addr;
			return true;
		}
		Addr++;
	}
	return false;
}

APIHooker::~APIHooker()
{
	Unhook();
}

void APIHooker::Unhook()
{
	if (m_pfnOrig != NULL)
	{
		DWORD dwOldProtect;
		VirtualProtect((LPVOID)m_pfnOrig, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		*((DWORD *)m_pfnOrig)=*((DWORD *)m_btOldBytes);

		*(((DWORD *)m_pfnOrig)+1)=*(((DWORD *)m_btOldBytes)+1);

//		WriteProcessMemory(GetCurrentProcess(), (void *)m_pfnOrig,
//			m_btOldBytes, sizeof(DWORD) * 2, NULL);
	}
	m_bHookbin=false;
}

void APIHooker::Rehook()
{
	if (m_pfnOrig != NULL)
	{
		DWORD dwOldProtect;
		VirtualProtect((LPVOID)m_pfnOrig, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		*((DWORD *)m_pfnOrig)=*((DWORD *)m_btNewBytes);
		*(((DWORD *)m_pfnOrig)+1)=*(((DWORD *)m_btNewBytes)+1);
	
		//WriteProcessMemory(::GetCurrentProcess(), (void *)m_pfnOrig,
		//	m_btNewBytes, sizeof(DWORD) * 2, NULL);
	}
}
/*



////////////////////////////////////////////



int WINAPI hook_WSARecv(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd,
	LPDWORD lpFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{

	g_WSARecv.Unhook();
	int nRet = ::WSARecv(s, lpBuffers,
		dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
	g_WSARecv.Rehook();
	return nRet;
}

int WINAPI hook_WSARecvFrom(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd,
	LPDWORD lpFlags,
	struct sockaddr FAR *lpFrom,
	LPINT lpFromlen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{

	g_WSARecvFrom.Unhook();
	int nRet = ::WSARecvFrom(s, lpBuffers, dwBufferCount,
		lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
	g_WSARecvFrom.Rehook();
	return nRet;
}

*/