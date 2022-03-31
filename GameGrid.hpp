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

	int mCurrentScore = 0;

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