#ifndef __FILESTREAMWINDOWS_H__
#define __FILESTREAMWINDOWS_H__

#include "../base/FileStreamBase.h"
#include <string>

class FileStreamWindows : public FileStreamBase {
public:
	explicit FileStreamWindows() { }
	~FileStreamWindows() { }

	string getText( string filePath ) const;
	Mesh* getMesh( string filePath ) const;
};
	
#endif // __FILESTREAMWINDOWS_H__