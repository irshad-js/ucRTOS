#include <stdio.h>
#include <stdarg.h>
#include <lut.h>
#include <ff.h>
#include "stm32f4xx_conf.h"
#include "ssd1289.h"
#include "nesgamepad.h"
#include "ucrtos.h"
#include "display.h"
#include "../../core/ucrtos.h"
#include "../../../eMIDI/src/midifile.h"
#include "../../../../../lib/LockFreeFifo/LockFreeFifo.h"

// LED

void statusLedOn() {
  GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

void statusLedOff() {
  GPIO_ResetBits(GPIOD, GPIO_Pin_12);
}

void errorLedOn() {
  GPIO_SetBits(GPIOD, GPIO_Pin_13);
}

void errorLedOff() {
  GPIO_ResetBits(GPIOD, GPIO_Pin_13);
}

void errorState() {
  while(1) {
    errorLedOn();
    delayMs(50);
    errorLedOff();
    delayMs(500);
  }
}

// Display

static uint8_t* _pFrameBuffer = 0;
static int _xMax = 0;
static int _yMax = 0;

void hardwareDisplayInit(uint8_t* pFrameBuffer, int xMax, int yMax) {
  _pFrameBuffer = pFrameBuffer;
  _xMax = xMax;
  _yMax = yMax;

  SSD1289_Init();
  SSD1289_Clear(Black);
}

void hardwareDisplayDraw() {
  for(int y = 0; y < _yMax; ++y)
    for(int x = 0; x < _xMax; ++x) {
      int idx = y * _xMax + x;
      int subIdx = idx / 2;
      int isHigh = idx % 2 == 0;
      uint8_t ci = 0;

      if(!isHigh)
        ci = _pFrameBuffer[subIdx] & 0x0F;
      else
        ci = (_pFrameBuffer[subIdx] & 0xF0) >> 4;

      SSD1289_SetPoint(x, y, RGB565CONVERT(pMasterPalette[ci].red, pMasterPalette[ci].green, pMasterPalette[ci].blue));
    }
}

// MIDI device:

void hardwareInitMidiDevice() {
  // TODO: implement
}

void hardwareFreeMidiDevice() {
  // TODO: implement
}

void hardwareSendMidiMsg(const MidiEvent* pEvent) {
  // TODO: implement
}

// Input Device
void hardwareInputDeviceInit() {
  setupNesGamePad();
}

// Dummy function to avoid compiler error (Is called by libc_init_array()):

void _init() {
  // Just leave empty
}

// Debug Interface:

void PrintCharUsr(char c) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // Wait until transmit finishes
  USART_SendData(USART1, (u8) c);
}

InputDeviceStates_t getInputDeviceState() {
  InputDeviceStates_t states;
  union NesGamePadStates_t nesStates = getNesGamepadState();

  states.Action = nesStates.states.A;
  states.Back   = nesStates.states.B;
  states.East   = nesStates.states.East;
  states.West   = nesStates.states.West;
  states.North  = nesStates.states.North;
  states.South  = nesStates.states.South;
  states.Start  = nesStates.states.Start;
  states.Select = nesStates.states.Select;

  states.Connected = nesStates.code != 0xFF;

  return states;
}

