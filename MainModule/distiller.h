#ifndef DISTILLER_H
#define DISTILLER_H

#include "settings.h"
#include "sensors.h"
#include <map> //From library ArduinoSTL


class Distiller {
private:
	enum {
		IDLE = 0,
		HEATING,
		STABILIZATION,
		HEAD,
		BODY, 
		COMPLETION
	} _state;
	
	DistillerMode _mode;

	std::map<SensorLocation, SensorHash> _sensorsMap;
	void autoTick();
	void semiAutoTick();
	void manualTick();

public:
	explicit Distiller();
	~Distiller();
	Distiller(const Distiller&) = delete;
	Distiller(Distiller&&) = delete;
	Distiller& operator=(const Distiller&) = delete;
	Distiller& operator=(Distiller&&) = delete;

	static Distiller* getInstance();
	void tick();

	bool setMode(const DistillerMode value);
};

#endif //DISTILLER_H