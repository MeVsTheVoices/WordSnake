#include "Dictionary.hpp"

Dictionary::Dictionary() {
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

	mAlphabet = new EnglishAlphabet;
}

Dictionary::~Dictionary() {
	delete mAlphabet;
}

int Dictionary::getWordScore(const wxString& str) const {
	return mAlphabet->calculateScore(str);
}

bool Dictionary::isWord(wxString str) {
	str.MakeLower();
	return !(mDictionary.count(str) == 0);
}

wxString Dictionary::getWordScoreBreakdown(const wxString& word) const {
	wxString str;
	if (word.length() == 0)
		return wxString("No current word score");
	str.append("Current word score: ");
	for (int i = 0; i < word.length(); i++) {
		str.append("(");
		str.append(word.at(i));
		str.append(": ");
		str.append(std::to_string(mAlphabet->calculateLetterScore(word.at(i))));
		str.append(") * ");
		str.append(std::to_string(i + 1));
		if (i < (word.length() - 1))
			str.append(" + ");
	}
	return str;
}

wxUniChar Dictionary::getRandomLetter() {
	return mAlphabet->getRandomLetter();
}
