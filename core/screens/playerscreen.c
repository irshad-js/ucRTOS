#include "../StackBasedFsm.h"
#include "../../lib/colorprint/colorprint.h"
#include "../../lib/eMIDI/src/midiplayer.h"
#include "../../lib/eMIDI/src/helpers.h"
#include "../../core/ucrtos.h"
#include "../display.h"

#include "playerscreen.h"

static struct {
  MidiPlayer player;
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

static void userEventCallback(const MidiEvent* pEvent, void* pContext) {
  eMidi_printMidiEvent(pEvent);

	int numParamBytes = 0;

	switch (pEvent->eventId & 0xF0) {
	case MIDI_EVENT_NOTE_OFF:          numParamBytes = 2; break;
	case MIDI_EVENT_NOTE_ON:           numParamBytes = 2; break;
		//    case MIDI_EVENT_POLY_KEY_PRESSURE: numParamBytes = 2; break;
	case MIDI_EVENT_CONTROL_CHANGE:    numParamBytes = 2; break;
	case MIDI_EVENT_PROGRAM_CHANGE:    numParamBytes = 1; break;
		//    case MIDI_EVENT_CHANNEL_PRESSURE:  numParamBytes = 1; break;
	case MIDI_EVENT_PITCH_BEND:        numParamBytes = 2; break;

	default:
		return;
	}

  hardwareSendMidiMsg(pEvent);
}

static void onEnter(StackBasedFsm_t* pFsm, void* pParams) {
  hal_printf("player::onEnter\n");

  hardwareInitMidiDevice();

  PlayerScreenParams* pPlayerParams = (PlayerScreenParams*)pParams;
  if (!pPlayerParams)
    hal_printfWarning("Param is nullptr\n");
  else {
    context.someLocalVariable = pPlayerParams->someInt;
    context.pReturnValue = pPlayerParams->pReturnValue;
  }

  Error error;
  // error = eMidi_openPlayer(&context.player, "..\\..\\..\\lib\\eMIDI\\tests\\midis\\cdefgabc_0.mid", userEventCallback, NULL);
  error = eMidi_openPlayer(&context.player, "..\\..\\..\\lib\\eMIDI\\tests\\midis\\fish_fry0.mid", userEventCallback, NULL);

  if (error) {
    hal_printfError("Error on opening midi file!\n");
    eMidi_printError(error);

    leaveState(pFsm);
    return;
  }

  hal_printf("Midi file opened!\n");

  draw();
}

static void onLeaveState(StackBasedFsm_t* pFsm) {
  hal_printf("player::onLeaveState\n");

  eMidi_closePlayer(&context.player);
  hardwareFreeMidiDevice();
}

static void onBackPress(StackBasedFsm_t* pFsm) {
  hal_printf("player::onBackPress\n");

  leaveState(pFsm);
}

static void onTick(StackBasedFsm_t* pFsm) {
  // hal_printf("player::onTick()");

  if (eMidi_playerTick(&context.player) != EMIDI_OK)
    leaveState(pFsm);
}

// Always implement this as last function of your state file:

void playerScreen(StackBasedFsm_t* pFsm, FsmState* pState) {
  pState->onEnterState = onEnter;
  pState->onLeaveState = onLeaveState;
  pState->onBackPress  = onBackPress;
  pState->onTick       = onTick;
}

