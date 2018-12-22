#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <stdio.h>
#include <time.h>

extern "C" {
  #include "../../../../core/lut.h"
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
        int idx = y * xMax_ + x;
        int subIdx = idx / 2;
        int useMsb = idx % 2 == 0;
        uint8_t ci = 0;

        if (!useMsb)
          ci = pFrameBuffer_[subIdx] & 0x0F;
        else
          ci = (pFrameBuffer_[subIdx] & 0xF0) >> 4;

        pen.SetColour(pMasterPalette[ci].red, pMasterPalette[ci].green, pMasterPalette[ci].blue);

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
  // EnableMaximizeButton(false); // TODO: set in wxFrame constructor
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

static void* _rtosThread(void* pData) {
  coreMain();
}

//-----------------------------------------------------------------------------
// FreeRTOS Application Hooks
//-----------------------------------------------------------------------------

extern "C" void vApplicationIdleHook(void) {
  // The co-routines are executed in the idle task using the idle task hook.
  // vCoRoutineSchedule(); // Comment this out if not using Co-routines.

  // Makes the process more agreeable when using the Posix simulator:
  struct timespec xTimeToSleep, xTimeSlept;
  xTimeToSleep.tv_sec = 1;
  xTimeToSleep.tv_nsec = 0;
  nanosleep(&xTimeToSleep, &xTimeSlept);
}

extern "C" void vMainQueueSendPassed(void) {

}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

// static HANDLE _hCreateLcdSimulator = CreateEvent(NULL, FALSE, FALSE, NULL);

static void mySleepMs(int ms) {
  printf("Now sleeping...");

  clock_t c = clock();
  while(clock() - c < 1000 * ms)
    ;

  printf(" done\n");
}

int main(int argc, char* pArgv[]) {
  printf("ucRTOS linux simulator\n");
  wxEntryStart(argc, pArgv);

  pthread_t rtosThread;
  if (int error = pthread_create(&rtosThread, NULL, _rtosThread, (void*)NULL))
    return 1;

  wxTheApp->CallOnInit();
  // WaitForSingleObject(_hCreateLcdSimulator, INFINITE);
  mySleepMs(2000);

  // SetEvent(_hCreateLcdSimulator);
  wxTheApp->OnRun();

  pthread_join(rtosThread, NULL);
  return 0;
}

extern "C" void mainCreateWxLcdSimulator(uint8_t* pFrameBuffer, int xMax, int yMax) {
  // SetEvent(_hCreateLcdSimulator);
  // WaitForSingleObject(_hCreateLcdSimulator, INFINITE);

  mySleepMs(1000); // TODO: remove

  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();
  pDrawPane->init(pFrameBuffer, xMax, yMax);
}

extern "C" void mainLcdDraw() {
  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();

  pDrawPane->paintNow();
}

