#include "sensors.h"


OneWireAddress::OneWireAddress() {
	uint8_t _rawAddress[OneWireAddress::getAddressLength()];
	memset(_rawAddress, 0xFF, OneWireAddress::getAddressLength());
	setAddress(_rawAddress);
}

OneWireAddress::OneWireAddress(const uint8_t *rawAddress) {
	setAddress(rawAddress);
}

OneWireAddress::OneWireAddress(const OneWireAddress &other) {
	setAddress(other._rawAddress);
}

OneWireAddress::OneWireAddress(OneWireAddress &&other) {
	_rawAddress = other._rawAddress;
	other._rawAddress = nullptr;
}

OneWireAddress::~OneWireAddress() {
	if (_rawAddress)
		delete[] _rawAddress;
	_rawAddress = nullptr;
}

OneWireAddress &OneWireAddress::operator=(const OneWireAddress &other) {
    if (this != &other) {
		setAddress(other._rawAddress);
	}
	return *this;
}

OneWireAddress &OneWireAddress::operator=(OneWireAddress &&other) {
    if (this != &other) {
		_rawAddress = other._rawAddress;
		other._rawAddress = nullptr;
	}
	return *this;
}

void OneWireAddress::setAddress(const uint8_t *rawAddress) {
	if (!_rawAddress) {
		_rawAddress = new uint8_t[OneWireAddress::getAddressLength()];
	}
	memcpy(_rawAddress, rawAddress, OneWireAddress::getAddressLength());
}

SensorHash OneWireAddress::getHash() const {
    SensorHash hash = 0;
	const uint8_t length = getAddressLength();
	for (uint8_t i = 0, j = length -1; i < length && j >= 0; ++i, --j) {
		hash += _rawAddress[i] + _rawAddress[j];
	}
	return hash;
}

String OneWireAddress::asString() const {
    String addr_str;
	for (uint8_t i = 0; i < OneWireAddress::getAddressLength(); ++i) {
		// делаем адрес в строковом представлении
		addr_str += _rawAddress[i] < 16 ? "00" : String(_rawAddress[i], HEX);
	}
	return addr_str;
}

//===============================================================================

DS18B20::DS18B20() {
	init();
}

DS18B20::DS18B20(const OneWireAddress &addr)
    : _address(addr)
{
	init();
}

DS18B20::DS18B20(const DS18B20 &other)
	: _address(other._address)
	, _info(other._info)
	, _rawTemperature(other._rawTemperature)
	, _timer(other._timer)
{ }

DS18B20::DS18B20(DS18B20 &&other)
	: _address(other._address)
	, _info(other._info)
	, _rawTemperature(other._rawTemperature)
	, _timer(other._timer)
{ }

const DS18B20 &DS18B20::operator=(const DS18B20 &other) {
	_address = other._address;
	// _info = other._info;
	_rawTemperature = other._rawTemperature;
	_timer = other._timer;
	return *this;
}

const DS18B20 &DS18B20::operator=(DS18B20 &&other) {
	_address = std::move(other._address);
	// _info = std::move(other._info);
	_rawTemperature = std::move(other._rawTemperature);
	_timer = std::move(other._timer);
	return *this;
}

void DS18B20::init() {
	setName("");
	_info._hash = _address.getHash();
	setResolution(DS_Resolution::TEMP_12_BIT);
}

void DS18B20::setInfo(const SensorInfo &info) {
	_info = info;
}

float DS18B20::getTemperature() const {
    return _rawTemperature + getCorrection();
}

void DS18B20::setName(const String &name) {
	_info._name = name.isEmpty() ? _address.asString() : name;
}

const String& DS18B20::getName() const { 
	return _info._name; 
}

void DS18B20::setResolution(const DS_Resolution res) {
	_info._resolution = res;
	switch (res) {
		case DS_Resolution::TEMP_9_BIT:
			_timer.SetPeriod(94); // магические числа взяты из даташита
		break;
		case DS_Resolution::TEMP_10_BIT:
			_timer.SetPeriod(188);
		break;
		case DS_Resolution::TEMP_11_BIT:
			_timer.SetPeriod(375);
		break;
		case DS_Resolution::TEMP_12_BIT:
			_timer.SetPeriod(750);
		break;
	}
}

void DS18B20::recalcHash() {
    _info._hash = _address.getHash();
}

bool DS18B20::isTimerReady() const { 
	return _timer.IsReady(); 
}

//==============================================================================

SensorsManager* sensorsManager = nullptr;

