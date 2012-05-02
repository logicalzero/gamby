// Various LCD commands. Sent to the LCD when Register Select (RS) is LOW.

#define	POWER_CONTROL		B00101111	// all power circuits ON (LCD controlled)
#define	DC_STEP_UP		B01100100	// 011001xx, xx select 3-5x boosting
#define	REGULATOR_RESISTOR	B00100000	// 00100xxx (center)
#define	SET_EVR_1		B10000001	// 2-byte instruction!
//#define SET_EVR_2		B00011100	// XXxxxxxx, xxxxxx = value, XX = don't care. (center)
#define	SET_EVR_2		B00011011	// XXxxxxxx, xxxxxx = value, XX = don't care. (center)
#define	SET_DUTY_1		B01001000 	// 2-byte instruction!
#define	SET_DUTY_2		B01000000
#define	SET_BIAS		B01010001	// 01010xxx, xxx = bias. Should be 001 for 1/5 bias
#define	SET_OSC_ON		B10101011	// Starts internal oscillator
#define	SHL_SELECT_REVERSE	B11001000	// 1100xXXX, x = COM scanning direction. 0 normal, 1 reverse. X = don't care.
#define	ADC_SELECT		B10100001	// 1010000x

#define	REGULATOR_RESISTOR_VAL	0x01		// range 0x00 ~ 0x03 // resistor value '001' @ 3.3V and 3x boost seems to work nicely
#define	VOLUME_CONTROL_VAL	0x0A		// range 0x00 ~ 0x3F; 0x1C was first try

#define	ALL_BLACK		B10100100	// 1010010x, x=1 forces all pixels on. 'Entire Display ON' in datasheet.
#define	DISPLAY_INVERT_OFF     	B10100110	// 1010011x, x = invert. 0 normal, 1 inverse video.
#define	DISPLAY_INVERT_ON	B10100111	// 1010011x, x = invert. 0 normal, 1 inverse video.
#define	POWER_SAVE_STANDBY     	B10101000	// 1010100x, x = power save level (0=standby, 1=sleep)
#define	POWER_SAVE_SLEEP    	B10101001	// 1010100x, x = power save level (0=standby, 1=sleep)
#define	POWER_SAVE_CLEAR	B11100001
#define	SOFT_RESET		B11100010	// Initialize the internal functions

#define DISPLAY_POWER_OFF      	B10101110	// B1010111x, 0=off, 1=on
#define DISPLAY_POWER_ON       	B10101111	// B1010111x, 0=off, 1=on
#define SET_N_LINE_INVERSION_1	B01001100	// 2 byte; 2nd is XXXxxxxx; X = don't care. See datasheet pg. 43.
#define CLEAR_N_LINE_INVERSION	B11100100
#define SET_INITIAL_COM0_1	B01000100	// 2 byte, 2nd is 00XXxxxx; X = don't care
#define SET_INITIAL_LINE_1	B01000000	// 2 byte, 2nd is 0Xxxxxxx

#define	SET_PAGE_ADDR		B10110000	// 1011xxxx, xxxx = page addr.
#define	SET_COLUMN_ADDR_1	B00010010	// 2-byte! 00010xxx, xxx = Y[6..4] (32 (bit 5) added for offset to work inverted)
#define	SET_COLUMN_ADDR_2	B00000000	// 0000xxxx, xxxx = Y[3..0]
