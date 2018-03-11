#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../../lib/eMIDI/src/midifile.h"
#include "../../lib/eMIDI/src/helpers.h"
#include "../../core/ucrtos.h"
#include "../display.h"

#include "playerscreen.h"


static struct {
  MidiFile midi;
  int someLocalVariable;
  int* pReturnValue;
} context;

static void draw() {
  displayClear();

  displayDrawText(CENTER, 0 + 0 * 18, "Player screen", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 1 * 18, "Press 'Back' button to exit", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);
  displayDrawText(CENTER, 0 + 3 * 18, "Now playing...", 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00);

  displayDraw();
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("player::onEnter()");

  hardwareInitMidiDevice();

  PlayerScreenParams* pPlayerParams = (PlayerScreenParams*)pParams;
  if (!pPlayerParams)
    hal_printfWarning("Param is nullptr");
  else {
    context.someLocalVariable = pPlayerParams->someInt;
    context.pReturnValue = pPlayerParams->pReturnValue;
  }

  Error error;
  error = eMidi_open(&context.midi, "C:\\Users\\Stephan\\Music\\bridge0.mid");
  // error = eMidi_open(&context.midi, "C:\\Users\\Stephan\\Music\\cdefgabc_0.mid");

  if (error) {
    printf("Error on opening midi file!\n");
    eMidi_printError(error);

    leaveState(pFsm);
    return;
  }

  printf("Midi file opened!\n");

  draw();

  MidiEvent e;

  static const uint32_t c = 60000000;
  uint32_t bpm = 120;
  uint32_t uspqn = c / bpm;

  do {
    error = eMidi_readEvent(&context.midi, &e);
    if (error) {
      printf("Error on reading event: [0x%02X] (Error %d: %s)\n", e.eventId, error, eMidi_errorToStr(error));
      return;
    }

    if (e.eventId == MIDI_EVENT_META) {
      if (e.metaEventId == MIDI_SET_TEMPO)
        uspqn = e.params.msg.meta.setTempo.usPerQuarterNote;
    }

    uint32_t TQPN = context.midi.header.division.tqpn.TQPN;
    uint32_t usToWait = (e.deltaTime * uspqn) / TQPN;

    delayUs(usToWait);

    eMidi_printMidiEvent(&e);
    hardwareSendMidiMsg(&e);

  } while (!(e.eventId == MIDI_EVENT_META && e.metaEventId == MIDI_END_OF_TRACK));

  leaveState(pFsm);
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("player::onLeaveState()");

  eMidi_close(&context.midi);
  hardwareFreeMidiDevice();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("player::onBackPress()");

  leaveState(pFsm);
}

static void onTick(StackBasedFsm_t* pFsm) {
  // hal_printf("player::onTick()");

}

// Always implement this as last function of your state file:

void playerScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState = onEnter;
  pState->onLeaveState = onLeaveState;
  pState->onBackPress  = onBackPress;
  pState->onTick       = onTick;
}
