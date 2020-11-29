#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <mmsystem.h>
#include "FreeRTOSConfig.h"
#include "../../core/ucrtos.h"
#include "../../../eMIDI/src/midifile.h"
#include "../../../../lib/LockFreeFifo/LockFreeFifo.h"

void statusLedOn() {
  if (GetKeyState(VK_SCROLL) == 1)
    return;

  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void statusLedOff() {
  if (GetKeyState(VK_SCROLL) == 0)
    return;

  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

void PrintCharUsr(char c) {
  putchar(c);
}

int eMidi_halTimeUs() {
  return upTimeMs() * 1000;
}

// Display:

void mainCreateWxLcdSimulator(uint8_t* pFrameBuffer, int xMax, int yMax);
void mainLcdDraw();

void hardwareDisplayInit(uint8_t* pFrameBuffer, int xMax, int yMax) {
  mainCreateWxLcdSimulator(pFrameBuffer, xMax, yMax);
}

void hardwareDisplayDraw() {
  mainLcdDraw();
}

// Colored print:

static void _printColored(const char* pFormat, va_list args, uint16_t colorAttributes) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
  WORD saved_attributes;

  GetConsoleScreenBufferInfo(hConsole, &consoleInfo); // Save current font color
  saved_attributes = consoleInfo.wAttributes;
  SetConsoleTextAttribute(hConsole, colorAttributes); // Change font color
  vprintf(pFormat, args);
  SetConsoleTextAttribute(hConsole, saved_attributes); // Restore original font color
}

void hal_printf(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  _printColored(pFormat, args, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_printfError(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  _printColored(pFormat, args, FOREGROUND_RED | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_printfWarning(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  _printColored(pFormat, args, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow
  va_end(args);
}

void hal_printfSuccess(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  _printColored(pFormat, args, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_printfInfo(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  _printColored(pFormat, args, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_strcpy_s(char* pDst, int maxSize, const char* pSrc) {
  for (int i = 0; i < maxSize - 1; i++) {
    pDst[i] = pSrc[i];

    if (i == maxSize - 1)
      pDst[i] = 0;
  }
}

// MIDI device:

static HMIDIOUT _hMidiOut;

void hardwareInitMidiDevice() {
  uint32_t result = midiOutOpen(&_hMidiOut, MIDI_MAPPER, 0, 0, 0);

  if (result != MMSYSERR_NOERROR)
    hal_printfError("MIDI device does not work!");
}

void hardwareFreeMidiDevice() {
  uint32_t result = midiOutClose(_hMidiOut);

  if (result != MMSYSERR_NOERROR)
    hal_printfError("Error on freeing MIDI device!");
}

void hardwareSendMidiMsg(const MidiEvent* pEvent) {
  if (pEvent->eventId == MIDI_EVENT_META)
    return;

  union {
    uint32_t word;
    uint8_t data[3];
  } message = { 0 };

  message.data[0] = pEvent->eventId;
  message.data[1] = pEvent->params.pRaw[0];
  message.data[2] = pEvent->params.pRaw[1];

  MMRESULT flag = midiOutShortMsg(_hMidiOut, message.word);
  if (flag != MMSYSERR_NOERROR) {
    hal_printfWarning("Warning: MIDI Output is not open.\n");
  }
}

void hardwareInputDeviceInit() {

}

// Input device:

#define KEY_ACTION  VK_RETURN
#define KEY_BACK    VK_BACK
#define KEY_NORTH   'W'
#define KEY_WEST    'A'
#define KEY_SOUTH   'S'
#define KEY_EAST    'D'
#define KEY_START   VK_SPACE
#define KEY_SELECT 'X'
#define KEY_SIMULATE_DISCONNECTED VK_TAB

uint8_t getButtonStateEmu(int virtualKeyCode) {
  return (GetAsyncKeyState(virtualKeyCode) & 32768) >> 15;

  // if (GetForegroundWindow() == g_hEmuWnd)
  //   return (GetAsyncKeyState(virtualKeyCode) & 32768) >> 15;
  // else
  //   return 0;
}

InputDeviceStates_t getInputDeviceState() {
  InputDeviceStates_t states;

  states.Connected = !getButtonStateEmu(KEY_SIMULATE_DISCONNECTED);
  states.Action    = getButtonStateEmu(KEY_ACTION);
  states.Back      = getButtonStateEmu(KEY_BACK);
  states.North     = getButtonStateEmu(KEY_NORTH) || getButtonStateEmu(VK_UP);
  states.West      = getButtonStateEmu(KEY_WEST)  || getButtonStateEmu(VK_LEFT);
  states.South     = getButtonStateEmu(KEY_SOUTH) || getButtonStateEmu(VK_DOWN);
  states.East      = getButtonStateEmu(KEY_EAST)  || getButtonStateEmu(VK_RIGHT);
  states.Start     = getButtonStateEmu(KEY_START);
  states.Select    = getButtonStateEmu(KEY_SELECT);

  return states;
}

// RS485:
void hal_rs485Send(char dataByte) {
  // Not used on win32 yet:
  hal_printf("hal_rs485Send\n");
}

void hal_rs485init(LockFreeFIFO_t* pFifo) {

}

// Filesystem:

static HANDLE fo_findHandle;

void hal_fileSystemInit() {
  // Not used on windows simulator
}

bool hal_findInit(const char* pPath, FO_FIND_DATA* findData) {
  char pSearchPath[256];
  WIN32_FIND_DATAA search_data;
  memset(&search_data, 0, sizeof(WIN32_FIND_DATAA));
  strcpy(pSearchPath, pPath);
  strcat(pSearchPath, "/*");

  fo_findHandle = FindFirstFileA(pSearchPath, &search_data);
  strcpy_s(findData->fileName, sizeof(findData->fileName), search_data.cFileName);
  return fo_findHandle != INVALID_HANDLE_VALUE ? TRUE : FALSE;
}

bool hal_findNext(FO_FIND_DATA* findData) {
  WIN32_FIND_DATAA search_data;
  memset(&search_data, 0, sizeof(WIN32_FIND_DATAA));
  BOOL fileHasFound = FindNextFileA(fo_findHandle, &search_data);
  strcpy_s(findData->fileName, sizeof(findData->fileName), search_data.cFileName);
  return fileHasFound;
}

void hal_findFree() {
  FindClose(fo_findHandle);
}

int32_t hal_fopen(FILE** pFile, const char* pFileName) {
  return fopen_s(pFile, pFileName, "rb") == 0;
}

int32_t hal_fclose(FILE* pFile) {
  return fclose(pFile);
}

int32_t hal_fseek(FILE* pFile, int startPos) {
  return fseek(pFile, startPos, SEEK_SET);
}

size_t hal_fread(FILE* pFile, void* pDst, size_t numBytes) {
  return fread_s(pDst, numBytes, 1, numBytes, pFile); // TODO: word access? Does it increase performance?
}

int32_t hal_ftell(FILE* pFile) {
  return ftell(pFile);
}