SensorsManager::SensorsManager()
	: _wire(PinsDefine::ONE_WIRE_BUS)
{
	_dsInterface.setOneWire(&_wire);
	_dsInterface.setWaitForConversion(false);

	_rescanBusTimer.SetPeriod(Time::SEC_5);
	_bmp.begin();
	_bmp.setInfo(Settings::getInstance()->getBmpInfo());
	rescanOneWireBus();
}

SensorsManager::~SensorsManager() {
	if (sensorsManager) {
		delete sensorsManager;
		sensorsManager = nullptr;
	}
}

SensorsManager* SensorsManager::getInstance() {
	if (!sensorsManager) {
		sensorsManager = new SensorsManager();
	}
	return sensorsManager;
}

void SensorsManager::rescanOneWireBus() {
	_dsInterface.begin();
	const uint8_t count_device_on_wire = _dsInterface.getDeviceCount();
#ifdef DEBUG
	//Serial.printf("SensorsManager::count_device_on_wire is %u \n", count_device_on_wire);
#endif //DEBUG
	std::vector<DS18B20> rescan_data; 
	const auto* settings = Settings::getInstance();
	for (size_t i = 0; i < count_device_on_wire; ++i) {
		uint8_t rawAddress[OneWireAddress::getAddressLength()];
		_dsInterface.getAddress(rawAddress, i);
		const OneWireAddress address(rawAddress);
		DS18B20 sensor(address);
#ifdef DEBUG
		// Serial.printf("address: %s, CRC: %u \n", sensor._info._name.c_str(), sensor._info._hash);
#endif //DEBUG
		// вспоминаем настройки из json для подключенных датчиков.
		if (const SensorInfo* info = settings->getSensorInfoAtHash(sensor.getHash())) {
			sensor.setInfo(*info);
			for (const auto& lastSensorData : _dsSensors) {
				if (sensor.getHash() == lastSensorData.getHash()) {
					sensor = lastSensorData;
				}
			}
		}
		else {
			//TODO: Создать event о неизвестных датчиках на шине?
			_hasUnknownSensor = true;
		}
		rescan_data.push_back(sensor);
	}
	_dsSensors = std::move(rescan_data);
}

void SensorsManager::setRescanTimer(const uint32_t millis_period) {
	_rescanBusTimer.SetPeriod(millis_period);
}

void SensorsManager::tick() {
	if (!_bmp.isInitialized()) {
		Serial.println("WARNING! Pressure sensor is not initialized. Try again...");
		_bmp.begin();
	}
	_bmp.tick();
	if (_bmp.getError()) {
		//TODO: Создать event о лаже BMP?
	}
	dsTick();
}

size_t SensorsManager::getSensorsCount() const {
    return _dsSensors.size();
}

bool SensorsManager::saveSensorsDataToJson() const {
	std::vector<SensorInfo> infos;
	for (const auto& it : _dsSensors) {
		infos.push_back(it.getInfo());
	}
	return Settings::getInstance()->saveSensorsInfo(infos);
}

bool SensorsManager::saveBmpDataToJson() const {
    return Settings::getInstance()->saveBmpInfo(_bmp.getInfo());
}

float SensorsManager::getTemperature(const SensorHash hash) const {
	for (const auto& sensor : _dsSensors) {
		if (sensor.getHash() == hash) {
			return sensor.getTemperature();
		}
	}
	return DEVICE_DISCONNECTED_C;
}

void SensorsManager::dsTick() {
	std::vector<DS18B20*> lostSensors;
	for (auto& it : _dsSensors) {
		if (it.isTimerReady()) {
			const float temp = _dsInterface.getTempC(it.getAddress().getRawAddress());
			if (_dsInterface.requestTemperaturesByAddress(it.getAddress().getRawAddress())
				&& static_cast<int>(temp) != DEVICE_DISCONNECTED_C) 
			{
				it.setRawTemperature(temp);
			}
			else {
				lostSensors.push_back(&it);
			}
		}
	}
#ifdef DEBUG
	if (!lostSensors.empty()) {
		for (const auto* it : lostSensors) {
			//TODO: Создать event о потерянном датчике
			Serial.printf("lost: %s\n", it->getInfo()._name.c_str());
		}
	}
#endif //DEBUG
	if (_rescanBusTimer.IsReady() || !lostSensors.empty()) {
		rescanOneWireBus();
		if (_hasUnknownSensor) {
			_hasUnknownSensor = !saveSensorsDataToJson();
		}
	}
}