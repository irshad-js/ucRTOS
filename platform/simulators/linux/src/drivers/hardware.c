#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <linux/soundcard.h>
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

// Colored print:

static void _printColored(const char* text, uint16_t colorAttributes) {
//  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
//  WORD saved_attributes;

//  GetConsoleScreenBufferInfo(hConsole, &consoleInfo); // Save current font color
//  saved_attributes = consoleInfo.wAttributes;
//  SetConsoleTextAttribute(hConsole, colorAttributes); // Change font color
//  printf(text);
//  SetConsoleTextAttribute(hConsole, saved_attributes); // Restore original font color
//  printf("\n\r");

  printf("%s\n\r", text);
}

void hal_printf(char* format, ...) {
  char formattedText[512];

  va_list args;
  va_start(args, format);
  vsnprintf(formattedText, sizeof(formattedText), format, args);
  _printColored(formattedText, 0); // White
  va_end(args);
}

void hal_printfError(const char* format, ...) {
  char formattedText[512];

  va_list args;
  va_start(args, format);
  // vsnprintf_s(formattedText, sizeof(formattedText), sizeof(formattedText), format, args);
  // _printColored(formattedText, FOREGROUND_RED | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_printfWarning(char* format, ...) {
  char formattedText[512];

  va_list args;
  va_start(args, format);
  // vsnprintf_s(formattedText, sizeof(formattedText), sizeof(formattedText), format, args);
  // _printColored(formattedText, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow
  va_end(args);
}

void hal_printfSuccess(char* format, ...) {
  char formattedText[512];

  va_list args;
  va_start(args, format);
  // vsnprintf_s(formattedText, sizeof(formattedText), sizeof(formattedText), format, args);
  // _printColored(formattedText, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  va_end(args);
}

void hal_printfInfo(char* format, ...) {
  char formattedText[512];

  va_list args;
  va_start(args, format);
  // vsnprintf_s(formattedText, sizeof(formattedText), sizeof(formattedText), format, args);
  // _printColored(formattedText, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  va_end(args);
}

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

