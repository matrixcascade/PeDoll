#include "./Cube_Lexer.h"
#define PT_STRING_COPY strcpy_s
//----------------------------------------------------------------------------------

void CubeLexer::CA_ERROR(char *Error)
{
	printf("<CA ERROR>  %s\n",Error);
}

//-------------------------------------------Lexer----------------------------------

int CubeLexer::IsCharNumeric(char chr)
{
	if (chr>=('0')&&chr<=('9'))
	{
		return TRUE;
	}
	return FALSE;
}

int CubeLexer::IsStringInteger(char *pString)
{
	int Dot=FALSE;
	if (pString==NULL)
	{
		return FALSE;
	}

	if (strlen(pString)==0)
	{
		return FALSE;
	}
	unsigned int CurrentCharIndex;

	if (!IsCharNumeric(pString[0])&&!(pString[0]==('-')))
	{
		if(pString[0]!='.')
			return FALSE;
	}
	for (CurrentCharIndex=1;
		CurrentCharIndex<strlen(pString);
		CurrentCharIndex++)
	{
		if (!IsCharNumeric(pString[CurrentCharIndex]))
		{
			if (pString[CurrentCharIndex]=='e'&&(pString[CurrentCharIndex+1]=='+'||pString[CurrentCharIndex+1]=='-'))
			{
				CurrentCharIndex++;
				continue;
			}

			if (pString[CurrentCharIndex]=='.')
			{
				if (Dot)
				{
					return FALSE;
				}
				else
				{
					Dot=TRUE;
					continue;
				}
			}
			return FALSE;
		}
	}
	return TRUE;
}



// -------------------------------------------New Line-------------------------------
int CubeLexer::IsNewLine(char ch)
{
	return ch=='\r'||ch=='\n';
}

//-------------------------------------------Comment----------------------------------


int CubeLexer::IsCommentStart(char ch)
{
	int i=0;
	for (i=0;i<m_CommentCount;i++)
	{
		if(ch==m_Comment[i].CommentBegin)
			return TRUE;
	}
	return FALSE;
}

int CubeLexer::IsCommentEnd(char startch,char ch)
{
	int i=0;
	for (i=0;i<m_CommentCount;i++)
	{
		if(startch==m_Comment[i].CommentBegin&&ch==m_Comment[i].CommentEnd)
			return TRUE;
	}
	return FALSE;
}


//--------------------------------------------Spacer-----------------------------------


void CubeLexer::RegisterSpacer(char Spacer)
{
	int i;
	if (m_SpacerCount<CA_SPACER_MAX_COUNT)
	{
		for (i=0;i<m_SpacerCount;i++)
		{
			if (m_Spacer[i]==Spacer)
			{
				assert(0);
				return;
			}
		}
		m_Spacer[m_SpacerCount++]=Spacer;
	}
}

