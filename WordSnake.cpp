#include "GameGrid.hpp"
class WordSnake : public wxApp
{
private:
    GameGrid* mGameGrid = nullptr;
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(WordSnake);
bool WordSnake::OnInit() {
    wxLog::SetLogLevel(wxLOG_Debug);
    wxLog::EnableLogging();
    wxLog::SetActiveTarget(new wxLogWindow(nullptr, "Logging"));

    mGameGrid = new GameGrid();
    mGameGrid->Show();

    return true;
}