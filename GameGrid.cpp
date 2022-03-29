#include "GameGrid.hpp"

#include <vector>

GameGrid::GameGrid() : wxFrame(nullptr, wxID_ANY, "WordSnake") {
	//the next few lines create a way of generating random characters based
	//on the distribution of words in the language
	mRandomEngine = new std::default_random_engine(std::random_device{}());

	//setup for the alphabet and the generation of random letters
	std::vector<int> mAlphabetDistributionVector;
	std::copy(
		std::begin(mAlphabetDistribution),
		std::end(mAlphabetDistribution),
		std::back_inserter(mAlphabetDistributionVector));

	mRandomDistribution = new std::discrete_distribution<int> { 
		mAlphabetDistributionVector.begin(), mAlphabetDistributionVector.end()
	};

	//go ahead and load up the dictionary from the file specified
	mDictionary.initialize();

	//currently the score of each letter is the inverse of how common it is
	// TODO: change how scores are calculated, increases for longer words, normalize a little so that for example, z isn't worth 1000x an e
	for (int i = 0; i < mAlphabetCeil; i++) {
		mAlphabetScores[i] = 
			static_cast<int>(
				(1.0f/static_cast<float>(mAlphabetDistribution[i]) * 1000.0f));
	}

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	// TODO: tidy all this garbage up, maybe subclass, or create a factory for button classes
	wxFlexGridSizer* flexGrid = new wxFlexGridSizer(0, 1, 0, 0);
	flexGrid->SetFlexibleDirection(wxBOTH);
	flexGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	/*//create as many buttons as are needed to populate the grid
	mButtons = new wxToggleButton * [GRID_HEIGHT * GRID_WIDTH];
	mValues = new wxChar[GRID_HEIGHT * GRID_WIDTH];

	wxGridSizer* grid = new wxGridSizer(GRID_HEIGHT, GRID_WIDTH, 0, 0);

	wxFont aFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	//populate the grid with buttons, generate random letters for each, and bind their even methods
	for (int i = 0; i < GRID_HEIGHT; i++) {
		for (int j = 0; j < GRID_WIDTH; j++) {
			mButtons[(i * GRID_WIDTH) + j] = new wxToggleButton(
				this, 
				10000 + (i * GRID_WIDTH) + j,
				std::to_string(i) + ' ' + std::to_string(j));
			mButtons[(i * GRID_WIDTH) + j]->SetFont(aFont);

			grid->Add(mButtons[(i * GRID_WIDTH) + j], 1, wxEXPAND | wxALL);

			mButtons[(i * GRID_WIDTH) + j]->Bind(wxEVT_TOGGLEBUTTON, &GameGrid::OnButtonClicked, this);

			mValues[(i * GRID_WIDTH) + j] = randomLetter();

			mButtons[(i * GRID_WIDTH) + j]->SetLabel(mValues[(i * GRID_WIDTH) + j]);
		}
	}*/

	wxFont gridButtonFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	mToggleButtonGrid = new ToggleButtonGrid(this, 10, 10, gridButtonFont);

	flexGrid->Add(mToggleButtonGrid, 1, wxEXPAND, 5);

	//all this holds the Ok and Cancel buttons
	wxStdDialogButtonSizer* sdbSizer = new wxStdDialogButtonSizer();
	sdbSizer->AddButton(new wxButton(this, wxID_OK));
	sdbSizer->GetAffirmativeButton()->Bind(wxEVT_BUTTON, &GameGrid::OnOkButtonClicked, this);
	sdbSizer->AddButton(new wxButton(this, wxID_CANCEL));
	sdbSizer->GetCancelButton()->Bind(wxEVT_BUTTON, &GameGrid::OnCancelButtonClicked, this);
	sdbSizer->SetMinSize(wxSize(100, 100));
	sdbSizer->Realize();

	flexGrid->Add(sdbSizer, 1, wxALIGN_CENTER, 5);

	wxBoxSizer* scorePreviewBoxSizer;
	scorePreviewBoxSizer = new wxBoxSizer(wxVERTICAL);

	mScorePreview = new wxStaticText(this, wxID_ANY, wxT("static_text_score_preview"), wxDefaultPosition, wxDefaultSize, 0);
	mScorePreview->Wrap(-1);
	
	wxFont scorePreviewFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	mScorePreview->SetLabel("Currently selected word score");
	mScorePreview->SetFont(scorePreviewFont);
	scorePreviewBoxSizer->Add(mScorePreview, 0, wxALL | wxALIGN_CENTER, 5);
	scorePreviewBoxSizer->Layout();

	flexGrid->Add(scorePreviewBoxSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxFIXED_MINSIZE, 5);

	//thank you to doublema on the wxWidgets forums for the fix
	this->SetSizerAndFit(flexGrid);
	this->Layout();
}


