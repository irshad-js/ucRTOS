#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <string>
#include <memory>

using namespace std;

extern "C" {
  #include "FreeRTOS.h"
  #include "task.h"
  #include <stdio.h>
}

StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

extern "C" {
  int coreMain(void);

  void vApplicationMallocFailedHook(void) {
    vAssertCalled(__LINE__, __FILE__);
  }

  void vApplicationIdleHook(void) {

  }

  void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    vAssertCalled(__LINE__, __FILE__);
  }

  void vApplicationTickHook(void) {

  }

  void vApplicationDaemonTaskStartupHook(void) {

  }

  void vAssertCalled(unsigned long ulLine, const char * const pcFileName) {
    static portBASE_TYPE xPrinted = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    (void)ulLine;
    (void)pcFileName;

    printf("ASSERT! Line %d, file %s, GetLastError() %d\r\n", ulLine, pcFileName, GetLastError());

    while (ulSetToNonZeroInDebuggerToContinue == 0) {
      __asm { NOP };
      __asm { NOP };
    }
  }

  void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  }

  void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize) {
    static StaticTask_t xTimerTaskTCB;

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  }
}

// WX

// CANVAS

class BasicDrawPane : public wxPanel {

public:
  BasicDrawPane(wxFrame* parent) : wxPanel(parent),
      pBackBuffer_(new wxBitmap(320, 240)) {

    clearBackBuffer();

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
  }

  void init(uint16_t* pFrameBuffer) {
    pFrameBuffer_ = pFrameBuffer;
  }

  void clearBackBuffer() {
    for (int x = 0; x < 320; ++x)
      for (int y = 0; y < 240; ++y)
        pixelOld_[x][y] = MyColorOld(0, 0, 0);
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

    for (int y = 0; y < 240; ++y) {
      for (int x = 0; x < 320; ++x) {
        // TODO: remove:
        const MyColorOld& pixelOld = pixelOld_[x][y];
        uint8_t redOld   = (pixelOld.red   * 255) / 31;
        uint8_t greenOld = (pixelOld.green * 255) / 63;
        uint8_t blueOld  = (pixelOld.blue  * 255) / 31;
        pen.SetColour(redOld, greenOld, blueOld);
        const int MAX_X = 320; // TODO: pass buffer dimensions to init function
        const int MAX_Y = 240; // TODO: pass buffer dimensions to init function
        // -

        int index = y * MAX_X + x;
        struct Bla {
          uint16_t
          red   : 5,
          green : 6,
          blue  : 5;
        };

        Bla* c = &reinterpret_cast<Bla*>(pFrameBuffer_)[index];

        uint8_t red   = (c->red   * 255) / 31;
        uint8_t green = (c->green * 255) / 63;
        uint8_t blue  = (c->blue  * 255) / 31;
        pen.SetColour(red, green, blue);

        backBufferDc.SetPen(pen);
        backBufferDc.DrawPoint(x, y);
      }
    }

    backBufferDc.SelectObject(wxNullBitmap);

    clientDc.DrawBitmap(*pBackBuffer_, 0, 0);
  }

  void setPixel(int x, int y, int r, int g, int b) {
    pixelOld_[x][y] = MyColorOld(r, g, b);
  }

private:
  union MyColorOld {
    MyColorOld() {}
    MyColorOld(uint8_t red, uint8_t green, uint8_t blue) {
      this->red   = red   * 31 / 255;
      this->green = green * 63 / 255;
      this->blue  = blue  * 31 / 255;
    }

    uint16_t
    red   : 5,
    green : 6,
    blue  : 5;
  };

  uint16_t* pFrameBuffer_{nullptr};

  // TODO: remove:
  MyColorOld pixelOld_[320][240];
  // -

  std::auto_ptr<wxBitmap> pBackBuffer_{nullptr};

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

extern "C" void mainCreateWxLcdSimulator(uint16_t* pFrameBuffer) {
  SetEvent(_hCreateLcdSimulator);
  WaitForSingleObject(_hCreateLcdSimulator, INFINITE);

  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();
  pDrawPane->init(pFrameBuffer);
}

extern "C" void mainLcdSetPixel(int x, int y, int red, int green, int blue) {
  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();

  pDrawPane->setPixel(x, y, red, green, blue);
}

extern "C" void mainLcdDraw() {
  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();

  pDrawPane->paintNow();
}

extern "C" void mainLcdClear() {
  BasicDrawPane* pDrawPane = WxApp::instance()->drawPane();

  pDrawPane->clearBackBuffer();
}
