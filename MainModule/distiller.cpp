#include "distiller.h"

Distiller* Distiller::distiller = nullptr;

Distiller::Distiller() {
    //TODO: поковыряться в Settings, вспомнить настройки
	_info = Settings::getInstance()->getDistillerInfo();
}

Distiller::~Distiller() {
	if (distiller) {
		delete distiller;
		distiller = nullptr;
	}
}

Distiller* Distiller::getInstance() {
	if (!distiller) {
		distiller = new Distiller();
	}
	return distiller;
}

void Distiller::tick() {
	if (_info->_sensorsMap.size() < static_cast<size_t>(SensorLocation::MAX_LOCATION)) {
		//TODO: Эвент о не назначенных датчиках. Работа колонны в автоматическом режиме не возможна
		return;
	}

	switch (_info->_mode) {
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

void Distiller::autoTick() {
}

void Distiller::semiAutoTick() {
}

void Distiller::manualTick() {
}

bool Distiller::setMode(const DistillerMode value) {
	DistillerMode& mode = _info->_mode;
	switch (value) {
		case DistillerMode::AUTO:
			//TODO: Проверить можно ли включить авто-режим (имеются ли необходимые датчики на местах) 
			if (true) {
				mode = value;
			}
		break;
		case DistillerMode::SEMI_AUTO:
			mode = value;
		break;
		case DistillerMode::MANUAL:
			mode = value;
		break;
		default:
		break;
	}
	return value == mode;
}

bool Distiller::saveDataToJson() {
    return Settings::getInstance()->saveDistillerInfo();
}
