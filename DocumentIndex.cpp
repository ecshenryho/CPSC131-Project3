#include	<fstream>
#include	<iostream>
#include	<string>
#include	<sstream>
// #include	<unistd.h>

#include	"DocumentIndex.h"
#include	"GetLine.h"
#include	<cctype>
using namespace std;
typedef	string::size_type	StringSize;
//****************************************************************************************
//
//	DocumentFile::Close
//
//****************************************************************************************
void	DocumentFile::Close()
{
	file_.close();
	file_.clear();
	return;
}
//****************************************************************************************
//
//	DocumentFile::GetPageNumber
//
//****************************************************************************************
int	DocumentFile::GetPageNumber()
{
	return(pageNumber_);
}
//****************************************************************************************
//
//	DocumentFile::GetWord
//
//****************************************************************************************
string	DocumentFile::GetWord()
{
	//************************************************************************************
	//	LOCAL DATA
	string	word;//holding each word after breaking out from a line
	string tempWord;//holding a current legal word.
	char nextChar;//holding each character in a word for checking legal or not
	string specialChar(".,/-");//special characters 
		//************************************************************************************
		//	EXECUTABLE STATEMENTS
	iss >>word;
	for (unsigned int i = 0; i < word.length(); i++)//go through each character in a word
	{
		if (word[i]=='-'||word[i]=='/'||word[i]=='*')//if the word contain a -,/,* ? 
		{
			return GetWord();//if yes skip it go to next word
		}

		nextChar = word[i];
		//word ending with specials characters?
		if (i != word.length() - 1 && specialChar.find_first_of(nextChar) != std::string::npos) return GetWord();

		if ((int)word[i] == 39 && word[i + 1] == 's')
		{
			break;
		}
		if (nextChar != '('&& nextChar != '{')//if next character is not ( or {
		{
			if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z'))
			{
				tempWord.push_back(nextChar);//push legal character into temporary tempWord variable
			}
		}
	}
	if (word.find_first_of("0123456789")!=std::string::npos)//check if a word contain numbers
	{
		return GetWord();//if yes skip it go to next word
	}
	
	for (unsigned int i = 0; i < ExclusionWords.size(); i++)//go through the exclusion words vector to skip it
	{
		if (tempWord == ExclusionWords[i])//compare with exclusion words vector
		{
			return GetWord();//get next word if found
		}
	}
	word = tempWord;// assign to word a final legal word.
	tempWord.clear();//clear tempWord for re-used.
	return(word);
}
//****************************************************************************************
//
//	DocumentFile::LoadExclusions
//
//****************************************************************************************
bool	DocumentFile::LoadExclusions(const string& name)
{
	//************************************************************************************
	//	LOCAL DATA
	bool	success=false;//initialize success to false
	string word;//hodling a word
	ifstream myFile;//ifstream object
	myFile.open(name);//open the file with given name
	while (myFile>>word)//read in word from input file
	{
		ExclusionWords.push_back(word);//pushing exclusion word to vector.
	}
	success=true;//return true if success.
	return(success);
}
//****************************************************************************************
//
//	DocumentFile::Open
//
//****************************************************************************************
bool	DocumentFile::Open(const string& name)
{
	file_.open(name, ios::in);
	if (!file_.fail())
		return(true);
	else
		return(false);
}
//****************************************************************************************
//
//	DocumentFile::Read
//
//****************************************************************************************
bool DocumentFile::Read()
{
	bool	success = false;//initialize success to false

	success = GetLine(file_, text_);//read the first line from file

	if (text_.empty())//if empty line
	{
		success = GetLine(file_, text_);//read next line

		if (text_.empty())//if empty line
		{
			success = GetLine(file_, text_);//read next line, now we just went over 2 empty lines
			pageNumber_++;//increase page number
		}
	}
	iss.clear();//clear before re-used
	iss.str(text_);//getting a line
	return(success);
}
//****************************************************************************************
//
//	DocumentIndex::Create
//
//****************************************************************************************
void	DocumentIndex::Create(DocumentFile& documentFile)
{
	string word;//holding a word
	int pageNumber;//holding page number
	
	bool success;
	while (true)
	{
		success = documentFile.Read();//call funtion Read() to read the file
		if (!success)
			break;
		while (true)
		{
			word = documentFile.GetWord();//call function GetWord() to get a word
			if (word.empty())
				break;
			pageNumber = documentFile.GetPageNumber();// call function GetPageNumber() to get page number
			
			if (TempIndex.find(word) == TempIndex.end())//if the word is not on the temporary index
			{
				vector<int> v;//create vector holding page number for the word
				v.push_back(pageNumber);//add page number to vector
				TempIndex.insert(std::make_pair(word, v));//insert new pair index into temporary map
			}
			else//if the word was found in the map
			{
				TempIndex[word].push_back(pageNumber);//only push back the new page number for that word
			}
		}
	}

	for (auto it = TempIndex.begin(); it != TempIndex.end(); ++it)//go through the temporary map
	{
		if (it->second.size() < 11)//only take words that repeated no more than 10 times.
		{
			//set will makes everthing in order and no repeated page number.
			set<int> set(it->second.begin(), it->second.end());//put the whole vector pages' number into a set.
			Index.insert(std::make_pair(it->first, set));//make a pair to insert into final index map.
		}
	}
	return;
}
//****************************************************************************************
//
//	DocumentIndex::Write
//
//****************************************************************************************
void	DocumentIndex::Write(ostream& indexStream)
{
	for (auto it = Index.begin(); it != Index.end(); ++it)//go through the final index map write out require output.
	{
		auto its = it->second.begin();
		indexStream << it->first << " "<< *its;
		its++;
		for (; its != it->second.end(); ++its)
		{
			indexStream <<", "<< *its;
		}
		indexStream<<endl;
	}
	return;
}
