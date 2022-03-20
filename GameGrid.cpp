#include "GameGrid.hpp"

#include <vector>

GameGrid::GameGrid() : wxFrame(nullptr, wxID_ANY, "WordSnake") {
	//the next few lines create a way of generating random characters based
	//on the distribution of words in the language
	mRandomEngine = new std::default_random_engine(std::random_device{}());

	std::vector<int> mAlphabetDistributionVector;
	std::copy(
		std::begin(mAlphabetDistribution),
		std::end(mAlphabetDistribution),
		std::back_inserter(mAlphabetDistributionVector));

	mRandomDistribution = new std::discrete_distribution<int> { 
		mAlphabetDistributionVector.begin(), mAlphabetDistributionVector.end()
	};

	mDictionary.initialize();

	for (int i = 0; i < mAlphabetCeil; i++) {
		mAlphabetScores[i] = 
			static_cast<int>(
				(1.0f/static_cast<float>(mAlphabetDistribution[i]) * 1000.0f));
	}

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* flexGrid = new wxFlexGridSizer(0, 1, 0, 0);
	flexGrid->SetFlexibleDirection(wxBOTH);
	flexGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	//create as many buttons as are needed to populate the grid
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
	}

	flexGrid->Add(grid, 1, wxEXPAND, 5);

	wxStdDialogButtonSizer* sdbSizer = new wxStdDialogButtonSizer();
	sdbSizer->AddButton(new wxButton(this, wxID_OK));
	sdbSizer->GetAffirmativeButton()->Bind(wxEVT_BUTTON, &GameGrid::OnOkButtonClicked, this);
	sdbSizer->AddButton(new wxButton(this, wxID_CANCEL));
	sdbSizer->GetCancelButton()->Bind(wxEVT_BUTTON, &GameGrid::OnCancelButtonClicked, this);
	sdbSizer->SetMinSize(wxSize(100, 100));
	sdbSizer->Realize();

	flexGrid->Add(sdbSizer, 1, wxALIGN_CENTER, 5);

	//add the sizer to the frame and set it to lay itself out
	this->SetSizer(flexGrid);
	this->SetSize(flexGrid->GetMinSize());
	this->Layout();
}


GameGrid::~GameGrid() {
	//clean up
	delete[] mButtons;
	delete[] mValues;
	delete mRandomDistribution;
	delete mRandomEngine;
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
	wxLogDebug("ok button pressed");
	wxLogDebug(getSelectedString());
	wxLogDebug("%s", mDictionary.isWord(getSelectedString()) ? " is a word" : " is not a word");
	wxLogDebug("word score %i", getWordScore(getSelectedString()));
}

void GameGrid::OnCancelButtonClicked(wxCommandEvent& evt) {
	wxLogDebug("cancel button pressed");
	for (auto i : mCurrentSelection)
		mButtons[i.second * GRID_WIDTH + i.first]->SetValue(false);
	mCurrentSelection.clear();
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

wxString GameGrid::getSelectedString() const {
	wxString str;
	for (auto i : mCurrentSelection)
		str.Append(mValues[i.second * GRID_WIDTH + i.first]);
	return str;
}

int GameGrid::getWordScore(wxString str) {
	int sum = 0;
	for (int i = 0; i < str.length(); i++)
		for (int j = 0; j < mAlphabetCeil; j++)
			if (mAlphabet[j] == str[i])
				sum += mAlphabetScores[j];
	return sum;
}
