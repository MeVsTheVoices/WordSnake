#pragma once

#include "Alphabet.hpp"

class EnglishAlphabet : public virtual Alphabet {
public:
	virtual int getAlphabetCeil() const {
		return mAlphabetCeil;
	}

	virtual const std::vector<wxChar>& getAlphabet() const {
		return mAlphabet;
	}

	virtual const std::vector<int>& getAlphabetDistribution() const {
		return mAlphabetDistribution;
	}

	virtual std::map<wxUniChar, int>& getAlphabetScores() {
		if (!HAVE_SCORES_BEEN_CALCULATED)
			calculateLetterScores();
		return mAlphabetScores;
	}

protected:

	virtual void setAlphabetScores(std::map<wxUniChar, int> scores) {
		mAlphabetScores = scores;
	}

	int mAlphabetCeil = 26;
	std::vector<wxChar> mAlphabet{
		'A', 'B', 'C', 'D', 'E',
		'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y',
		'Z'
	};
	std::vector<int> mAlphabetDistribution{
		820, 150, 270, 470, 1300,
		220, 20,  620, 690, 14,
		78,  410, 270, 670, 780,
		190, 11,  590, 620, 960,
		270, 97,  240, 15,  200,
		8
	};
	std::map<wxUniChar, int> mAlphabetScores;
};
