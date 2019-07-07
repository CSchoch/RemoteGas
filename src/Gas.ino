#include <Plotter.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;
Plotter p;
const int gasHandInput = 0;
const int gasPlusInput = 2;
const int gasMinusInput = 3;
const int gasMax = 4500; // Max gas value deviation in mV
const int gasRefernce = 5000; // Gas reference value in mV when pedal is not pushed
const int analogControllerVoltage = 5000; // Analog controller input voltage in mV
const int gasIntervall = 25; // Interwall which increases the gas
int handGas;
int handGasValue = gasRefernce;
int meanInputValue;
int actualGasValue = 0;    
int setGasValue;
int setGasValueOld = 0;
unsigned long oldTime;
unsigned long timer;
void setup(void)
{
	// Serial.begin(9600);
	// Serial.println("Starting electronic gas");
	// For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
	// For MCP4725A0 the address is 0x60 or 0x61
	// For MCP4725A2 the address is 0x64 or 0x65
	dac.begin(0x60);
	dac.setNearestActualVoltage(gasRefernce - actualGasValue, analogControllerVoltage, false);
	p.Begin();
	p.AddTimeGraph("Werte", 1000, "Handgas", handGas, "Sollwert Winde", actualGasValue, "Sollwert Gesammt", setGasValue);
	pinMode(gasPlusInput, INPUT);
	pinMode(gasMinusInput, INPUT);
}

void loop(void)
{
	if (timer >= gasIntervall)
	{
		oldTime = millis();
	}
	// create timer
	if (oldTime > millis())
	{
		timer = (4294967295 - oldTime) + millis();
	}
	else
	{
		timer = (millis() - oldTime);
	}
	// Get actual handgas value
	handGasValue = 0.1 * analogRead(gasHandInput) + 0.9 * handGasValue;
	handGas = (handGasValue / 1024.0) * analogControllerVoltage;
	// Increase gas
	if (digitalRead(gasPlusInput) && (timer >= gasIntervall))
	{
		actualGasValue = min(actualGasValue + 5, gasMax);
	}
	// Decrease gas
	if (digitalRead(gasMinusInput) && (timer >= gasIntervall))
	{
		actualGasValue = max(actualGasValue - 5, 0);
	}
	// Write selected gas value
	setGasValue = max(min(handGas - actualGasValue, 4500), 500);
	if (setGasValue != setGasValueOld)
	{
		// Serial.println("Set gas value to: " + String(setGasValue));
		dac.setNearestActualVoltage(setGasValue, analogControllerVoltage, false);
		setGasValueOld = setGasValue;
	}
	// Löschen b_Test
	meanInputValue = 0.1 * analogRead(1) + 0.9 * meanInputValue;
	// Serial.println(handGas);
	p.Plot();
}
