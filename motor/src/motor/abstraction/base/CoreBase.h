#ifndef __COREBASE_H__
#define __COREBASE_H__

#include "../../utils/List.h"
#include "../../core/GameObject.h"
#include "../../core/Scene.h"
#include <memory>

using namespace std;
class RendererBase;
class FileStreamBase;

class CoreBase {
public:
	explicit CoreBase();
	virtual ~CoreBase() { };

	virtual bool init( const unsigned int width, const unsigned int height ) = 0;
	virtual void update() = 0;
	virtual void render( Scene* pScene, const bool clear ) const = 0;
	virtual void debugLog( const char* message ) = 0;

	virtual shared_ptr<RendererBase> getRenderer() const = 0;
	virtual shared_ptr<FileStreamBase> getFileStream() const = 0;
	void addGameObject( GameObject* pGameObject ) { mGameObjects.add( pGameObject ); };
	bool shouldExit() const { return mExit; };

protected:
	List<GameObject> mGameObjects;
	bool mExit;
};

#endif // __COREBASE_H__
