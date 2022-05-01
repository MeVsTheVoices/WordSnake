#include "GameGrid.hpp"

#include <vector>

GameGrid::GameGrid() : wxFrame(nullptr, wxID_ANY, "WordSnake") {
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	//create and populate the menu bar
	auto menuBar = new wxMenuBar(0);
	menuBar->SetLabel("Game");
	auto menuOne = new wxMenu();
	menuOne->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent& ev) {
		mCurrentScore = 0;
		clearCurrentSelection();
		setGauges();
	});
	auto menuOneItemOne = new wxMenuItem(menuOne, wxID_NEW, wxString(wxT("New game")), wxEmptyString, wxITEM_NORMAL);
	menuOne->Append(menuOneItemOne);
	menuBar->Append(menuOne, wxT("Game menu"));
	this->SetMenuBar(menuBar);

	// TODO: tidy all this garbage up, maybe subclass, or create a factory for button classes
	wxFlexGridSizer* flexGrid = new wxFlexGridSizer(0, 1, 0, 0);
	flexGrid->SetFlexibleDirection(wxBOTH);
	flexGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	wxFont gridButtonFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	mToggleButtonGrid = new ToggleButtonGrid(this, 10, 10, gridButtonFont);

	for (int i = 0; i < GRID_HEIGHT; i++) {
		for (int j = 0; j < GRID_WIDTH; j++) {
			mToggleButtonGrid->getToggleButtonByIndex(i, j)->SetLabelText(mDictionary.getRandomLetter());
		}
	}

	using namespace std::placeholders;

	mToggleButtonGrid->registerButtonEventListener(std::bind(&GameGrid::handleToggleButtonEvent, this, _1, _2, _3, _4));
	flexGrid->Add(mToggleButtonGrid, 1, wxEXPAND, 5);

	//all this holds the Ok and Cancel buttons
	wxFont sdbSizerFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_MAX, wxFONTWEIGHT_LIGHT);
	wxStdDialogButtonSizer* sdbSizer = new wxStdDialogButtonSizer();
	sdbSizer->AddButton(new wxButton(this, wxID_OK));
	sdbSizer->GetAffirmativeButton()->SetLabelText("Submit word");
	sdbSizer->GetAffirmativeButton()->Bind(wxEVT_BUTTON, &GameGrid::OnOkButtonClicked, this);
	sdbSizer->GetAffirmativeButton()->SetFont(sdbSizerFont);
	sdbSizer->GetAffirmativeButton()->SetDefault();

	sdbSizer->AddButton(new wxButton(this, wxID_CANCEL));
	sdbSizer->GetCancelButton()->SetLabelText("Clear selection");
	sdbSizer->GetCancelButton()->Bind(wxEVT_BUTTON, &GameGrid::OnCancelButtonClicked, this);
	sdbSizer->GetCancelButton()->SetFont(sdbSizerFont);

	sdbSizer->SetMinSize(wxSize(100, 100));
	sdbSizer->Realize();
	flexGrid->Add(sdbSizer, 1, wxALIGN_CENTER, 5);

	auto scoreSizer = new wxFlexGridSizer(1, 5, 5);
	scoreSizer->SetFlexibleDirection(wxBOTH);

	//create the two gauges that reside on the bottom, one for the current score of the words, one for the players score total
	mTotalScoreGauge = new wxGauge(this, wxID_ANY, mScoreGaugeMax);
	mWordScoreGauge = new wxGauge(this, wxID_ANY, mScoreGaugeMax);
	mTotalScoreGauge->SetValue(0);
	mTotalScoreGauge->SetLabel("Game score");
	mWordScoreGauge->SetValue(0);
	mTotalScoreGauge->SetLabel("Word score");
	scoreSizer->Add(mTotalScoreGauge);
	scoreSizer->Add(mWordScoreGauge);
	scoreSizer->Layout();

	flexGrid->Add(scoreSizer, 1, wxEXPAND | wxALL | wxALIGN_CENTRE_HORIZONTAL, 5);

	//thank you to doublema on the wxWidgets forums for the fix
	this->SetSizerAndFit(flexGrid);
	this->Layout();
}


GameGrid::~GameGrid() {
	//clean up
	delete[] mButtons;
	delete[] mValues;
	delete mToggleButtonGrid;
}

#include <iostream>

