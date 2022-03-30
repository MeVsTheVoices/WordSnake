#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/tglbtn.h>
#include <wx/statline.h>

#include <random>
#include <list>

#include "Dictionary.hpp"
#include "ToggleButtonGrid.hpp"

class GameGrid : public wxFrame {
public:
	GameGrid();
	~GameGrid();

	const static int GRID_WIDTH = 10;
	const static int GRID_HEIGHT = 10;

	wxChar* mValues = nullptr;

	wxToggleButton** mButtons;

	void OnButtonClicked(wxCommandEvent& evt);
	void OnOkButtonClicked(wxCommandEvent& evt);
	void OnCancelButtonClicked(wxCommandEvent& evt);

	bool handleToggleButtonEvent(wxCommandEvent& evt, int x, int y, wxToggleButton* button);

private:
	static const int mAlphabetCeil = 26;
	const wxChar mAlphabet[mAlphabetCeil] {
		'A', 'B', 'C', 'D', 'E',
		'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y',
		'Z'
	};
	const int mAlphabetDistribution[mAlphabetCeil]{
		820, 150, 270, 470, 1300,
		220, 20,  620, 690, 14,
		78,  410, 270, 670, 780,
		190, 11,  590, 620, 960,
		270, 97,  240, 15,  200,
		8
	};
	int mAlphabetScores[mAlphabetCeil];

	int mCurrentScore = 0;

	std::default_random_engine* mRandomEngine;
	std::discrete_distribution<int>* mRandomDistribution;
	wxChar randomLetter() const;

	std::list<std::pair<int, int> > mCurrentSelection;
	static bool isAdjacent(int, int, int, int);
	static std::pair<int, int> getCoordinates();
	void replaceCurrentSelection();
	wxString getSelectedString() const;
	void clearCurrentSelection();

	int getWordScore(wxString);
	Dictionary mDictionary;

	wxStaticText* mScorePreview;

	ToggleButtonGrid* mToggleButtonGrid;
};