int CubeLexer::IsSpacer(char chr)
{
	int i;
	for (i=0;i<m_SpacerCount;i++)
	{
		if (m_Spacer[i]==chr)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------Delimiter--------------------------------


unsigned int CubeLexer::RegisterDelimiter(char Delimiter)
{
	int i;
	if (m_DelimiterCount<CA_DELIMITER_MAX_COUNT)
	{
		for (i=0;i<m_DelimiterCount;i++)
		{
			if (m_Delimiter[i]==Delimiter)
			{
				return i;
			}
		}
		m_Delimiter[m_DelimiterCount++]=Delimiter;
		return m_DelimiterCount-1;
	}
	return -1;
}





int CubeLexer::IsDelimiter(char chr)
{
	int i;
	for (i=0;i<m_DelimiterCount;i++)
	{
		if (m_Delimiter[i]==chr)
		{
			return TRUE;
		}
	}
	return FALSE;
}

unsigned int CubeLexer::GetDelimiterType(char Delimiter)
{
	int i;
	for (i=0;i<m_DelimiterCount;i++)
	{
		if (m_Delimiter[i]==Delimiter)
		{
			return i;
		}
	}
	return -1;
}

//--------------------------------------------Container---------------------------------



unsigned int CubeLexer::RegisterContainer(char Begin,char End)
{
	int i;
	if (m_ContainerCount<CA_DELIMITER_MAX_COUNT)
	{
		for (i=0;i<m_ContainerCount;i++)
		{
			if (m_Container[i].ContainerBegin==Begin&&m_Container[i].ContainerEnd==End)
			{
				return i;
			}
		}
		m_Container[m_ContainerCount].ContainerBegin=Begin;
		m_Container[m_ContainerCount].ContainerEnd=End;
		m_ContainerCount++;
		return m_ContainerCount-1;
	}
	return -1;
}


unsigned int CubeLexer::GetContainerType(char *pContainerText)
{
	for (int i=0;i<m_ContainerCount;i++)
	{
		if (pContainerText[0]==m_Container[i].ContainerBegin&&pContainerText[strlen(pContainerText)-1]==m_Container[i].ContainerEnd)
		{
			return i;
		}
	}
	return -1;
}

int CubeLexer::IsContainerStart(char ch)
{
	int i=0;
	for (i=0;i<m_ContainerCount;i++)
	{
		if(ch==m_Container[i].ContainerBegin)
			return TRUE;
	}
	return FALSE;
}

int CubeLexer::IsContainerEnd(char startch,char ch)
{
	int i=0;
	for (i=0;i<m_ContainerCount;i++)
	{
		if(startch==m_Container[i].ContainerBegin&&ch==m_Container[i].ContainerEnd)
			return TRUE;
	}
	return FALSE;
}


//--------------------------------------------TOKEN-------------------------------------


CA_Token *CubeLexer::GetToken(const char *Mnemonic)
{
	for (unsigned int i=0;i<m_Tokens.size();i++)
	{
		if (strcmp(Mnemonic,m_Tokens[i].Mnemonic)==0)
		{
			return &m_Tokens[i];
		}
	}
	return NULL;
}


void  CubeLexer::RegisterToken(const char *Mnemonic,unsigned int Type)
{
	assert(strlen(Mnemonic)<CA_TOKEN_MAX_CHARACTER_SIZE);


	if (GetToken(Mnemonic)!=NULL)
	{
		return;
	}

	CA_Token Token;
	PT_STRING_COPY(Token.Mnemonic,sizeof(Token.Mnemonic),Mnemonic);
	Token.Type=Type;

	m_Tokens.push_back(Token);

}




int CubeLexer::NextChar(char ch)
{

	switch(m_SortStatus)
	{
	case SORT_STATUS_NORMAL:
		if (IsSpacer(ch))
		{
			m_SortStatus=SORT_STATUS_SPACER;
			return TRUE;
		}
		if (IsCommentStart(ch))
		{
			m_SortStatus=SORT_STATUS_COMMENT;
			m_SortComment=ch;
			return FALSE;
		}
		if (IsNewLine(ch))
		{
			m_SortStatus=SORT_STATUS_NEWLINE;
			return TRUE;
		}
		return TRUE;
		break;
	case SORT_STATUS_SPACER:
		if (IsSpacer(ch))
		{
			return FALSE;
		}
		if (IsCommentStart(ch))
		{
			m_SortStatus=SORT_STATUS_COMMENT;
			m_SortComment=ch;
			return FALSE;
		}
		if (IsNewLine(ch))
		{
			m_SortStatus=SORT_STATUS_NEWLINE;
			return TRUE;
		}
		m_SortStatus=SORT_STATUS_NORMAL;
		return TRUE;
		break;
	case SORT_STATUS_COMMENT:
		if (IsCommentEnd(m_SortComment,ch))
		{
			if(IsNewLine(ch)&&m_SortStatus!=SORT_STATUS_NEWLINE)
			{
				m_SortStatus=SORT_STATUS_NEWLINE;
				return TRUE;
			}
			m_SortStatus=SORT_STATUS_NORMAL;
		}
		return FALSE;
		break;
	case SORT_STATUS_NEWLINE:
		if (IsSpacer(ch)||IsNewLine(ch))
		{
			return FALSE;
		}
		if (IsCommentStart(ch))
		{
			m_SortStatus=SORT_STATUS_COMMENT;
			m_SortComment=ch;
			return FALSE;
		}
		m_SortStatus=SORT_STATUS_NORMAL;
		return TRUE;
	}
	return TRUE;
}
int CubeLexer::SortText(char *SourceText)
{
	unsigned long Offset=0;
	if (m_Sources)
	{
		free(m_Sources);
		m_SourceOffset=0;
		m_SortStatus=SORT_STATUS_NORMAL;
	}
	if ((m_Sources=(char *)malloc(strlen(SourceText)+1))==NULL)
	{
		return FALSE;
	}

	m_SortStatus=SORT_STATUS_NEWLINE;

	while(*SourceText)
	{
		if (NextChar(*SourceText))
		{
			if(IsNewLine(*SourceText)&&IsSpacer(m_Sources[Offset-1]))
				Offset--;
			if(IsCommentStart(*SourceText)&&IsSpacer(m_Sources[Offset-1]))
				Offset--;
			if(IsDelimiter(*SourceText)&&IsSpacer(m_Sources[Offset-1]))
				Offset--;
			if(IsSpacer(*SourceText)&&IsDelimiter(m_Sources[Offset-1]))
			{
				SourceText++;
				continue;
			}

			m_Sources[Offset++]=*(SourceText);
		}
		SourceText++;
	}
	m_Sources[Offset]='\0';
	return TRUE;
}




int CubeLexer::IsSourcsEnd()
{
	return !m_Sources[m_SourceOffset];
}


unsigned int CubeLexer::GetNextLexeme()
{
	int iLem=0;
	char chBegin;
		if (m_Sources[m_SourceOffset]=='\0')
		{
			//printf("<End>\n");
			m_Symbol='0';
			m_CurLexeme[0]='\0';
			m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_END;
			return CUBE_LEXER_LEXEME_TYPE_END;
		}
		if (IsSpacer(m_Sources[m_SourceOffset]))
		{
			m_Symbol=m_Sources[m_SourceOffset];
			m_CurLexeme[0]=m_Sources[m_SourceOffset];
			m_CurLexeme[1]='\0';
			m_SourceOffset++;
			//printf("<Spacer>\n");
			m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_SPACER;
			return CUBE_LEXER_LEXEME_TYPE_SPACER;
		}

		if (IsNewLine(m_Sources[m_SourceOffset]))
		{
			m_Symbol=m_Sources[m_SourceOffset];
			m_CurLexeme[0]=m_Sources[m_SourceOffset];
			m_CurLexeme[1]='\0';
			m_SourceOffset++;
			//printf("<New line>\n");
			m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_NEWLINE;
			return CUBE_LEXER_LEXEME_TYPE_NEWLINE;
		}
		if (IsContainerStart((chBegin=m_Sources[m_SourceOffset])))
		{
			iLem=0;
			m_CurLexeme[iLem++]=m_Sources[m_SourceOffset];
			m_SourceOffset++;
			while (!IsSourcsEnd()&&!IsContainerEnd(chBegin,m_Sources[m_SourceOffset]))
			{
				m_CurLexeme[iLem++]=m_Sources[m_SourceOffset++];

				if (iLem>=CA_TOKEN_MAX_CHARACTER_SIZE)
				{
					m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_ERR;
					return CUBE_LEXER_LEXEME_TYPE_ERR;
				}
			}
			m_CurLexeme[iLem++]=m_Sources[m_SourceOffset];
			m_CurLexeme[iLem]='\0';
			m_SourceOffset++;

			//printf("<Container> %s\n",m_CurLexeme);
			m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_CONATINER;
			return CUBE_LEXER_LEXEME_TYPE_CONATINER;
		}

		if (IsDelimiter(m_Sources[m_SourceOffset]))
		{
			m_Symbol=m_Sources[m_SourceOffset];
			m_CurLexeme[0]=m_Sources[m_SourceOffset];
			m_CurLexeme[1]='\0';
			//printf("<Delimiter> %c\n",m_Sources[m_SourceOffset]);
			m_SourceOffset++;
			m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_DELIMITER;
			return CUBE_LEXER_LEXEME_TYPE_DELIMITER;
		}
		iLem=0;
		m_Symbol='\0';
		while (!IsSourcsEnd()&&!IsDelimiter(m_Sources[m_SourceOffset])&&!IsSpacer(m_Sources[m_SourceOffset])&&!IsNewLine(m_Sources[m_SourceOffset])&&!IsContainerStart(m_Sources[m_SourceOffset]))
		{
			m_CurLexeme[iLem++]=m_Sources[m_SourceOffset++];
			if (iLem>=CA_TOKEN_MAX_CHARACTER_SIZE)
			{
				m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_ERR;
				return CUBE_LEXER_LEXEME_TYPE_ERR;
			}
		}
		m_CurLexeme[iLem]='\0';
		//printf("<Token> %s\n",m_CurLexeme);
	m_CurrentLexemeFlag=CUBE_LEXER_LEXEME_TYPE_TOKEN;
	return CUBE_LEXER_LEXEME_TYPE_TOKEN;
}

char * CubeLexer::GetIncludedString()
{
	if (m_CurrentLexemeFlag==CUBE_LEXER_LEXEME_TYPE_CONATINER)
	{
		char Lexeme[CA_TOKEN_MAX_CHARACTER_SIZE];
		strcpy(Lexeme,m_CurLexeme);
		Lexeme[strlen(Lexeme)-1]='\0';
		return Lexeme+1;
	}
	return GetLexemeString();
}


unsigned int CubeLexer::GetCurrentLexeme()
{
	return m_CurrentLexemeFlag;
}

void CubeLexer::RegisterComment( char Begin,char End )
{
	int i;
	if (m_CommentCount<CA_COMMENT_MAX_COUNT)
	{
		for (i=0;i<m_CommentCount;i++)
		{
			if (m_Comment[i].CommentBegin==Begin&&m_Comment[i].CommentEnd==End)
			{
				return;
			}
		}
		m_Comment[m_CommentCount].CommentBegin=Begin;
		m_Comment[m_CommentCount].CommentEnd=End;
		m_CommentCount++;
	}
}

int CubeLexer::LoadSourceFromFile( char *FileName )
{
	FILE *pf;
	char *Source;
	size_t FileSize;

	Free();

	if ((FileSize=GetFileSize(FileName))==0)
	{
		return FALSE;
	}
	if ((Source=(char *)malloc(FileSize+1))==NULL)
	{
		return FALSE;
	}
	Source[FileSize]=0;
	pf=fopen(FileName,"rb");
	if (!pf)
	{
		return FALSE;
	}
	if(!fread(Source,FileSize,1,pf))
	{
		fclose(pf);
		return FALSE;
	}
	fclose(pf);

	if(!SortText(Source))
	{
		free(Source);
		return FALSE;
	}
	free(Source);
	return TRUE;
}

size_t CubeLexer::GetFileSize( char *FileName )
{
		FILE *pf=NULL;
		size_t Size;
		if ((pf=fopen(FileName,"rb")))
		{
			fseek(pf,0,SEEK_SET);
			fseek(pf,0,SEEK_END);
			Size= ftell(pf);
			fclose(pf);
			return Size;
		}
		return 0;
}

void CubeLexer::Free()
{
	if(m_Sources)
	free(m_Sources);
	m_Sources=NULL;
}

