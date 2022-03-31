#include "GameGrid.hpp"

#include <vector>

GameGrid::GameGrid() : wxFrame(nullptr, wxID_ANY, "WordSnake") {
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

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

	sdbSizer->AddButton(new wxButton(this, wxID_CANCEL));
	sdbSizer->GetCancelButton()->SetLabelText("Clear selection");
	sdbSizer->GetCancelButton()->Bind(wxEVT_BUTTON, &GameGrid::OnCancelButtonClicked, this);
	sdbSizer->GetCancelButton()->SetFont(sdbSizerFont);

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
	mScorePreview->SetAutoLayout(true);
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
		mScorePreview->SetLabelText(mDictionary.getWordScoreBreakdown(getSelectedString()));
	}
}

void GameGrid::OnCancelButtonClicked(wxCommandEvent& evt) {
	wxLogDebug("cancel button pressed");
	clearCurrentSelection();
}

bool GameGrid::handleToggleButtonEvent(wxCommandEvent& evt, int x, int y, wxToggleButton* button) {
	wxLogDebug("button %i %i was clicked", x, y);

	if (mCurrentSelection.size() == 0) {
		mCurrentSelection.push_back(std::pair<int, int>(x, y));
		wxLogDebug("added as first selected");
		mScorePreview->SetLabelText(mDictionary.getWordScoreBreakdown(getSelectedString()));
	}
	else if (mCurrentSelection.size() > 0) {
		auto lastClicked = mCurrentSelection.back();
		if ((lastClicked.first == x) && (lastClicked.second == y)) {
			button->SetValue(false);
			wxLogDebug("removing last clicked");
			mCurrentSelection.pop_back();
			return false;
		}
		if (mCurrentSelection.size() > 1) {
			for (auto i = mCurrentSelection.begin(); i != std::prev(mCurrentSelection.end()); i++) {
				if ((i->first == x) && (i->second == y)) {
					mToggleButtonGrid->getToggleButtonByIndex(x, y)->SetValue(true);
					wxLogDebug("resetting clicked that wasn't tail");
					return false;
				}
			}
		}
		if (isAdjacent(x, y, lastClicked.first, lastClicked.second)) {
			wxLogDebug("adding adjacent");
			mCurrentSelection.push_back(std::pair<int, int>(x, y));
			mScorePreview->SetLabelText(mDictionary.getWordScoreBreakdown(getSelectedString()));
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
