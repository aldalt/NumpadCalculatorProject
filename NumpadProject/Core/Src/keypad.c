// Keypad.c
// By Alexander Alt

#include "keypad.h"

// Definition of key matrix
const char KeyMatrix[NUMROWS][NUMCOLS] = { // This will need to be made a 3 dimensional array for layers
		{'1', '2', '3', '4', '5', '6'},
		{'7', '8', '9', '0', 'A', 'B'},
		{'C', 'D', 'E', 'F', 'G', 'H'},
		{'I', 'J', 'K', 'L', 'M', 'N'},
		{'O', 'P', 'Q', 'R', 'S', 'T'}
};

// Definition of ports and pins for all of the rows and columns
GPIO_TypeDef* RowPorts[NUMROWS] = {GPIOA, GPIOA, GPIOA, GPIOA, GPIOA};
GPIO_TypeDef* ColPorts[NUMCOLS] = {GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB};
uint16_t RowPins[NUMROWS] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};
uint16_t ColPins[NUMCOLS] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8};

// Function to scan the keypad and look for key presses
char ScanKeyPress(void) {
	for (uint8_t a = 0; a < NUMROWS; a++) { // Iterate over each row
		HAL_GPIO_WritePin(RowPorts[a], RowPins[a], GPIO_PIN_RESET); // Write the pin low
		for (volatile uint16_t b = 0; b < 100; b++) {} // This loop is to create a 1 microsecond delay (Clock runs at 100 MHz)

		for (uint8_t c = 0; c < NUMCOLS; c++) {// Find which column is low (this means a key is being pressed)
			if (HAL_GPIO_ReadPin(ColPorts[c], ColPins[c]) == GPIO_PIN_RESET) {
				HAL_GPIO_WritePin(RowPorts[a], RowPins[a], GPIO_PIN_SET);
				return KeyMatrix[a][c];
			}
		}
		HAL_GPIO_WritePin(RowPorts[a], RowPins[a], GPIO_PIN_SET); // Set the pin high again, to move onto the next one
	}
	return '\0';
}

// Function to get and validate the key presses from the matrix
char GetCurrentKey(void) {
	static char LastKey = '\0'; // Last key press read
	static char ConfKey = '\0'; // Confirmed key press to be returned/last key press actually returned
	static uint32_t LastTime = 0; // Last time that the key press was read

	char CurrKey = ScanKeyPress();

	if (CurrKey != LastKey) { // If the keys are different then we know there is no bouncing
		LastTime = HAL_GetTick();
		LastKey = CurrKey;
	}

	if (HAL_GetTick() - LastTime >= 30) { // Only do anything if there has been a 30ms delay, to prevent bouncing
		if (CurrKey != ConfKey) { // Since we only want to return new key presses, check against last key press
			ConfKey = CurrKey;
			if (ConfKey != '\0') return ConfKey; // We only want to return on an actual key press, otherwise we will return NULL
		}
	}
	return '\0';
}

