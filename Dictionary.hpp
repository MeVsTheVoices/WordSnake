#pragma once

#include <set>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class Dictionary {
	const wxString DICTIONARY_FILE_NAME { "Dictionary.txt" };
	std::set<wxString> mDictionary;
	
public:
	void initialize();
	bool isWord(wxString str);
};

