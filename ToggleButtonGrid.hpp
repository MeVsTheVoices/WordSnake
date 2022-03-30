#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/sizer.h>
#include <vector>
#include <map>
#include <functional>
#include <list>

class ToggleButtonGrid : public wxPanel {
private:
	std::vector<std::vector<wxToggleButton*> > mButtonsByIndex;
	std::map<int, std::pair<int, int> > mButtonsByID;
	std::list<std::function<bool(wxCommandEvent&, int, int, wxToggleButton*)> > mButtonEventListenerList;
	int mWidth, mHeight;
public:
	ToggleButtonGrid(wxWindow* parent,
		int initialWidth, int initialHeight,
		wxFont aFont,
		int vGap = 0, int hGap = 0);

	void OnButtonClicked(wxCommandEvent& evt);

	void registerButtonEventListener(std::function<bool(wxCommandEvent&, int, int, wxToggleButton*)>);
	void clearButtonEventListeners();

	wxToggleButton* getToggleButtonByIndex(int x, int y);

protected:

	void handleButtonEvent(wxCommandEvent& event);

};