void hal_printf(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_printfWarning(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_printfError(char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);
  myvprintf(pFormat, args);
  va_end(args);
}

void hal_strcpy_s(char* dst, int maxSize, const char* src) {
  strcpy(dst, src);
}

// RS485:

static LockFreeFIFO_t* _pFifoDebugPort = 0;

void hal_rs485init(LockFreeFIFO_t* pFifo) {
  _pFifoDebugPort = pFifo;
}

void hal_rs485Send(char dataByte) {
  while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
  USART_SendData(USART6, dataByte);
}

void USART1_IRQHandler() {
  // check if the USART1 receive interrupt flag was set
  if(USART_GetITStatus(USART1, USART_IT_RXNE))
    writeToRingBuffer(_pFifoDebugPort, USART1->DR); // The Interrupt gets released just by reading USART1->DR
}

// Filesystem:

static FATFS SD_Fs;
static DIR dirs;

static bool mountSdCard() {
  /* Try to mount SD card */
  /* SD card is at 0: */
  hal_printf("Mounting SD card... ");
  if (f_mount(&SD_Fs, "0:", 1) == FR_OK) {
    hal_printf("success!\n\r"); /* Mounted ok */

    /* Get total and free space on SD card */
    uint32_t free, total;
    TM_FATFS_DriveSize(&total, &free);

    /* Total space */
    hal_printf("Total: %8u kB; %5u MB; %2u GB\n\r", total, total / 1024, total / 1048576);
    hal_printf("Free:  %8u kB; %5u MB; %2u GB\n\r", free, free / 1024, free / 1048576);
    return true;
  }
  else {
    hal_printf("Failed! No SD-Card?\n\r");
    return false;
  }
}

void hal_fileSystemInit() {
  mountSdCard();
}

bool hal_findNext(FO_FIND_DATA* findData) {
  static FILINFO finfo;
  finfo.lfname = findData->fileName;
  finfo.lfsize = 256;
  FRESULT fres = f_readdir(&dirs, &finfo);
  bool foundFile = finfo.lfname[0];

  return fres == FR_OK && foundFile;
}

bool hal_findInit(char* path, FO_FIND_DATA* findData) {
  FRESULT fres = f_opendir(&dirs, "/");
  return fres == FR_OK && hal_findNext(findData);
}

void hal_findFree() {
  // Not needed in this implementation.
}

// Returns 1, if file was opened successfully or 0 on error.
// TODO: check pointer dereferencing here!!!
int32_t hal_fopen(FILE** pFile, const char* pFileName) {
  static FIL file;
  FRESULT res = f_open(&file, pFileName, FA_READ);
  *pFile = &file;

  return res == FR_OK;
}

int32_t hal_fclose(FILE* pFile) {
  return f_close((FIL*)pFile);
}

int32_t hal_fseek(FILE* pFile, int startPos) {
  return f_lseek((FIL*)pFile, startPos);
}

size_t hal_fread(FILE* pFile, void* dst, size_t numBytes) {
  UINT bytesRead;
  f_read((FIL*)pFile, dst, numBytes, &bytesRead); // TODO: word access? Does it increase performance?
  return bytesRead;
}

int32_t hal_ftell(FILE* pFile) {
  return f_tell((FIL*)pFile);
}

// eMIDI:

static int _filePos = 0;
static uint32_t _fileSize = 0;

int eMidi_timeUs() {
  return upTimeMs() * 1000;
}

FILE* eMidi_fopen(const char* pPathName, const char* pMode) {
  FILE* pFile;
  hal_fopen(&pFile, pPathName);

  if (!pFile)
    return NULL;

  _filePos = 0;
  _fileSize = f_size((FIL*)pFile);

  return pFile;
}

int eMidi_fclose(FILE* pStream) {
  return hal_fclose(pStream);
}

long eMidi_ftell(FILE* pStream) {
  return hal_ftell(pStream);
}

int eMidi_fseek(FILE* pStream, long offset, int whence) {
  switch (whence) {
    case SEEK_CUR: _filePos += offset;   break;
    case SEEK_SET: _filePos = 0;         break;
    case SEEK_END: _filePos = _fileSize; break;
      break;

    default:
      return -1;
  }

  return hal_fseek(pStream, _filePos);
}

size_t eMidi_fread(void* p, size_t size, size_t nmemb, FILE* pStream) {
  int numBytesRead = hal_fread(pStream, p, nmemb);
  _filePos += numBytesRead;

  return numBytesRead;
}

size_t eMidi_fwrite(const void* p, size_t size, size_t nmemb, FILE* pStream) {
  return -1; // Not supported
}

int eMidi_printf(const char* pFormat, ...) {
  va_list args;
  va_start(args, pFormat);

  int ret = myvprintf(pFormat, args);

  va_end(args);
  return ret;
}

