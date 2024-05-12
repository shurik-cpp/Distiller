#include "settings.h"
#include "file_wrapper.h"

Settings* Settings::_setup = nullptr;

const String jsonStr = "{"
			"\"ds18b20\":["
				"{\"hash\":946,\"resolution\":11,\"name\":\"TOP\",\"correction\":-0.34},"
				"{\"hash\":1612,\"resolution\":9,\"name\":\"MIDDLE\",\"correction\":0.27}"
			"],"
			"\"bmp\":{"
				"\"t_correction\":0.15,\"p_correction\":0.32"
			"}"
		"}";

static String DistillerModeToStr(const DistillerMode& mode) {
	switch (mode) {
		case DistillerMode::AUTO:
			return "AUTO";
		break;
		case DistillerMode::SEMI_AUTO:
			return "SEMI_AUTO";
		break;
		case DistillerMode::MANUAL:
			return "MANUAL";
		break;
	}
	return "DEFAULT";
}

static DistillerMode StrToDistillerMode(const String& mode) {
	if (mode == "AUTO") 
		return DistillerMode::AUTO;
	else if (mode == "SEMI_AUTO") 
		return DistillerMode::SEMI_AUTO;
	else if (mode == "MANUAL") 
		return DistillerMode::MANUAL;

	return DistillerMode::AUTO; // DEFAULT
}

std::map<SensorHash, SensorInfo> Settings::getSensorsInfoFromJson(const JsonDocument& json) const {
	std::map<SensorHash, SensorInfo> result;
	size_t i = 0;
	const auto& sensors = json["ds18b20"];
	while (sensors[i]) {
		SensorInfo info;
		info._hash = sensors[i]["hash"];
		info._resolution = static_cast<DS_Resolution>(static_cast<int>(sensors[i]["resolution"]));
		info._name = String(static_cast<const char*>(sensors[i]["name"]));
		info._correction = sensors[i]["correction"];
		result.insert({info._hash, std::move(info)});
		++i;
	}
	return std::move(result);
}

bool Settings::initFromJson() {

	JsonDocument json = getJsonFromFile();

	if (json.containsKey("ds18b20")) {
		_dsSensors = getSensorsInfoFromJson(json);
#ifdef DEBUG
		Serial.printf("====== from json ======\n");
		for (const auto& it : _dsSensors) {
			Serial.printf("hash: %u\nres: %u\nname: %s\ncorr: %.3f\n----\n", it.first, it.second._resolution, it.second._name.c_str(), it.second._correction);
		}
		Serial.printf("=======================\n");
#endif //DEBUG
	}
	if (json.containsKey("bmp")) {
		_bmpInfo._temperatureCorrection = json["bmp"]["t_correction"];
		_bmpInfo._pressureCorrection = json["bmp"]["p_correction"];
	}
	if (json.containsKey("distiller")) {
		const char* strMode = json["distiller"]["mode"];
		_distillerInfo._mode = StrToDistillerMode(String(strMode));
	}
	return true;
}

JsonDocument Settings::getJsonFromFile() const {
	JsonDocument json;
	FileWrapper file(_filename, FILE_READ);
	if (file.isOpen()) {
		char buff[file.get().size() + 1];
		file.get().readBytes(buff, file.get().size());
#ifdef DEBUG
		Serial.printf("Data from file \"%s\":\n%s\n", _filename.c_str(), buff);
#endif //DEBUG
		// const DeserializationError error = deserializeJson(json, jsonStr);
		const DeserializationError error = deserializeJson(json, buff);
#ifdef DEBUG
		Serial.printf("deserializeJson(): %s\n", error.f_str());
#endif //DEBUG
	}
    return std::move(json);
}

bool Settings::saveJsonToFile(const JsonDocument &json) {
    FileWrapper file(_filename, FILE_WRITE);
	if (!file.isOpen()) {
		Serial.println("WRITE TO FILE FAILED!");
		return false;
	}
	String serializedData;
	serializeJson(json, serializedData);
	file.get().print(serializedData);
	Serial.println(serializedData);
	return true;
}

Settings::Settings() {
	initFromJson();
}

Settings::~Settings() {
	if (_setup)
		delete _setup;
	_setup = nullptr;
}

Settings *const Settings::getInstance() {
	if (!_setup)
		_setup = new Settings();
	return _setup;
}

const SensorInfo* Settings::getSensorInfoAtHash(const SensorHash hash) const noexcept {
	if (_dsSensors.count(hash))
		return &_dsSensors.at(hash);
	return nullptr;
}

bool Settings::saveSensorsInfo(const std::vector<SensorInfo>& sensorsInfo) {
	for (const auto& it : sensorsInfo) {
		_dsSensors[it._hash] = it;
	}
	JsonDocument json = getJsonFromFile();

	if (!json.containsKey("ds18b20")) {
		Serial.println("Create new json value \"ds18b20\"");
		json.add("ds18b20");
	}
	JsonArray sensors = json["ds18b20"].to<JsonArray>();
	for (const auto& it : _dsSensors) {
		JsonObject item = sensors.add<JsonObject>();
		item["hash"] = it.first;
		item["resolution"] = static_cast<int>(it.second._resolution);
		item["name"] = it.second._name.c_str();
		item["correction"] = it.second._correction;
	}
	return saveJsonToFile(json);
}

bool Settings::saveBmpInfo(const BmpInfo& bmpInfo) {
	_bmpInfo = bmpInfo;

	JsonDocument json = getJsonFromFile();

	if (!json.containsKey("bmp")) {
		Serial.println("Create new json value \"bmp\"");
		json.add("bmp");
		json["bmp"].add("t_correction");
		json["bmp"].add("p_correction");
	}
	auto bmpJson = json["bmp"].to<JsonObject>();
	bmpJson["t_correction"] = _bmpInfo._temperatureCorrection;
	bmpJson["p_correction"] = _bmpInfo._pressureCorrection;

	return saveJsonToFile(json);
}

bool Settings::saveDistillerInfo(const DistillerInfo& distillerInfo) {
	_distillerInfo = distillerInfo;

	JsonDocument json = getJsonFromFile();

	if (!json.containsKey("distiller")) {
		Serial.println("Create new json value \"distiller\"");
		json.add("distiller");
		json["distiller"].add("mode");
	}
	auto distillerJson = json["distiller"].to<JsonObject>();
	distillerJson["mode"] = DistillerModeToStr(_distillerInfo._mode).c_str();

	return saveJsonToFile(json);
}
