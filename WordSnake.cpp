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

    wxLog::EnableLogging();

#ifdef _DEBUG
    wxLog::SetLogLevel(wxLOG_Debug);
    wxLog::SetActiveTarget(new wxLogWindow(nullptr, "Logging"));
#else
    wxLog::SetLogLevel(wxLOG_Error);
#endif // DEBUG

    mGameGrid = new GameGrid();
    mGameGrid->Show();

    return true;
}