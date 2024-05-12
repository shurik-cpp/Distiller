#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <FS.h>
#include <SPIFFS.h>

class FileWrapper {
private:
	const String _filename;
	File _file;
	bool _isOpen = false;
public:
	FileWrapper() = delete;
	explicit FileWrapper(const String& path, const char* mode = FILE_READ);
	~FileWrapper();
	bool isOpen() const { return _isOpen; }
	File& get() { return _file; }
};

#endif //FILE_WRAPPER_H