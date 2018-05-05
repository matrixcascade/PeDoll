#pragma  once 


#include "../Lexer/Cube_Lexer.h"
#include <regex>

#include <list>
#define  CUBE_GRAMMAR_TYPE_MAX_LIST		32
#define  CUBE_GRAMMAR_TYPE_MAX_LEMSIZE	256

#define  GRAMMAR_TYPE_ANY       0

///////////////////////////////////////////////////////////////////
//Layer 0
#define  GRAMMAR_TYPE_ANY		     0
#define  GRAMMAR_TOKEN				 1
#define  GRAMMAR_SPACER				 2
#define  GRAMMAR_DELIMITER			 3
#define  GRAMMAR_CONATINER           4
#define  GRAMMAR_NEWLINE	         5
#define  GRAMMAR_PARAMETER           6
#define  GRAMMAR_UNDEFINED			 6
#define  GRAMMAR_SENTENCE_UNKNOW			 ((unsigned int)(-2))
#define  GRAMMAR_SENTENCE_END				 ((unsigned int)(-1))


struct CubeBlockType
{
	unsigned int	BlockTypeTree[CUBE_GRAMMAR_TYPE_MAX_LIST];
	char			Mnemonic[CUBE_GRAMMAR_TYPE_MAX_LEMSIZE];
	regex			pattern;
	bool            bRegex;

	CubeBlockType(char *Mne,int Layer,...)
	{
		va_list arg_ptr; 
		memset(BlockTypeTree,0,sizeof(BlockTypeTree));
		memset(Mnemonic,0,sizeof(Mnemonic));

		bRegex=false;
		if(Mne)
		{
		#ifdef _DEBUG
			if (strlen(Mne)>=CUBE_GRAMMAR_TYPE_MAX_LEMSIZE)
			{
				assert(0);//Mnemonic is too long.
			}
		#endif}
		if (strlen(Mne)>=CUBE_GRAMMAR_TYPE_MAX_LEMSIZE)
		{
		memcpy(Mnemonic,Mne,CUBE_GRAMMAR_TYPE_MAX_LEMSIZE-1);
		}
		if(Mne)
		strcpy(Mnemonic,Mne);

		}
		va_start(arg_ptr,Layer); 
		for(int Offset=0;Offset<Layer;Offset++)
		BlockTypeTree[Offset]=va_arg(arg_ptr, int); 
		va_end(arg_ptr);

	}
	CubeBlockType()
	{
		memset(BlockTypeTree,0,CUBE_GRAMMAR_TYPE_MAX_LIST);
		memset(Mnemonic,0,sizeof(Mnemonic));
		bRegex=false;
	}

	~CubeBlockType()
	{
	}
	void AsRegex(bool Regex=true)
	{
		if(Regex)
		{
			regex tpattern(Mnemonic,regex_constants::extended);
			pattern=tpattern;
		}
		bRegex=Regex;
	}


	bool IsCharNumeric(char ch)
	{
		return ch>='0'&&ch<='9';
	}

	bool IsNumeric()
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

