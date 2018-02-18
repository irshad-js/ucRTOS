#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <string>
#include <memory>

using namespace std;

extern "C" {
  #include "../../../../core/display.h"
  int coreMain(void);
}

// WX

// CANVAS

class BasicDrawPane : public wxPanel {

public:
  BasicDrawPane(wxFrame* parent) : wxPanel(parent) {
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
  }

  ~BasicDrawPane() {
    delete pBackBuffer_;
  }

  void init(uint8_t* pFrameBuffer, int xMax, int yMax) {
    pFrameBuffer_ = pFrameBuffer;
    xMax_ = xMax;
    yMax_ = yMax;
    pBackBuffer_ = new wxBitmap(xMax_, yMax_);
  }

  void paintEvent(wxPaintEvent & evt) {
    // Always create the wxPaintDC object here, even if you don't use it!
    // Creating this object will tell wxWidgets that the invalid regions in the window have been repainted so
    // that the windowing system won't keep sending paint events ad infinitum:

    wxPaintDC dc(this);
    render(dc);
  }

  void paintNow() {
    wxClientDC dc(this); // Windows only?
    render(dc);
  }

  void render(wxDC& clientDc) {
    wxMemoryDC backBufferDc;
    wxPen pen;

    backBufferDc.SelectObject(*pBackBuffer_);

    for (int y = 0; y < yMax_; ++y) {
      for (int x = 0; x < xMax_; ++x) {

        int index = y * xMax_ + x;
        int ci = pFrameBuffer_[index];

        pen.SetColour(pPalette[ci].red, pPalette[ci].green, pPalette[ci].blue);

        backBufferDc.SetPen(pen);
        backBufferDc.DrawPoint(x, y);
      }
    }

    backBufferDc.SelectObject(wxNullBitmap);
    clientDc.DrawBitmap(*pBackBuffer_, 0, 0);
  }

private:
  int xMax_{0};
  int yMax_{0};

  uint8_t* pFrameBuffer_{ nullptr };
  wxBitmap* pBackBuffer_{ nullptr };

  DECLARE_EVENT_TABLE()
};

// END OF CANVAS

class MainFrame : public wxFrame {
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
  void OnAbout(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

enum {
  ID_PasteBlueprint = 1,
  ID_GetBlueprint   = 2
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

class WxApp : public wxApp {
public:
  virtual bool OnInit();
  static WxApp* instance() { return static_cast<WxApp*>(wxApp::GetInstance()); }
  BasicDrawPane* drawPane() { return pDrawPane_; }

private:
  MainFrame* pMainFrame_{nullptr};
  BasicDrawPane* pDrawPane_{nullptr};
};

IMPLEMENT_APP_NO_MAIN(WxApp);
IMPLEMENT_WX_THEME_SUPPORT;

bool WxApp::OnInit() {
  pMainFrame_ = new MainFrame("ucRTOS LCD Simulator", wxPoint(50, 50), wxSize(350, 340));
  pDrawPane_ = new BasicDrawPane(pMainFrame_);
  pMainFrame_->Show(true);

  printf("ucRTOS LCD Simulator started.\n");
  return true;
}

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_PAINT(BasicDrawPane::paintEvent) // catch paint events
END_EVENT_TABLE()

//--------------------------------------------------------------------------------------------------------------
// MainFrame
//--------------------------------------------------------------------------------------------------------------

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame(NULL, wxID_ANY, title, pos, size) {

  EnableCloseButton(false);
  EnableMaximizeButton(false);
  SetMinSize(GetSize());
  SetMaxSize(GetSize());

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);
  CreateStatusBar();
  SetStatusText("Ready.");
}

void MainFrame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("Made by coon\n\n"
               "E-Mail: coon@mailbox.org\n"
               "IRC: coon@freenode.org\n"
               "Twitter: @coon_42\n"
               "\n"
               "https://www.reddit.com/user/coon42\nhttps://github.com/coon42\n",
               "About ucRTOS", wxOK | wxICON_INFORMATION);
}

// end of WX

static DWORD WINAPI _rtosThread(void* data) {
  return coreMain();
}

static HANDLE _hCreateLcdSimulator = CreateEvent(NULL, FALSE, FALSE, NULL);

int main(int argc, char *argv[]) {
  wxEntryStart(argc, argv);

  HANDLE hThread = CreateThread(NULL, 0, _rtosThread, NULL, 0, NULL);

  if(!hThread)
    return 1;

  WaitForSingleObject(_hCreateLcdSimulator, INFINITE);
  wxTheApp->CallOnInit();
  SetEvent(_hCreateLcdSimulator);
  wxTheApp->OnRun();

  return 0;
}

extern "C" void mainCreateWxLcdSimulator(uint8_t* pFrameBuffer, int xMax, int yMax) {
  SetEvent(_hCreateLcdSimulator);
  WaitForSingleObject(_hCreateLcdSimulator, INFINITE);

  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();
  pDrawPane->init(pFrameBuffer, xMax, yMax);
}

extern "C" void mainLcdDraw() {
  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();

  pDrawPane->paintNow();
}
