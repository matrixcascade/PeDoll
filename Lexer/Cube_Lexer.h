#pragma  once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <stdarg.h>
#include <assert.h>

#ifndef TRUE
#define TRUE                 1
#endif;

#ifndef FALSE
#define FALSE                0
#endif;

#define  SORT_STATUS_NORMAL  0
#define  SORT_STATUS_SPACER  1
#define  SORT_STATUS_COMMENT 2
#define  SORT_STATUS_NEWLINE 3

#define  CA_COMMENT_MAX_COUNT    128
#define  CA_SPACER_MAX_COUNT	 128
#define  CA_DELIMITER_MAX_COUNT  128
#define  CA_CONTAINER_MAX_COUNT  128
#define  CA_TOKEN_MAX_CHARACTER_SIZE 256

typedef struct _CA_Token
{
	unsigned int Type;
	char Mnemonic[CA_TOKEN_MAX_CHARACTER_SIZE];
}CA_Token;


typedef struct _CA_Container 
{
	char ContainerBegin;
	char ContainerEnd;
}CA_Container;


typedef struct _CA_COMMENT 
{
	char CommentBegin;
	char CommentEnd;
}CA_Comment;



#define  CUBE_LEXER_LEXEME_TYPE_END       0
#define  CUBE_LEXER_LEXEME_TYPE_SPACER    1
#define  CUBE_LEXER_LEXEME_TYPE_DELIMITER 2
#define  CUBE_LEXER_LEXEME_TYPE_CONATINER 3
#define  CUBE_LEXER_LEXEME_TYPE_NEWLINE   4
#define  CUBE_LEXER_LEXEME_TYPE_TOKEN     5
#define  CUBE_LEXER_LEXEME_TYPE_ERR		  -1

using namespace std;
class CubeLexer
{
public:
	CubeLexer()
	{
		m_CommentCount=0;
		m_SpacerCount=0;
		m_DelimiterCount=0;
		m_ContainerCount=0;
		m_SourceOffset=0;
		m_Sources=NULL;
		memset(m_Delimiter,0,sizeof(m_Delimiter));
		memset(m_Spacer,0,sizeof(m_Spacer));
	}
	~CubeLexer()
	{
		Free();
	}
	void RegisterComment(char Begin,char End);
	void RegisterSpacer(char Spacer);
	unsigned int RegisterDelimiter(char Delimiter);
	unsigned int GetDelimiterType(char Delimiter);
	unsigned int RegisterContainer(char Begin,char End);
	unsigned int GetContainerType(char *pContainerText);
	void Free();
	int SortText(char *SourceText);
	int LoadSourceFromFile(char *FileName);
	size_t GetFileSize(char *FileName);
	unsigned int GetNextLexeme();
	unsigned int GetCurrentLexeme();
	char		 GetSymbol(){return m_Symbol;}
	char *		 GetLexemeString(){return m_CurLexeme;}
	char *		 GetIncludedString();
	int IsCharNumeric(char chr);
	int IsStringInteger(char *pString);
private:
	int IsCommentStart(char ch);
	int IsCommentEnd(char startch,char ch);
	int IsSpacer(char chr);
	int IsDelimiter(char chr);
	int IsContainerStart(char ch);
	int IsContainerEnd(char startch,char ch);
	CA_Token *GetToken(const char *Mnemonic);
	int NextChar(char ch);
	int IsSourcsEnd();
	void CA_ERROR(char *Error);
	int IsNewLine(char ch);
	void RegisterToken(const char *Mnemonic,unsigned int Type);
	char m_Symbol;
	

	CA_Comment m_Comment[CA_COMMENT_MAX_COUNT];
	char	   m_Spacer[CA_SPACER_MAX_COUNT];
	char       m_Delimiter[CA_DELIMITER_MAX_COUNT];
	CA_Container m_Container[CA_CONTAINER_MAX_COUNT];
	int	       m_ContainerCount;
	int        m_DelimiterCount;
	int	       m_CommentCount;
	char		*m_Sources;
	unsigned int m_SortStatus;
	unsigned int m_CurrentLexemeFlag;
	char         m_SortComment;
	char		m_CurLexeme[CA_TOKEN_MAX_CHARACTER_SIZE];
	unsigned long m_SourceOffset;

	vector<CA_Token> m_Tokens;

	int m_SpacerCount;
};
