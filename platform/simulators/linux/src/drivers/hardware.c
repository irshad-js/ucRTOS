#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <dirent.h>
#include "FreeRTOSConfig.h"
#include "../../core/ucrtos.h"
#include "../../../eMIDI/src/midifile.h"
#include "../../../../lib/LockFreeFifo/LockFreeFifo.h"

void statusLedOn() {
  // TODO: implement
}

void statusLedOff() {
  // TODO: implement
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

// --

enum GeneralFormatting {
  GEN_FORMAT_RESET       = 0,
  GEN_FORMAT_BRIGHT      = 1,
  GEN_FORMAT_DIM         = 2,
  GEN_FORMAT_ITALIC      = 3,
  GEN_FORMAT_UNDERSCORE  = 4,
  GEN_FORMAT_SLOW_BLINK  = 5,
  GEN_FORMAT_RAPID_BLINK = 6,
  GEN_FORMAT_REVERSE     = 7,
  GEN_FORMAT_HIDDEN      = 8,
  GEN_FORMAT_CROSSED_OUT = 9
};

enum ForegroundColors {
  FOREGROUND_COL_BLACK         = 30,
  FOREGROUND_COL_RED           = 31,
  FOREGROUND_COL_GREEN         = 32,
  FOREGROUND_COL_YELLOW        = 33,
  FOREGROUND_COL_BLUE          = 34,
  FOREGROUND_COL_MAGENTA       = 35,
  FOREGROUND_COL_CYAN          = 36,
  FOREGROUND_COL_LIGHT_GRAY    = 37,
  FOREGROUND_COL_DEFAULT       = 39,
  FOREGROUND_COL_DARK_GRAY     = 90,
  FOREGROUND_COL_LIGHT_RED     = 91,
  FOREGROUND_COL_LIGHT_GREEN   = 92,
  FOREGROUND_COL_LIGHT_YELLOW  = 93,
  FOREGROUND_COL_LIGHT_BLUE    = 94,
  FOREGROUND_COL_LIGHT_MAGENTA = 95,
  FOREGROUND_COL_LIGHT_CYAN    = 96,
  FOREGROUND_COL_WHITE         = 97
};

enum BackgroundColors {
  BACKGROUND_COL_BLACK         = 40,
  BACKGROUND_COL_RED           = 41,
  BACKGROUND_COL_GREEN         = 42,
  BACKGROUND_COL_YELLOW        = 43,
  BACKGROUND_COL_BLUE          = 44,
  BACKGROUND_COL_MAGENTA       = 45,
  BACKGROUND_COL_CYAN          = 46,
  BACKGROUND_COL_LIGHT_GRAY    = 47,
  BACKGROUND_COL_DEFAULT       = 49,
  BACKGROUND_COL_DARK_GRAY     = 100,
  BACKGROUND_COL_LIGHT_REDY    = 101,
  BACKGROUND_COL_LIGHT_GREEN   = 102,
  BACKGROUND_COL_LIGHT_YELLOW  = 103,
  BACKGROUND_COL_LIGHT_BLUE    = 104,
  BACKGROUND_COL_LIGHT_MAGENTA = 105,
  BACKGROUND_COL_LIGHT_CYAN    = 106,
  BACKGROUND_COL_WHITE         = 107
};

static void _printColored(const char* pText, int32_t background, int32_t foreground, int32_t effect) {
  char pBuf[512];

  const char* p = pText;
  const char* pStart = p;

  while (*p++) {
    if (*p == '\n') {
      int len = p - pStart;
      strncpy(pBuf, pStart, len);
      pBuf[len] = '\0';
      printf("\x1b[%d;%d;%dm%s", effect, foreground, background, pBuf);
      printf("\x1b[0m\n");

      pStart = p + 1;
    }
  }

  printf("\x1b[%d;%d;%dm%s", effect, foreground, background, pStart);
}

void hal_printf(char* pFormat, ...) {
  char pFormattedText[512];

  va_list args;
  va_start(args, pFormat);
  vsnprintf(pFormattedText, sizeof(pFormattedText), pFormat, args);
  _printColored(pFormattedText, BACKGROUND_COL_DEFAULT, FOREGROUND_COL_DEFAULT, GEN_FORMAT_RESET);
  va_end(args);
}

void hal_printfError(const char* pFormat, ...) {
  char pFormattedText[512];

  va_list args;
  va_start(args, pFormat);
  vsnprintf(pFormattedText, sizeof(pFormattedText), pFormat, args);
  _printColored(pFormattedText, BACKGROUND_COL_DEFAULT, FOREGROUND_COL_RED, GEN_FORMAT_BRIGHT);
  va_end(args);
}

void hal_printfWarning(char* pFormat, ...) {
  char pFormattedText[512];

  va_list args;
  va_start(args, pFormat);
  vsnprintf(pFormattedText, sizeof(pFormattedText), pFormat, args);
  _printColored(pFormattedText, BACKGROUND_COL_RED, FOREGROUND_COL_YELLOW, GEN_FORMAT_BRIGHT);
  va_end(args);
}

void hal_printfSuccess(char* pFormat, ...) {
  char pFormattedText[512];

  va_list args;
  va_start(args, pFormat);
  vsnprintf(pFormattedText, sizeof(pFormattedText), pFormat, args);
  _printColored(pFormattedText, BACKGROUND_COL_DEFAULT, FOREGROUND_COL_GREEN, GEN_FORMAT_BRIGHT);
  va_end(args);
}

void hal_printfInfo(char* pFormat, ...) {
  char pFormattedText[512];

  va_list args;
  va_start(args, pFormat);
  vsnprintf(pFormattedText, sizeof(pFormattedText), pFormat, args);
  _printColored(pFormattedText, BACKGROUND_COL_DEFAULT, FOREGROUND_COL_YELLOW, GEN_FORMAT_BRIGHT);
  va_end(args);
}

// --

void hal_strcpy_s(char* dst, int maxSize, const char* src) {
  for (int i = 0; i < maxSize - 1; i++) {
    dst[i] = src[i];

    if (i == maxSize - 1)
      dst[i] = 0;
  }
}

// MIDI device:

struct {
  int32_t fd;
  uint8_t devnum;
} _midiDevCtx;

void hardwareInitMidiDevice() {
  const char* pDevice = "/dev/sequencer";

  _midiDevCtx.fd = open(pDevice, O_WRONLY, 0);
  _midiDevCtx.devnum = 1;

  if(_midiDevCtx.fd < 0) {
    hal_printf("Error: cannot open %s\n", pDevice);
    hal_printfError("MIDI device does not work!");
    return;
  }
}

void hardwareFreeMidiDevice() {
  close(_midiDevCtx.fd);
}

void hardwareSendMidiMsg(const MidiEvent* pEvent) {
  if (pEvent->eventId == MIDI_EVENT_META)
    return;

  int numParamBytes = 0;

  switch(pEvent->eventId & 0xF0) {
    case MIDI_EVENT_NOTE_OFF:          numParamBytes = 2; break;
    case MIDI_EVENT_NOTE_ON:           numParamBytes = 2; break;
//    case MIDI_EVENT_POLY_KEY_PRESSURE: numParamBytes = 2; break;
//    case MIDI_EVENT_CONTROL_CHANGE:    numParamBytes = 2; break;
    case MIDI_EVENT_PROGRAM_CHANGE:    numParamBytes = 1; break;
//    case MIDI_EVENT_CHANNEL_PRESSURE:  numParamBytes = 1; break;
    case MIDI_EVENT_PITCH_BEND:        numParamBytes = 2; break;

    default:
      return;
  }

  uint8_t packet[4] = { SEQ_MIDIPUTC, 0, _midiDevCtx.devnum, 0 };

  packet[1] = pEvent->eventId;
  write(_midiDevCtx.fd, packet, sizeof(packet));

  for(int i = 0; i < numParamBytes; ++i) {
    packet[1] = pEvent->params.pRaw[i];
    write(_midiDevCtx.fd, packet, sizeof(packet));
  }
}

void hardwareInputDeviceInit() {

}

// Input device:

#define KEY_ACTION    13   // RETURN
#define KEY_BACK       8   // BACKSPACE
#define KEY_NORTH     'W'
#define KEY_WEST      'A'
#define KEY_SOUTH     'S'
#define KEY_EAST      'D'
#define KEY_NORTH_ALT 59   // UP ARROW
#define KEY_WEST_ALT  58   // LEFT ARROW
#define KEY_SOUTH_ALT 61   // DOWN ARROW
#define KEY_EAST_ALT  60   // RIGHT ARROW
#define KEY_START     0x20 // SPACE
#define KEY_SELECT    'X'
#define KEY_SIMULATE_DISCONNECTED 9 // TAB

int mainGetButtonStateEmu(int virtualKeyCode);

uint8_t getButtonStateEmu(int virtualKeyCode) {
  return mainGetButtonStateEmu(virtualKeyCode);
}

InputDeviceStates_t getInputDeviceState() {
  InputDeviceStates_t states;
  memset(&states, 0, sizeof(InputDeviceStates_t));

  states.Connected = !getButtonStateEmu(KEY_SIMULATE_DISCONNECTED);
  states.Action    = getButtonStateEmu(KEY_ACTION);
  states.Back      = getButtonStateEmu(KEY_BACK);
  states.North     = getButtonStateEmu(KEY_NORTH) || getButtonStateEmu(KEY_NORTH_ALT);
  states.West      = getButtonStateEmu(KEY_WEST)  || getButtonStateEmu(KEY_WEST_ALT);
  states.South     = getButtonStateEmu(KEY_SOUTH) || getButtonStateEmu(KEY_SOUTH_ALT);
  states.East      = getButtonStateEmu(KEY_EAST)  || getButtonStateEmu(KEY_EAST_ALT);
  states.Start     = getButtonStateEmu(KEY_START);
  states.Select    = getButtonStateEmu(KEY_SELECT);

  return states;
}

// RS485:
void hal_rs485Send(char dataByte) {
  // Not used on linux simulator yet:
  hal_printf("hal_rs485Send\n");
}

void hal_rs485init(LockFreeFIFO_t* pFifo) {

}

// Filesystem:

static DIR* _pDir;

bool hal_findInit(char* path, FO_FIND_DATA* findData) {
  struct dirent* pDirent = NULL;

  char pSearchPath[256];
  strcpy(pSearchPath, path);
  strcat(pSearchPath, "/");

  _pDir = opendir(pSearchPath);

  if (!_pDir)
    return false;

  pDirent = readdir(_pDir);

  if (!pDirent)
    return false;

  strcpy(findData->fileName, pDirent->d_name);

  return true;
}

bool hal_findNext(FO_FIND_DATA* findData) {
  struct dirent* pDirent = readdir(_pDir);

  if (!pDirent)
    return false;

  strcpy(findData->fileName, pDirent->d_name);

  return true;
}

void hal_findFree() {
  closedir(_pDir);
}

int32_t hal_fopen(FILE** pFile, const char* pFileName) {
  FILE* p = fopen(pFileName, "rb");

  if (!p) {
    *pFile = NULL;
    return -1;
  }

  *pFile = p;

  return 0;
}

int32_t hal_fclose(FILE* pFile) {
  return fclose(pFile);
}

int32_t hal_fseek(FILE* pFile, int startPos) {
  return fseek(pFile, startPos, SEEK_SET);
}

size_t hal_fread(FILE* pFile, void* dst, size_t numBytes) {
  return fread(dst, numBytes, 1, pFile);
}

int32_t hal_ftell(FILE* pFile) {
  return ftell(pFile);
}

// Common:

static void reverse(char s[]) {
  int i, j;
  char c;

  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void itoa(int n, char s[]) {
  int i, sign;

  if ((sign = n) < 0) // record sign
    n = -n; // make n positive

  i = 0;
  do { // generate digits in reverse order
    s[i++] = n % 10 + '0'; // get next digit
  } while ((n /= 10) > 0); // delete it

  if (sign < 0)
    s[i++] = '-';

  s[i] = '\0';
  reverse(s);
}

