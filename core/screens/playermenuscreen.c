  #include <stddef.h>
#include "../StackBasedFsm.h"
#include "../SlotBasedMenu.h"
#include "../../lib/colorprint/colorprint.h"
#include "../display.h"
#include "../../core/ucrtos.h"

#include "playermenuscreen.h"
#include "playerscreen.h"

#define MENU_MAX_SLOTS 12
#define MENU_FILES_PER_PAGE 10

#define MIDI_PATH ""

#ifdef _WIN32
  #undef MIDI_PATH
  #define DONT_DEFINE_COMMON_MAIN
  #define MIDI_PATH "_sdcard"
#endif

#ifdef __linux__
  #undef MIDI_PATH
  #define MIDI_PATH "./_sdcard"
#endif

static struct {
  SlotBasedMenu_t menu;
  char filePath[256];
  FO_FIND_DATA findData;
  int curPage;
} context;

static void onBrowseNewPage(int currentPage, int totalPages) {
  hal_printf("playlist::onBrowseNewPage()\n");

  // Set tracks of current page
  bool endOfDirectory = !hal_findInit(context.filePath, &context.findData);
  int curFileIndex = 0;

  context.menu.numSlots = 0; // Clear all slots.

  while (!endOfDirectory) {
    if (context.findData.fileName[0] != '.') {
      if (curFileIndex >= (currentPage - 1) * MENU_FILES_PER_PAGE)
        menuAddSlot(&context.menu, context.findData.fileName, playerScreen);

      curFileIndex++;
    }

    endOfDirectory = !hal_findNext(&context.findData) || curFileIndex >= (currentPage - 1) * MENU_FILES_PER_PAGE + MENU_FILES_PER_PAGE;
  }
  hal_findFree();

  // Update page number
  char pageText[32];
  sprintf(pageText, "%2d / %2d", currentPage, totalPages); // TODO: hal_sprintf?

  displayDrawText(255, 220, pageText, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
}

static void getFilePathFromCursorPos(char* dstFilePath, int cursorPos) {
  static FO_FIND_DATA findData;

  strcpy(dstFilePath, MIDI_PATH);
  bool endOfDirectory = !hal_findInit(dstFilePath, &findData);
  int itemCount = 0;

  while (!endOfDirectory) {
    if (findData.fileName[0] != '.' && findData.fileName[1] != '.')
      if (itemCount++ == cursorPos) {
        strcat(dstFilePath, "/");
        strcat(dstFilePath, findData.fileName);
        break;
      }

    if (!hal_findNext(&findData))
      endOfDirectory = true;
  }

  hal_findFree();
}

static uint32_t getNumFiles(char* filePath) {
  static FO_FIND_DATA findData;
  bool endOfDirectory = !hal_findInit(filePath, &findData);
  int numFiles = 0;

  while (!endOfDirectory) {
    if (findData.fileName[0] != '.')
      if (findData.fileName[1] != '.')
        numFiles++;

    if (!hal_findNext(&findData))
      endOfDirectory = true;
  }
  hal_findFree();

  return numFiles;
}

static uint32_t getNumPages(uint32_t numFiles, uint32_t filesPerPage) {
  bool isPageCountEven = numFiles % filesPerPage == 0;
  int pageCount = isPageCountEven ? numFiles / filesPerPage : numFiles / filesPerPage + 1;

  return pageCount;
}

static void draw() {
  displayClear();
  displayDrawText(CENTER, 0, "Use the game pad to select a song", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 18, "Press A button to start", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  int numFilesTotal = getNumFiles(context.filePath);
  int numPages = getNumPages(numFilesTotal, MENU_FILES_PER_PAGE);

  if (numFilesTotal > 0) {
    onBrowseNewPage(context.curPage, numPages); // FIXME: implement correctly!
    menuDraw(&context.menu);
  }
  else {
    displayDrawText(CENTER, context.menu.yPos - 5 + 3 * 18, "No tracks available!", 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00);
    displayDrawText(CENTER, context.menu.yPos - 5 + 4 * 18, "SD-Card missing?", 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00);
  }

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("playerMenuScreen::onEnter\n");

  hal_fileSystemInit();

  context.curPage = 1;

  userMenuInit(&context.menu, pFsm, 3, 50);
  strcpy(context.filePath, MIDI_PATH);

  draw();
}

static void onAction(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onActionPress\n");

  if (pressed) {
    char path[256];
    getFilePathFromCursorPos(path, (context.curPage - 1) * MENU_FILES_PER_PAGE + context.menu.cursorPos);

    PlayerScreenParams params;
    params.pFileName = path;

    userMenuTransitToSelectedSlot(&context.menu, &params);
  }
}

static void onBack(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onBackPress\n");

  if (pressed)
    leaveState(pFsm);
}

static void onStart(StackBasedFsm_t* pFsm, bool pressed) {
  hal_printf("playerMenuScreen::onStartPress\n");

  // TODO: play MIDI
}

static void onDirection(StackBasedFsm_t* pFsm, bool south, bool north, bool west, bool east) {
  hal_printf("playerMenuScreen::onDirectionPress\n");

  if (south) {
    if (context.menu.cursorPos == MENU_FILES_PER_PAGE - 1) {
      context.curPage++;
      context.menu.cursorPos = 0;
    }
    else
      menuMoveCursorDown(&context.menu);
  }

  if (north) {
    if (context.menu.cursorPos == 0 && context.curPage != 1) {
      context.curPage--;
      context.menu.cursorPos = MENU_FILES_PER_PAGE - 1;
    }
    else
      menuMoveCursorUp(&context.menu);
  }

  draw();
}

static void onReenter(StackBasedFsm_t* pFsm) {
  hal_printf("playerMenuScreen::onReenter\n");

  draw();
}

// Always implement this as last function of your state file:

void playerMenuScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState   = onEnter;
  pState->onAction       = onAction;
  pState->onBack         = onBack;
  pState->onStart        = onStart;
  pState->onDirection    = onDirection;
  pState->onReenterState = onReenter;
}

