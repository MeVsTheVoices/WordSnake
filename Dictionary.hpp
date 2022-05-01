#pragma once

#include <set>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include "EnglishAlphabet.hpp"

class Dictionary {
private:
	const wxString DICTIONARY_FILE_NAME { "Dictionary.txt" };
	std::set<wxString> mDictionary;
	Alphabet* mAlphabet;
	
public:
	Dictionary();
	~Dictionary();
	int getWordScore(const wxString& str) const;
	bool isWord(wxString str);
	wxString getWordScoreBreakdown(const wxString& word) const;
	wxUniChar getRandomLetter();
};

