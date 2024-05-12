#include "file_wrapper.h"

static bool isMounted = false;

FileWrapper::FileWrapper(const String& path, const char* mode) 
	: _filename(path)
{
	if (!isMounted) {
		isMounted = SPIFFS.begin(false);
	}
	if (!isMounted) {
		Serial.println("MOUNT FS FAILED, TRY FORMAT!");
		isMounted = SPIFFS.begin(true);
	}
	if (!isMounted) {
		Serial.println("FORMAT FS FAILED!");
		return;
	}

	if (mode == FILE_READ && !SPIFFS.exists(_filename)) {
		return;
	}

	if (SPIFFS.exists(_filename)) {
		_file = SPIFFS.open(_filename, mode);
	}
	else {
		_file = SPIFFS.open(_filename, mode, true);
	}
	_isOpen = isMounted;
}

FileWrapper::~FileWrapper() {
	if (isOpen()) {
		_file.close();
	}
	if (isMounted) {
		SPIFFS.end();
		isMounted = false;
	}
}