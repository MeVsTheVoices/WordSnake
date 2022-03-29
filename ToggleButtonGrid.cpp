#include "ToggleButtonGrid.hpp"

ToggleButtonGrid::ToggleButtonGrid(
		wxWindow* parent,
		int initialWidth, int initialHeight,
		wxFont aFont,
		int vGap, int hGap) :
		wxPanel(parent),
		mWidth(initialWidth), mHeight(initialHeight) {

	//mButtons = new wxToggleButton * [mHeight * mWidth];

	mButtonsByIndex = std::vector<std::vector<wxToggleButton*> >(mHeight, std::vector<wxToggleButton*>(mWidth));

	wxGridSizer* grid = new wxGridSizer(mHeight, mWidth, vGap, hGap);

	//populate the grid with buttons, generate random letters for each, and bind their even methods
	for (int i = 0; i < mHeight; i++) {
		for (int j = 0; j < mWidth; j++) {
			mButtonsByIndex[i][j] = new wxToggleButton(
				this,
				wxID_ANY,
				std::to_string(i) + ' ' + std::to_string(j));

			mButtonsByID[mButtonsByIndex[i][j]->GetId()] = std::pair<int, int>(i, j);

			mButtonsByIndex[i][j]->SetFont(aFont);

			grid->Add(mButtonsByIndex[i][j], 1, wxEXPAND | wxALL);

			mButtonsByIndex[i][j]->Bind(wxEVT_TOGGLEBUTTON, &ToggleButtonGrid::OnButtonClicked, this);
		}
	}

	this->SetSizerAndFit(grid);
}

void ToggleButtonGrid::OnButtonClicked(wxCommandEvent& evt) {

}

void ToggleButtonGrid::registerButtonEventListener(std::function<bool(wxCommandEvent&, int, int, wxToggleButton*)> listener) {
	mButtonEventListenerList.push_back(listener);
}

void ToggleButtonGrid::removeButtonEventListener(std::function<bool(wxCommandEvent&, int, int, wxToggleButton*)> listener) {
	mButtonEventListenerList.remove(listener);
}

void ToggleButtonGrid::clearButtonEventListeners() {
	mButtonEventListenerList.clear();
}

void ToggleButtonGrid::handleButtonEvent(wxCommandEvent& event) {
	auto coords = mButtonsByID[event.GetId()];
	for (auto i : mButtonEventListenerList) {
		if (i(event, coords.first, coords.second, mButtonsByIndex[coords.first][coords.second]))
			return;
		else
			continue;
	}
}