	void Set(char *Mne,int Layer,...)
	{
		va_list arg_ptr; 
		memset(BlockTypeTree,0,sizeof(BlockTypeTree));
		memset(Mnemonic,0,sizeof(Mnemonic));

		if (strlen(Mne)>=CUBE_GRAMMAR_TYPE_MAX_LEMSIZE)
		{
			memcpy(Mnemonic,Mne,CUBE_GRAMMAR_TYPE_MAX_LEMSIZE-4);
		}
		if(Mne)
			strcpy(Mnemonic,Mne);

		va_start(arg_ptr,Layer); 
		for(int Offset=0;Offset<Layer;Offset++)
			BlockTypeTree[Offset]=va_arg(arg_ptr, int); 
		va_end(arg_ptr); 
	}
	//equal mnemonic
	bool operator==(char *Mne)
	{
		return IsMemonicMatch(Mne);
	}
	//equal
	bool operator==(CubeBlockType& Token)
	{
		if(Mnemonic[0]!=0)
		return (memcmp(Token.BlockTypeTree,BlockTypeTree,CUBE_GRAMMAR_TYPE_MAX_LIST)==0)&&(strcmp(Mnemonic,Token.Mnemonic)==0);
		else
		return (memcmp(Token.BlockTypeTree,BlockTypeTree,CUBE_GRAMMAR_TYPE_MAX_LIST)==0);
	}
	//unequal
	bool operator!=(CubeBlockType& Token)
	{
		return !(*this==Token);
	}
	//Included
	bool operator>>(CubeBlockType &Token)
	{
		for (int i=0;i<CUBE_GRAMMAR_TYPE_MAX_LIST;i++)
		{
			if (Token.BlockTypeTree[i]==0)
			{
				return true;
			}
			if (BlockTypeTree[i]!=Token.BlockTypeTree[i])
			{
				return false;
			}
		}
		return true;
	}

	//Include
	bool  operator<<(CubeBlockType &Token)
	{
		for (int i=0;i<CUBE_GRAMMAR_TYPE_MAX_LIST;i++)
		{
			if (BlockTypeTree[i]==0)
			{
				return true;
			}
			if (BlockTypeTree[i]!=Token.BlockTypeTree[i])
			{
				return false;
			}
		}
		return true;
	}

	//Is Mnemonic match
	bool IsMemonicMatch(char *Str)
	{
		if (!bRegex)
		{
			return strcmp(Mnemonic,Str)==0;
		}
		else
		{
			match_results<string::const_iterator> result;
			string s(Str);
			return regex_match(s,result,pattern);
		}
	}
};


struct CubeGrammarSentence
{
	CubeGrammarSentence()
	{
		
	}
	~CubeGrammarSentence()
	{
		Reset();
	}

	void Reset()
	{
		m_vBlocks.clear();
	}
	void add(CubeBlockType &Token)
	{
		m_vBlocks.push_back(Token);
	}

	unsigned int GetBlocksCount()
	{
		return m_vBlocks.size();
	}

	char *GetBlockString(unsigned int i)
	{
		if (i>=m_vBlocks.size())
		{
			return NULL;
		}
		return m_vBlocks[i].Mnemonic;
	}
	int GetBlockInteger(unsigned int i)
	{
		if (i>=m_vBlocks.size())
		{
			return NULL;
		}
		return atoi(m_vBlocks[i].Mnemonic);
	}
	vector<CubeBlockType> m_vBlocks;
};

using namespace std;

class CubeGrammar
{
public: 
	CubeGrammar()
	{
		Free();
	}
	~CubeGrammar()
	{
		Free();
	}

	void SetLexer(CubeLexer *Lexer)
	{
		if(Lexer)
		m_Lexer=Lexer;
	}

	void RegisterBlockType(CubeBlockType Type)
	{
		for (unsigned int i=0;i<m_vRegBlock.size();i++)
		{
			if (m_vRegBlock[i]==Type)
			{
				return;
			}
		}
		m_vRegBlock.push_back(Type);
		return;
	}


	void Free()
	{
		m_Lexer=NULL;
		m_vRegBlock.clear();
		m_vRegGammarSentence.clear();
		m_vDiscard.clear();
	}

	CubeBlockType  GetBlockByMne(char *Mne)
	{
		for (unsigned int i=0;i<m_vRegBlock.size();i++)
		{
			if (m_vRegBlock[i]==Mne)
			{
				return m_vRegBlock[i];
			}
		}
		//Undefined
		return CubeBlockType(Mne,1,GRAMMAR_UNDEFINED);
	}

	bool MatchAt(CubeGrammarSentence &ResStream,CubeGrammarSentence &DesStream,int index)
	{
		return ResStream.m_vBlocks[index]>>DesStream.m_vBlocks[index];
	}

