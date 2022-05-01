#include "Alphabet.hpp"

wxUniChar Alphabet::getRandomLetter() {
	if (!HAVE_DISTRIBUTIONS_BEEN_CALCULATED) {
		const auto& distribution = this->getAlphabetDistribution();
		mRandomDistribution = std::discrete_distribution<int>(distribution.begin(), distribution.end());
		HAVE_DISTRIBUTIONS_BEEN_CALCULATED = true;
	}
	const auto& alphabet = this->getAlphabet();
	return alphabet[mRandomDistribution(mRandomEngine)];
}

int Alphabet::calculateLetterScore(wxUniChar c) {
	if (!HAVE_SCORES_BEEN_CALCULATED) calculateLetterScores();
	const auto alphabetCeil = this->getAlphabetCeil();
	const auto& alphabet = this->getAlphabet();
	auto& alphabetScores = this->getAlphabetScores();
	auto scoreAt = alphabetScores[c];
	return scoreAt;
}

int Alphabet::calculateScore(const wxString& str) {
	if (!HAVE_SCORES_BEEN_CALCULATED) calculateLetterScores();
	int sum = 0;
	for (int i = 0; i < str.length(); i++) {
		auto charAt = str.at(i);
		auto scoreAt = calculateLetterScore(charAt);
		sum += scoreAt * (i + 1);
	}
	return sum;
}

void Alphabet::calculateLetterScores() {
	std::map<wxUniChar, int> alphabetScores;
	const auto& alphabetDistribution = this->getAlphabetDistribution();
	const auto& alphabet = this->getAlphabet();
	for (int i = 0; i < this->getAlphabetCeil(); i++) {
		float letterScore;
		letterScore = alphabetDistribution[i];
		letterScore = 1 / letterScore;
		letterScore *= 1000.0f;
		letterScore += 5;
		alphabetScores[alphabet[i]] = static_cast<int>(letterScore);
	}
	this->setAlphabetScores(alphabetScores);
	HAVE_SCORES_BEEN_CALCULATED = true;
}