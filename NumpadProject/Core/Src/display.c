// Display.c
// By Alexander Alt

#include "display.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

//-------------------------------------------------------------------------------
// When DC is LOW, SSD1305 sends bytes to command register
// When DC is HIGH, SSD1305 sends bytes to the GDDRAM (Graphic data DRAM)
// When CS is LOW, SSD1305 listens for incoming data, starts sampling clock/data
// When CS is HIGH, SSD1305 ignores the data/clock lines (Active low)
//-------------------------------------------------------------------------------

// Screen buffer to send over SPI
static uint8_t ScreenBuffer[1024];

// Function to send commands over SPI
void ScreenWriteCommand(uint8_t Command) {
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET); // Pull DC low to send command
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET); // Pull CS low to activate
	HAL_SPI_Transmit(&OLED_SPI, &Command, 1, HAL_MAX_DELAY); // Send the command over SPI
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET); // Pull CS high to deactivate
}

// Function to send data to the screen
void ScreenWriteData(uint8_t *Data, uint16_t Size) {
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET); // Pull DC high to send data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET); // Pull CS low to activate
	HAL_SPI_Transmit(&OLED_SPI, Data, Size, HAL_MAX_DELAY); // Send the data over SPI
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET); // Pull CS high to deactivate
}

void ScreenUpdate(void) {
    for (uint8_t page = 0; page < 8; page++) {
        ScreenWriteCommand(0xB0 + page); // Set Page Start Address (0xB0 to 0xB7)
        ScreenWriteCommand(0x00);        // Set Lower Column Start Address
        ScreenWriteCommand(0x10);        // Set Higher Column Start Address

        ScreenWriteData(&ScreenBuffer[page * 128], 128);
    }
}

// Function to initialize the screen
void ScreenInit(void) {
    // Hardware Reset Pulse
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10); // Hold reset LOW for 10ms
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100); // Wait 100ms for 3.3V rail & 220uF capacitor to stabilize

    // Display Off
    ScreenWriteCommand(0xAE); // Display OFF (Sleep mode)

    // Clock & Multiplex Configuration
    ScreenWriteCommand(0xD5); // Set Display Clock Divide Ratio / Oscillator Frequency
    ScreenWriteCommand(0x80); // Default ratio
    ScreenWriteCommand(0xA8); // Set Multiplex Ratio
    ScreenWriteCommand(0x3F); // 64MUX (for 64 vertical rows)
    ScreenWriteCommand(0xD3); // Set Display Offset
    ScreenWriteCommand(0x00); // No vertical offset
    ScreenWriteCommand(0x40); // Set Display Start Line to 0

    // SSD1305 Master Configuration & Power Booster
    ScreenWriteCommand(0xAD); // Master Configuration
    ScreenWriteCommand(0x8E); // Enable external VCC / internal charge pump booster

    // Screen Orientation & Pin Hardware Mapping (Adafruit 2.42" layout)
    ScreenWriteCommand(0xA1); // Segment Re-map (Column 131 mapped to SEG0)
    ScreenWriteCommand(0xC8); // COM Output Scan Direction (Remapped top-to-bottom)
    ScreenWriteCommand(0xDA); // Set COM Pins Hardware Configuration
    ScreenWriteCommand(0x12); // Alternative COM pin config, disable left/right remap

    // Contrast, Pre-Charge, & VCOMH Deselect
    ScreenWriteCommand(0x81); // Set Current Control (Contrast)
    ScreenWriteCommand(0xCF); // High brightness/contrast (0x00 to 0xFF)
    ScreenWriteCommand(0xD9); // Set Pre-Charge Period
    ScreenWriteCommand(0xF1); // Phase 1 = 1 DCLK, Phase 2 = 15 DCLKs
    ScreenWriteCommand(0xDB); // Set VCOMH Deselect Level
    ScreenWriteCommand(0x40); // ~0.83 x VCC

    // Clear RAM Before Turning Panel On
    ScreenClear();            // Zero out local 1,024-byte buffer
    ScreenUpdate();           // Flush zeros to GDDRAM while display is off

    // Resume RAM Display & Turn Panel ON
    ScreenWriteCommand(0xA4); // Output follows GDDRAM content
    ScreenWriteCommand(0xA6); // Normal display (non-inverted)
    ScreenWriteCommand(0xAF); // Display ON
}

