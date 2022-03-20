#pragma once

#include <set>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class Dictionary {
	const wxString DICTIONARY_FILE_NAME { "Dictionary.txt" };
	std::set<wxString> mDictionary;
	
public:
	void initialize() {
		wxFileInputStream input(DICTIONARY_FILE_NAME);
		wxTextInputStream text(input);
		wxString line;
		if (input.Eof() || !input.CanRead() || !input.IsOk())
			wxLogDebug("problem opening %s", DICTIONARY_FILE_NAME);
		while (!input.Eof()) {
			text >> line;
			mDictionary.emplace(line);
		}
		int size = mDictionary.size();
		wxLogDebug("current %i entries in dictionary", size);
	}

	bool isWord(wxString str) {
		str.MakeLower();
		return !(mDictionary.count(str) == 0);
	}
};

