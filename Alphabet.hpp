#pragma once

#include <wx/string.h>
#include <vector>
#include <map>
#include <random>

struct Alphabet {
public:
	virtual ~Alphabet() {}
	virtual int getAlphabetCeil() const = 0;
	virtual const std::vector<wxChar>& getAlphabet() const = 0;
	virtual const std::vector<int>& getAlphabetDistribution() const = 0;
	virtual std::map<wxUniChar, int>& getAlphabetScores() = 0;

	virtual wxUniChar getRandomLetter();

	virtual int calculateLetterScore(wxUniChar c);
	virtual int calculateScore(const wxString& str);

protected:

	std::default_random_engine mRandomEngine;
	std::discrete_distribution<int> mRandomDistribution;

	virtual void setAlphabetScores(std::map<wxUniChar, int> scores) = 0;

	virtual void calculateLetterScores();

	bool HAVE_SCORES_BEEN_CALCULATED = false;
	bool HAVE_DISTRIBUTIONS_BEEN_CALCULATED = false;
};