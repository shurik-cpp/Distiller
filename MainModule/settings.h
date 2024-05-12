#ifndef SETTINGS_H
#define SETTINGS_H

#define DEBUG

enum PinsDefine {
	ONE_WIRE_BUS = 32
};

const int SENSOR_ACCURACY = 1000;

// для дисплея GMG12864-06D Ver.2.2 библиотека <U8g2lib.h>:
// U8G2_ST7565_ERC12864_ALT_F_4W_SW_SPI u8g2(U8G2_R0, /* clock */ 8, /* data */ 9,/* cs */ 5,/* reset */ 6)
// https://erlon.ru/%D0%B4%D0%B8%D1%81%D0%BF%D0%BB%D0%B5%D0%B9-gmg12864-06d-ver-2-2/
// https://github.com/olikraus/u8g2/wiki/

#include <Arduino.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <ArduinoJson.h>
#include "ddTypes.h"


class Settings {
private:
	const String _filename = "/settings.json";
	static Settings* _setup;
	std::map<SensorHash, SensorInfo> _dsSensors;
	std::map<SensorHash, SensorInfo> getSensorsInfoFromJson(const JsonDocument& json) const;
	BmpInfo _bmpInfo;
	DistillerInfo _distillerInfo;
	bool initFromJson();
	JsonDocument getJsonFromFile() const;
	bool saveJsonToFile(const JsonDocument& json);

public:
	explicit Settings();
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;
	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;
	~Settings();

	static Settings* const getInstance();

	const SensorInfo* getSensorInfoAtHash(const SensorHash hash) const noexcept;
	bool saveSensorsInfo(const std::vector<SensorInfo>& sensorsInfo);

	const BmpInfo& getBmpInfo() const { return _bmpInfo; }
	bool saveBmpInfo(const BmpInfo& bmpInfo);
	const DistillerInfo& getDistillerInfo() const { return _distillerInfo; }
	bool saveDistillerInfo(const DistillerInfo& distillerInfo);
};

#endif  // SETTINGS_H