	bool IsDiscard(CubeBlockType &Block)
	{
		for (unsigned int i=0;i<m_vDiscard.size();i++)
		{
			if (Block>>m_vDiscard[i])
			{
				return true;
			}
		}
		return false;
	}

	unsigned int RegisterSentence(CubeGrammarSentence &Str)
	{
		m_vRegGammarSentence.push_back(Str);
		return m_vRegGammarSentence.size()-1;
	}

	unsigned int GetNextInstr(CubeGrammarSentence &Stream)
	{
		m_vIst.clear();
		Stream.Reset();
		for (unsigned int i=0;i<m_vRegGammarSentence.size();i++)
		{
			m_vIst.push_back(i);
		}
		list<unsigned int>::iterator it;
		int Offset=0;

		while(!m_vIst.empty())
		{
			CubeBlockType Tok;

			switch(m_Lexer->GetNextLexeme())
			{
			case CUBE_LEXER_LEXEME_TYPE_CONATINER:
				{
					unsigned int ContainerIndex=m_Lexer->GetContainerType(m_Lexer->GetLexemeString());
					Tok.Set(m_Lexer->GetLexemeString(),2,GRAMMAR_CONATINER,ContainerIndex);
				}
				break;
			case CUBE_LEXER_LEXEME_TYPE_DELIMITER:
				{
					unsigned int DelimiterIndex=m_Lexer->GetDelimiterType(m_Lexer->GetLexemeString()[0]);
					Tok.Set(m_Lexer->GetLexemeString(),1,GRAMMAR_DELIMITER,DelimiterIndex);
				}
				break;
			case CUBE_LEXER_LEXEME_TYPE_NEWLINE:
				Tok.Set(m_Lexer->GetLexemeString(),1,GRAMMAR_NEWLINE);
				break;
			case CUBE_LEXER_LEXEME_TYPE_SPACER:
				Tok.Set(m_Lexer->GetLexemeString(),1,GRAMMAR_SPACER);
				break;
			case CUBE_LEXER_LEXEME_TYPE_TOKEN:
				Tok=GetBlockByMne(m_Lexer->GetLexemeString());
				break;
			case CUBE_LEXER_LEXEME_TYPE_END:
				//Set "END" as new line directly
				return (unsigned int)(-1);
				break;
			}

			//Discard type
			if (IsDiscard(Tok))
			{
			continue;
			}
			//Container string clip
			if(Tok.BlockTypeTree[0]==GRAMMAR_CONATINER)
			{
				strcpy(Tok.Mnemonic,m_Lexer->GetLexemeString()+1);
				Tok.Mnemonic[strlen(Tok.Mnemonic)-1]='\0';
			}
			else
			strcpy(Tok.Mnemonic,m_Lexer->GetLexemeString());

			it = m_vIst.begin();
			while (true)
			{
				if (!(Tok>>m_vRegGammarSentence[*it].m_vBlocks[Offset]))
				{
					list<unsigned int>::iterator rm;
					rm=it;
					it++;
					m_vIst.erase(rm);
				}
				else
				{
					if(Offset==(m_vRegGammarSentence[*it].GetBlocksCount()-1))
					{
						Stream.add(Tok);
						return *it;
					}
					it++;
				}

				if (it==m_vIst.end())
				{
					break;
				}
			}
			Stream.add(Tok);
			Offset++;
		}

		return GRAMMAR_SENTENCE_UNKNOW;

	}

	void RegisterDiscard(CubeBlockType &Type)
	{
		m_vDiscard.push_back(Type);
	}

private: 
	CubeLexer					*m_Lexer;
	vector<CubeBlockType>		m_vRegBlock;
	vector<CubeBlockType>		m_vDiscard;
	vector<CubeGrammarSentence>	m_vRegGammarSentence;
	
	list<unsigned int>	m_vIst;

	
};