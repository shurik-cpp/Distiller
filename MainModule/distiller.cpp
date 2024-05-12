#include "distiller.h"

Distiller* distiller = nullptr;

Distiller::Distiller() {
	//TODO: поковыряться в Settings, вспомнить настройки
}

Distiller::~Distiller() {
	if (distiller) {
		delete distiller;
		distiller = nullptr;
	}
}

Distiller *Distiller::getInstance() {
	if (!distiller) {
		distiller = new Distiller();
	}
	return distiller;
}

void Distiller::tick() {
	if (_sensorsMap.size() < static_cast<size_t>(SensorLocation::MAX_LOCATION)) {
		//TODO: Эвент о не назначенных датчиках. Работа колонны в автоматическом режиме не возможна
		return;
	}

	switch (_mode) {
		case DistillerMode::AUTO:
			autoTick();
		break;
		case DistillerMode::SEMI_AUTO:
			semiAutoTick();
		break;
		case DistillerMode::MANUAL:
			manualTick();
		break;
		default:
		break;
	}
}

bool Distiller::setMode(const DistillerMode value) {
	switch (value) {
		case DistillerMode::AUTO:
			//TODO: Проверить можно ли включить авто-режим (имеются ли необходимые датчики на местах) 
			if (true) {
				_mode = value;
			}
		break;
		case DistillerMode::SEMI_AUTO:
			_mode = value;
		break;
		case DistillerMode::MANUAL:
			_mode = value;
		break;
		default:
		break;
	}
	return value == _mode;
}
