#pragma once

#include <wx/string.h>
#include <vector>
#include <map>

struct Alphabet {
public:
	virtual ~Alphabet() {}
	virtual int getAlphabetCeil() const = 0;
	virtual const std::vector<wxChar>& getAlphabet() const = 0;
	virtual const std::vector<int>& getAlphabetDistribution() const = 0;
	virtual std::map<wxUniChar, int>& getAlphabetScores() = 0;

	virtual int calculateScore(const wxString& str) {
		wxLogDebug("alphabet called with %s", str);
		if (!HAVE_SCORES_BEEN_CALCULATED) calculateLetterScores();
		int sum = 0;
		const auto alphabetCeil = this->getAlphabetCeil();
		const auto &alphabet = this->getAlphabet();
		auto &alphabetScores = this->getAlphabetScores();
		for (int i = 0; i < str.length(); i++) {
			auto charAt = str.at(i);
			auto scoreAt = alphabetScores[charAt];
			sum += scoreAt * (i + 1);
		}
		return sum;
	}

protected:

	virtual void setAlphabetScores(std::map<wxUniChar, int> scores) = 0;

	virtual void calculateLetterScores() {
		std::map<wxUniChar, int> alphabetScores;
		const auto &alphabetDistribution = this->getAlphabetDistribution();
		const auto &alphabet = this->getAlphabet();
		for (int i = 0; i < this->getAlphabetCeil(); i++) {
			float letterScore;
			letterScore = alphabetDistribution[i];
			letterScore = 1 / letterScore;
			letterScore *= 1000.0f;
			alphabetScores[alphabet[i]] = static_cast<int>(letterScore);
		}
		HAVE_SCORES_BEEN_CALCULATED = true;
		this->setAlphabetScores(alphabetScores);
	}

	bool HAVE_SCORES_BEEN_CALCULATED = false;
};

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
		for (auto i : mAlphabetScores)
			wxLogDebug("%c %i", i.first, i.second);
		return mAlphabetScores;
	}

protected:

	virtual void setAlphabetScores(std::map<wxUniChar, int> scores) {
		mAlphabetScores = scores;
	}

	int mAlphabetCeil = 26;
	std::vector<wxChar> mAlphabet {
		'A', 'B', 'C', 'D', 'E',
		'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y',
		'Z'
	};
	std::vector<int> mAlphabetDistribution {
		820, 150, 270, 470, 1300,
		220, 20,  620, 690, 14,
		78,  410, 270, 670, 780,
		190, 11,  590, 620, 960,
		270, 97,  240, 15,  200,
		8
	};
	std::map<wxUniChar, int> mAlphabetScores;
};
