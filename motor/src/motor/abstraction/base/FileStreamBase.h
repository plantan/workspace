#ifndef __FILESTREAMBASE_H__
#define __FILESTREAMBASE_H__

#include <string>
using namespace std;
class Mesh;

class FileStreamBase {
public:
	explicit FileStreamBase() {}
	virtual ~FileStreamBase() {}

	virtual string getText( string filePath ) const = 0;
	virtual Mesh* getMesh( string filePath ) const = 0;
};

#endif // __FILESTREAMBASE_H__
