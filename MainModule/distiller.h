#ifndef DISTILLER_H
#define DISTILLER_H

#include "settings.h"
#include "sensors.h"
#include <map> //From library ArduinoSTL
#include <memory>


class Distiller {
private:
	static Distiller* distiller;
	enum {
		IDLE = 0,
		HEATING,
		STABILIZATION,
		HEAD,
		BODY, 
		COMPLETION
	} _state;
	
	std::shared_ptr<DistillerInfo> _info;

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
	bool saveDataToJson();
};

#endif //DISTILLER_H