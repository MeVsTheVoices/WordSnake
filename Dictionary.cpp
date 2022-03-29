#include "Dictionary.hpp"

void Dictionary::initialize() {
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

bool Dictionary::isWord(wxString str) {
	str.MakeLower();
	return !(mDictionary.count(str) == 0);
}