GameGrid::~GameGrid() {
	//clean up
	delete[] mButtons;
	delete[] mValues;
	delete mRandomDistribution;
	delete mRandomEngine;
	delete mToggleButtonGrid;
}

#include <iostream>

void GameGrid::OnButtonClicked(wxCommandEvent& evt) {
	//get the grid position based on the id we set earlier
	int x = (evt.GetId() - 10000) % GRID_WIDTH;
	int y = (evt.GetId() - 10000) / GRID_WIDTH;

	wxLogDebug("button %i %i was clicked", x, y);

	if (mCurrentSelection.size() == 0) {
		mCurrentSelection.push_back(std::pair<int, int>(x, y));
		wxLogDebug("added as first selected");
	}
	else if (mCurrentSelection.size() > 0) {
		auto lastClicked = mCurrentSelection.back();
		if ((lastClicked.first == x) && (lastClicked.second == y)) {
			mButtons[y * GRID_WIDTH + x]->SetValue(false);
			wxLogDebug("removing last clicked");
			mCurrentSelection.pop_back();
			return;
		}
		if (mCurrentSelection.size() > 1) {
			for (auto i = mCurrentSelection.begin(); i != std::prev(mCurrentSelection.end()); i++) {
				if ((i->first == x) && (i->second == y)) {
					mButtons[y * GRID_WIDTH + x]->SetValue(true);
					wxLogDebug("resetting clicked that wasn't tail");
					return;
				}
			}
		}
		if (isAdjacent(x, y, lastClicked.first, lastClicked.second)) {
			wxLogDebug("adding adjacent");
			mCurrentSelection.push_back(std::pair<int, int>(x, y));
		}
		else {
			wxLogDebug("resetting clicked that wasn't adjacent");
			mButtons[y * GRID_WIDTH + x]->SetValue(false);
		}
	}
}

void GameGrid::OnOkButtonClicked(wxCommandEvent& evt) {
	bool isWord = mDictionary.isWord(getSelectedString());
	int wordScore = getWordScore(getSelectedString());
	wxLogDebug("ok button pressed");
	wxLogDebug(getSelectedString());
	wxLogDebug("%s", isWord ? " is a word" : " is not a word");
	wxLogDebug("word score %i", wordScore);
	if (isWord) {
		replaceCurrentSelection();
		clearCurrentSelection();
		mCurrentScore += wordScore;
		wxLogDebug("current score now %i", mCurrentScore);
	}
}

void GameGrid::OnCancelButtonClicked(wxCommandEvent& evt) {
	wxLogDebug("cancel button pressed");
	clearCurrentSelection();
}

wxChar GameGrid::randomLetter() const {
	//return a random character in the alphabet, frequency distribution is based on the occurance
	//of those letters within words within the language
	return mAlphabet[mRandomDistribution->operator()(*mRandomEngine)];
}

bool GameGrid::isAdjacent(int x1, int y1, int x2, int y2) {
	wxLogDebug("(%i, %i), (%i, %i)", x1, y1, x2, y2);
	if (x1 == x2)
		if ((y1 == (y2 - 1)) || (y1 == (y2 + 1)))
			return true;
	if (y1 == y2)
		if ((x1 == (x2 - 1)) || (x1 == (x2 + 1)))
			return true;
	return false;
}

void GameGrid::replaceCurrentSelection() {
	for (auto i : mCurrentSelection) {
		wxChar newChar = randomLetter();
		mValues[i.second * GRID_WIDTH + i.first] = newChar;
		mButtons[i.second * GRID_WIDTH + i.first]->SetLabel(newChar);
	}
}

wxString GameGrid::getSelectedString() const {
	wxString str;
	for (auto i : mCurrentSelection)
		str.Append(mValues[i.second * GRID_WIDTH + i.first]);
	return str;
}

void GameGrid::clearCurrentSelection() {
	for (auto i : mCurrentSelection)
		mButtons[i.second * GRID_WIDTH + i.first]->SetValue(false);
	mCurrentSelection.clear();
}

int GameGrid::getWordScore(wxString str) {
	int sum = 0;
	for (int i = 0; i < str.length(); i++)
		for (int j = 0; j < mAlphabetCeil; j++)
			if (mAlphabet[j] == str[i])
				sum += mAlphabetScores[j];
	return sum;
}
