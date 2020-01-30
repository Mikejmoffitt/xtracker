#ifndef _XT_TRACK_H
#define _XT_TRACK_H

// Some program configuration.
#define XT_PHRASE_MAX_ROWS 64
#define XT_PHRASES_PER_CHANNEL 96
#define XT_FM_CHANNEL_COUNT 8
#define XT_PCM_CHANNEL_COUNT 4
#define XT_TOTAL_CHANNEL_COUNT (XT_FM_CHANNEL_COUNT + XT_PCM_CHANNEL_COUNT)
#define XT_FRAME_COUNT 128
#define XT_INSTRUMENT_COUNT 64
#define XT_SAMPLE_COUNT 64

typedef enum XtCmd
{
	XT_CMD_NONE = '\0',

	XT_CMD_TL_OP0 = '0',
	XT_CMD_TL_OP1 = '1',
	XT_CMD_TL_OP2 = '2',
	XT_CMD_TL_OP3 = '3',

	XT_CMD_MULT_OP0 = '4',
	XT_CMD_MULT_OP1 = '5',
	XT_CMD_MULT_OP2 = '6',
	XT_CMD_MULT_OP3 = '7',

	XT_CMD_AMPLITUDE = 'A',

	XT_CMD_BREAK = 'B',
	XT_CMD_HALT = 'C',
	XT_CMD_SKIP = 'D',

	XT_CMD_SPEED = 'F',

	XT_CMD_NOISE_EN = 'N',

	XT_CMD_PAN = 'O',

	XT_CMD_TUNE = 'P',

	XT_CMD_TREMOLO = 'T',
	XT_CMD_VIBRATO = 'V',
	XT_CMD_TREMOLO_TYPE = 'G',
	XT_CMD_VIBRATO_TYPE = 'H',

	XT_CMD_SLIDE_UP = 'Q',
	XT_CMD_SLIDE_DOWN = 'R',
	XT_CMD_MUTE_DELAY = 'S',
	XT_CMD_NOTE_DELAY = 'W',
	XT_CMD_CUT_DELAY = 'X',

	XT_CMD_MAX = 0xFF,
} XtCmd;

typedef enum XtNote
{
	XT_NOTE_NONE = 0x0,

	// Main notes. These align to OPM note values when 1 is subtracted.
	XT_NOTE_CS   = OPM_NOTE_CS + 1,
	XT_NOTE_D    = OPM_NOTE_D + 1,
	XT_NOTE_DS   = OPM_NOTE_DS + 1,
	XT_NOTE_E    = OPM_NOTE_E + 1,
	XT_NOTE_F    = OPM_NOTE_F + 1,
	XT_NOTE_FS   = OPM_NOTE_FS + 1,
	XT_NOTE_G    = OPM_NOTE_G + 1,
	XT_NOTE_GS   = OPM_NOTE_GS + 1,
	XT_NOTE_A    = OPM_NOTE_A + 1,
	XT_NOTE_AS   = OPM_NOTE_AS + 1,
	XT_NOTE_B    = OPM_NOTE_B + 1,
	XT_NOTE_C    = OPM_NOTE_C + 1,

	XT_NOTE_OFF = 0xFE,
	XT_NOTE_CUT = 0xFF,
} XtNote;

#define XT_NOTE_TONE_MASK      0x0F
#define XT_NOTE_OCTAVE_MASK    0xF0

// A single line of a phrase pattern.
typedef struct XtCell
{
	XtNote note;
	uint8_t inst;
	XtCmd cmd1;
	uint8_t arg1;
	XtCmd cmd2;
	uint8_t arg2;
} XtCell;

// Vertical collection of cells for one channel.
typedef struct XtPhrase
{
	XtCell cells[XT_PHRASE_MAX_ROWS];
	uint16_t phrase_valid;
} XtPhrase;

// One row within the arrangement table.
typedef struct XtFrame
{
	uint16_t row_idx[XT_TOTAL_CHANNEL_COUNT];
} XtFrame;

typedef struct XtSample
{
	char *name[64];
	uint16_t data_valid;
	uint16_t data_size;
	uint8_t *data;
} XtSample;

typedef struct XtInstrument
{
	uint8_t reg_20_pan_fl_con;  // Rch (1), Lch(1), FL(3), CON(3)
	uint8_t reg_38_pms_ams;     // null (1), PMS(3), null(2), AMS(2)
	uint8_t reg_40_dt1_mul[4];  // null (1), DT1(3), MUL(4)
	uint8_t reg_60_tl[4];       // null(1), TL(7)
	uint8_t reg_80_ks_ar[4];    // ks(2), null(1), AR(5)
	uint8_t reg_A0_ame_d1r[4];  // ame(1), null(2), D1R(5)
	uint8_t reg_C0_dt2_d2r[4];  // dt2(2), null(1), D2R(5)
	uint8_t reg_E0_d1l_rr[4];   // D1L(4), RR(4)
} XtInstrument;

// One whole song in memory.
typedef struct XtTrack
{
	char title[256];
	uint16_t num_phrases;
	XtPhrase phrases[XT_TOTAL_CHANNEL_COUNT * XT_PHRASES_PER_CHANNEL];
	uint16_t num_frames;
	XtFrame frames[XT_FRAME_COUNT];
	uint16_t num_instruments;
	XtInstrument instruments[XT_INSTRUMENT_COUNT];
	uint16_t num_samples;
	XtSample samples[XT_SAMPLE_COUNT];

	int16_t ticks_per_row_base;  // Number of ticks that elapse per row.
	int16_t ticks_per_row;  // Current ticks per row (can change during play)
	int16_t timer_period;  // Period of ticks.

	int16_t phrase_length;  // How many cell rows are in a phrase.
	int16_t loop_point;  // Arrangement row to return to (-1 for no repeat).

	char memo[1024];
} XtTrack;

#endif  // _XT_TRACK_H