//---------------------------------------------------------------------------------------
// Within the draw character function, characters including carat, pi and square root
// are assigned their own ascii values of 128, 129 and 130 respectively, this is because
// pi and root are not normal characters on the ascii table, and the carat on the ascii
// table is smaller than the font I wanted to display it in.
//---------------------------------------------------------------------------------------

// Function to draw a character on the screen
void ScreenDrawChar(uint16_t x, uint16_t y, char Ch, FontType Font) {
	if (!Font.IsFixed16) { // For 5x7 Font
		if (Ch < 32 || Ch > 126) Ch = ' '; // Replace unknown character with space
		uint16_t Offset = (Ch - 32) * 5;

		for (uint8_t Column = 0; Column < 5; Column++) { // Unpack each column
			uint8_t ByteData = Font.Data[Offset + Column]; // Reads the column for the character to be printed
			for (uint8_t Bit = 0; Bit < 7; Bit++) { // Unpack each bit in the column
				uint8_t OnOrOff = (ByteData >> Bit) & 0x01;
				ScreenDrawPixel(x + Column, y + Bit, OnOrOff);
			}
		}
	}

	else { // For 8x12 font
		uint8_t AsciiIndex = (uint8_t)Ch;
		if (Ch == '^') AsciiIndex = 128;
		else if (Ch == 'p') AsciiIndex = 129;
		else if (Ch == 'r') AsciiIndex = 130;

		const uint8_t *CharBase = &Font.Data[AsciiIndex * 16]; // Get characger footprint address

		for (uint8_t Column = 0; Column <8; Column++) { // Move across each column of the character
			uint8_t TopByte = CharBase[Column]; // Get the first 8 vertical rows
			uint8_t BottomByte = CharBase[Column + 8]; // Get the bottom 4 vertical rows
			// Pull each bit from the rows and map them out on the screen
			// Basically shift the row right to each bit, and the &0x01 sees if it is on
			for (uint8_t Bit = 0; Bit < 8; Bit++) ScreenDrawPixel(x + Column, y + Bit, (TopByte >> Bit) & 0x01);
			for (uint8_t Bit = 0; Bit < 4; Bit++) ScreenDrawPixel(x + Column, y + 8 + Bit, (BottomByte >> Bit) & 0x01);
		}
	}
}

// Function to draw a pixel on the screen
void ScreenDrawPixel(int16_t x, int16_t y, uint8_t Color) {
	if (x < 0 || x >= 128 || y < 0 || y >= 64) return; // Return if out of bounds

	uint16_t PixelIndex = x + (y / 8) * 128; // Find the index of this bit in the 1024-byte array
	uint8_t BitPosition = y % 8; // Find the position of this bit in the byte

	if (Color == 1) ScreenBuffer[PixelIndex] |= (1 << BitPosition); // ON
	else ScreenBuffer[PixelIndex] &= ~(1 << BitPosition); // OFF
}

// Function to display a string on the screen
void ScreenPrintString(uint16_t x, uint16_t y, const char *String, FontType Font) {
	while (*String != '\0') { // Print every character until the null terminator
		if (x + Font.Width > 128) break; // Exit loop to prevent from overrunning screen width
		ScreenDrawChar(x, y, *String, Font);
		x += (Font.Width + 1);
		String++;
	}
}

// Function to clear the screen buffer
void ScreenClear(void) { memset(ScreenBuffer, 0, sizeof(ScreenBuffer)); }

// Function to make the string scroll over when it reaches the left side of the screen
void PrintWithScroll(uint16_t x, uint16_t y, const char *String, FontType Font) {
	uint16_t Length = strlen(String);
	uint16_t CharStep = Font.Width + 1; // Width of one character with a small gap after
	uint16_t MaxChars = (128 - x) / CharStep; //Maximum number of characters that fit on the screen
	uint16_t StartIndex = 0;

	if (Length > MaxChars) StartIndex = Length - MaxChars;
	ScreenPrintString(x, y, &String[StartIndex], Font);
}






