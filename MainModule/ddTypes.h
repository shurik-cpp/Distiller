#ifndef DD_TYPES_H
#define DD_TYPES_H
#include <map>

enum class DS_Resolution : uint8_t {
	TEMP_9_BIT = 9,
	TEMP_10_BIT,
	TEMP_11_BIT,
	TEMP_12_BIT
};

typedef uint16_t SensorHash;
// Для хранения в ПЗУ
struct SensorInfo {
	SensorHash _hash = 0; // контрольная сумма адреса датчика
	DS_Resolution _resolution = DS_Resolution::TEMP_12_BIT;
	String _name;
	float _correction = 0; // коэффициент поправки датчика
};

struct BmpInfo {
	float _temperatureCorrection = 0;
	float _pressureCorrection = 0;
};


enum class DistillerMode : uint8_t {
	MANUAL = 0,
	AUTO,
	SEMI_AUTO	
};

enum class SensorLocation : uint8_t {
	TOP_COLUMN,
	CENTER_COLUMN,
	CUBE,
	TOP_REFLUX,
	BOTTOM_REFLUX,
	COOLANT_REFLUX,
	MAX_LOCATION
};

struct DistillerInfo {
	DistillerMode _mode = DistillerMode::AUTO;
	std::map<SensorLocation, SensorHash> _sensorsMap;
};



#endif //DD_TYPES_H