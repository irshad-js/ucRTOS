#include <wx/wx.h>
#include <string>

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
  BasicDrawPane(wxFrame* parent) : wxPanel(parent) {
    for (int x = 0; x < 320; ++x)
      for (int y = 0; y < 240; ++y)
        pixel_[x][y] = MyColor(0, 0, 0);
  }

  void paintEvent(wxPaintEvent & evt) {
    wxPaintDC dc(this);
    render(dc);
  }

  void paintNow() {
    wxClientDC dc(this); // Windows only?
    render(dc);
  }

  void render(wxDC& dc) {
    for (int x = 0; x < 320; ++x) {
      for (int y = 0; y < 240; ++y) {
        const MyColor& pixel = pixel_[x][y];

        dc.SetPen(wxPen(wxColor(pixel.red, pixel.green, pixel.blue), 1));
        dc.DrawPoint(x, y);
      }
    }
  }

  void setPixel(int x, int y, int r, int g, int b) {
    pixel_[x][y] = MyColor(r, g, b);
    paintNow();
  }

private:
  struct MyColor {
    MyColor() {}
    MyColor(uint8_t red, uint8_t green, uint8_t blue) {
      this->red   = red;
      this->green = green;
      this->blue  = blue;
    }

    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  MyColor pixel_[320][240];

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

private:
  MainFrame* pMainFrame_{nullptr};
  BasicDrawPane* pDrawPane_{nullptr};
};

IMPLEMENT_APP_NO_MAIN(WxApp);
IMPLEMENT_WX_THEME_SUPPORT;

bool WxApp::OnInit() {
  pMainFrame_ = new MainFrame("ucRTOS LCD Simulator", wxPoint(50, 50), wxSize(640, 480));
  pDrawPane_ = new BasicDrawPane(pMainFrame_);
  pMainFrame_->Show(true);

  pDrawPane_->setPixel(100, 100, 0, 255, 0);
  pDrawPane_->setPixel(100, 101, 0, 255, 0);
  pDrawPane_->setPixel(100, 102, 0, 255, 0);
  pDrawPane_->setPixel(100, 103, 0, 255, 0);
  pDrawPane_->setPixel(100, 104, 0, 255, 0);

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
  SetStatusText("LCD simulation is not implemented yet!");
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

HANDLE _hCreateLcdSimulator = CreateEvent(NULL, FALSE, FALSE, NULL);

int main(int argc, char *argv[]) {
  if (HANDLE hThread = CreateThread(NULL, 0, _rtosThread, NULL, 0, NULL)) {
    WaitForSingleObject(_hCreateLcdSimulator, INFINITE);

    wxEntryStart(argc, argv);
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();
  }

  return 0;
}

extern "C" void mainCreateWxLcdSimulator() {
  SetEvent(_hCreateLcdSimulator);
}
