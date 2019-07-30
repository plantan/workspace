#ifndef __COREBASE_H__
#define __COREBASE_H__

#include <memory>

using namespace std;
class IRenderer;
class FileStreamBase;
class Model;

class CoreBase {
public:
	explicit CoreBase();
	virtual ~CoreBase() { };

	virtual bool init(const unsigned int width, const unsigned int height) = 0;
	virtual void update() = 0;
	virtual void render(Model* pScene, const bool clear) const = 0;
	virtual void debugLog(const char* message) = 0;

	virtual shared_ptr<IRenderer> getRenderer() const = 0;
	virtual shared_ptr<FileStreamBase> getFileStream() const = 0;
	bool shouldExit() const { return mExit; };

protected:
	bool mExit;
};

#endif // __COREBASE_H__