void GameGrid::OnOkButtonClicked(wxCommandEvent& evt) {
	bool isWord = mDictionary.isWord(getSelectedString());
	int wordScore = mDictionary.getWordScore(getSelectedString());
	wxLogDebug(getSelectedString());
	wxLogDebug("%s", isWord ? " is a word" : " is not a word");
	wxLogDebug("word score %i", wordScore);
	if (isWord) {
		replaceCurrentSelection();
		clearCurrentSelection();
		mCurrentScore += wordScore;
		wxLogDebug("current score now %i", mCurrentScore);
	}
	setGauges();
}

void GameGrid::OnCancelButtonClicked(wxCommandEvent& evt) {
	wxLogDebug("cancel button pressed");
	clearCurrentSelection();
	setGauges();
}

bool GameGrid::handleToggleButtonEvent(wxCommandEvent& evt, int x, int y, wxToggleButton* button) {
	wxLogDebug("button %i %i was clicked", x, y);

	//if we're the first button to be clicked on since there having been none selected
	if (mCurrentSelection.size() == 0) {
		mCurrentSelection.push_back(std::pair<int, int>(x, y));
		wxLogDebug("added as first selected");
	}
	//if we're not the first button in the sequence to be clicked
	else if (mCurrentSelection.size() > 0) {
		auto lastClicked = mCurrentSelection.back();
		//if we were the last button in the sequence to be clicked
		if ((lastClicked.first == x) && (lastClicked.second == y)) {
			button->SetValue(false);
			wxLogDebug("removing last clicked");
			mCurrentSelection.pop_back();
			setGauges();
			return false;
		}
		//if we were not the tail of the sequence
		if (mCurrentSelection.size() > 1) {
			for (auto i = mCurrentSelection.begin(); i != std::prev(mCurrentSelection.end()); i++) {
				if ((i->first == x) && (i->second == y)) {
					mToggleButtonGrid->getToggleButtonByIndex(x, y)->SetValue(true);
					wxLogDebug("resetting clicked that wasn't tail");
					return false;
				}
			}
		}
		//if we weren't in the sequence, check if we're adjacted to the tail
		if (isAdjacent(x, y, lastClicked.first, lastClicked.second)) {
			wxLogDebug("adding adjacent");
			mCurrentSelection.push_back(std::pair<int, int>(x, y));
			setGauges();
		}
		else {
			wxLogDebug("resetting clicked that wasn't adjacent");
			mToggleButtonGrid->getToggleButtonByIndex(x, y)->SetValue(false);
		}
	}
	return false;
}

bool GameGrid::isAdjacent(int x1, int y1, int x2, int y2) {
	//tests for adjacency, two tiles are adjacent if they are in either the same row or column
	//and they are either +1 or -1 in whatever direction they aren't in the same as
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
	//replaces all tiles in current selection with new letter
	for (auto i : mCurrentSelection) {
		wxUniChar newChar = mDictionary.getRandomLetter();
		mToggleButtonGrid->getToggleButtonByIndex(i.first, i.second)->SetLabel(newChar);
	}
}

wxString GameGrid::getSelectedString() const {
	wxString str;
	for (auto i : mCurrentSelection)
		str.Append(mToggleButtonGrid->getToggleButtonByIndex(i.first, i.second)->GetLabelText());
	return str;
}

void GameGrid::clearCurrentSelection() {
	for (auto i : mCurrentSelection)
		mToggleButtonGrid->getToggleButtonByIndex(i.first, i.second)->SetValue(false);
	mCurrentSelection.clear();
}

int GameGrid::getWordScore(wxString str) {
	return mDictionary.getWordScore(str);
}

void GameGrid::setGauges() {
	static const int wordsInGame = 10;
	int wordScore = getWordScore(getSelectedString());
	setGauge(mWordScoreGauge, wordScore);
	setGauge(mTotalScoreGauge, mCurrentScore/wordsInGame);
}

#include <cmath>

void GameGrid::setGauge(wxGauge* gauge, int value) {
	static const auto xTermCoef = 0.08f;
	static const auto xTermBias = 5.5f;

	float eTerm = exp((-xTermCoef * value) + xTermBias);
	float bottomTerm = 1 + eTerm;
	float topTerm = gauge->GetRange();
	gauge->SetValue(static_cast<int>(topTerm / bottomTerm));
}
