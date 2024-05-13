#include "settings.h"
#include <time_manager.h>
#include "sensors.h"
#include "distiller.h"


//! -------------------------------- SETUP ----------------------
void setup(void) {
	Serial.begin(115200);
	delay(100);


}

//!--------------------------------------- LOOP ------------------------------
void loop(void) {
	SensorsManager* sensorsManager = SensorsManager::getInstance();
	Distiller* distiller = Distiller::getInstance();
	sensorsManager->tick();
	distiller->tick();
	
	static uint64_t cycle = 0;

	static TimeManager timer(1000);
	if (timer.IsReady()) {
		const auto start = millis();
		Serial.printf("Sensors cout is %u \n", sensorsManager->getSensorsCount());
		for (const auto& sensor : sensorsManager->getSensors()) {
			Serial.printf("%s: %.3f C, correction: %.3f, hash: %u\n", 
				sensor.getName().c_str(), 
				sensor.getTemperature(), 
				sensor.getCorrection(), 
				sensor.getHash()
				);
		}
		Serial.printf("bmpTemperature: %.3f C, bmpPressure: %.3f mmHg\n", 
			sensorsManager->getBmpCorrectTemperature(), 
			sensorsManager->getBmpCorrectPressure()
			);
		const auto end = millis();
		Serial.printf("loop time: %u, cycle: %u \n", (end - start), cycle);
	}
	cycle++;
}

