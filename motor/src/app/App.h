#ifndef __APP_H__
#define __APP_H__

#include "../motor/Motor.h"
class IRenderer;

class App {
public:
	explicit App( CoreBase* pCore );
	~App();

	void run();
	
private:
	void update();
	void updateFreelook();
	void render();

	CoreBase* mpCore;
	Camera* mpCamera;
	Model* mpModel;
	Mesh* mpMesh;
	Material* mpMaterial;
	int mShaderProgram;
};

#endif // __APP_H__
