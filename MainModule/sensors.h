#ifndef SENSORS_H
#define SENSORS_H

#include "settings.h"
#include <vector> //From library ArduinoSTL
#include <time_manager.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <SFE_BMP180.h>


class OneWireAddress {
private: 
	uint8_t* _rawAddress = nullptr;
public:
	OneWireAddress();
	OneWireAddress(const uint8_t* address);
	OneWireAddress(const OneWireAddress& other);
	OneWireAddress(OneWireAddress&& other);
	~OneWireAddress();
	OneWireAddress& operator=(const OneWireAddress& other);
	OneWireAddress& operator=(OneWireAddress&& other);

	void setAddress(const uint8_t* address);
	const uint8_t* getRawAddress() const { return _rawAddress; }
	static uint8_t getAddressLength() { return 8; }
	SensorHash getHash() const;
	String asString() const;
};

class SensorsManager;
class DS18B20 {
private:
	void init();
	OneWireAddress _address;
	SensorInfo _info;
	float _rawTemperature = 0;
	TimeManager _timer;		// таймер опроса датчика

public:
	friend class SensorManager;

	DS18B20();
	~DS18B20() = default;
	explicit DS18B20(const OneWireAddress& addr);
	DS18B20(const DS18B20& other);
	DS18B20(DS18B20&& other);
	const DS18B20& operator=(const DS18B20& other);
	const DS18B20& operator=(DS18B20&& other);

	const OneWireAddress& getAddress() const { return _address; }
	SensorHash getHash() const { return _info._hash; }
	void setInfo(const SensorInfo& info);
	float getTemperature() const;
	void setRawTemperature(const float temp) { _rawTemperature = temp; }
	float getRawTemperature() const { return _rawTemperature; }
	const SensorInfo& getInfo() const { return _info; }
	void setName(const String& name);
	const String& getName() const;
	float getCorrection() const { return _info._correction; }
	void setCorrection(const float corr) { _info._correction = corr; }
	void setResolution(const DS_Resolution res);
	DS_Resolution getResolution() const { return _info._resolution; }
	void recalcHash();
	bool isTimerReady() const;
};

class AdvancedBmp : public BMP180 {
private:
	BmpInfo _info;

public:
	void setTemperCorrection(const float value) { _info._temperatureCorrection = value; }
	void setPressureCorrection(const float value) { _info._pressureCorrection = value; }
	void setInfo(const BmpInfo& info) { _info = info; }
	const BmpInfo& getInfo() const { return _info; }
	double getCorrectTemperature() const { return getTemperature() + _info._temperatureCorrection; }
	double getCorrectPressure() const { return getMmHg() + _info._pressureCorrection; }
};

class SensorsManager {
private:
	OneWire _wire;
	DallasTemperature _dsInterface;
	TimeManager _rescanBusTimer;
	std::vector<DS18B20> _dsSensors;
	bool _hasUnknownSensor = false;

	AdvancedBmp _bmp;

	void dsTick();

public:
	explicit SensorsManager();
	~SensorsManager();
	SensorsManager(const SensorsManager&) = delete;
	SensorsManager(SensorsManager&&) = delete;
	SensorsManager& operator=(const SensorsManager&) = delete;
	SensorsManager& operator=(SensorsManager&&) = delete;

	static SensorsManager* getInstance();

	void tick();
	void rescanOneWireBus();
	void setRescanTimer(const uint32_t millis_period);

	float getTemperature(const SensorHash hash) const;
	size_t getSensorsCount() const;
	bool saveSensorsDataToJson() const;

	const std::vector<DS18B20>& getSensors() const { return _dsSensors; }
	float getBmpCorrectTemperature() const { return static_cast<float>(_bmp.getCorrectTemperature()); }
	float getBmpCorrectPressure() const { return static_cast<float>(_bmp.getCorrectPressure()); }
	bool saveBmpDataToJson() const;
};

#endif // SENSORS_H
