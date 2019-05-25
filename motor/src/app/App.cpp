#include "App.h"
#include<iostream>
#include<math.h>

#define MOVE_SPEED 0.1f
#define ROT_SPEED 0.02f

App::App( CoreBase* pCore ) :
mpCore(pCore),
mpCamera(0),
mpMesh(0) {
	string pShaderSource = pCore->getFileStream()->getText("shaders/default.dxs");
	mShaderProgram = pCore->getRenderer()->createShaderProgram( pShaderSource );

	Mesh* pMesh = pCore->getFileStream()->getMesh("Box@n.mesh");
	//pMesh->createCube();
	int mapedMeshID = pCore->getRenderer()->mapMesh( pMesh, mShaderProgram );

	mpCamera = new Camera();
	mpCamera->init( 800, 600, 45.0f, 0.01f, 5000.0f );
	mpCamera->update();

	mpMaterial = new Material( mShaderProgram, pCore->getRenderer() );

	mpModel = new Model( mapedMeshID, pMesh, mpMaterial );
	mpModel->mTransform.mPos.z = -300.0f;
	mpModel->mTransform.mPos.y = 0.0f;
	
	mScene.setCamera( mpCamera );
	mScene.addModel( mpModel );
	mScene.finalize();
}

App::~App() {
	DELETEPTR( mpCamera )
	DELETEPTR( mpMesh )
	DELETEPTR( mpModel )
	DELETEPTR( mpMaterial )
}

void App::run() {
	while( !mpCore->shouldExit() ) {
		//mpModel->mTransform.mRot.x += 0.005f;
		mpModel->mTransform.mRot.y += 0.005f;
		//mpModel->mTransform.mPos.x = sin(mpModel->mTransform.mRot.z * 0.01f);
		mpCamera->update();

		mScene.update();
		mpCore->update();
		mpCore->render( &mScene, true );
	}
}

//void App::updateFreelook() {
	/*
	GLFWwindow* pWindow = Renderer->getGLFWWindow();

    const float oldX = (float)mMouseX;
    const float oldY = (float)mMouseY;
	glfwGetCursorPos( pWindow, &mMouseX, &mMouseY );
    const float diffX = (float)(mMouseX - oldX);
    const float diffY = (float)(mMouseY - oldY);

    mRotX += (float)diffY * ROT_SPEED;
    mRotY += (float)diffX * ROT_SPEED;

    Mat4 r;
    r.identity();
    r.rotY( -mRotY );
    r.rotX( -mRotX );

    Vec3 forward( 0.0f, 0.0f, 1.0f );
    Vec3 left( 1.0f, 0.0f, 0.0f );
    r.transformVector( forward );
    r.transformVector( left );
    forward.normalize();
    left.normalize();

	Camera* pCamera = Renderer->getActiveCamera();
	
    pCamera->mPos += forward * MOVE_SPEED * (float)glfwGetKey( pWindow, GLFW_KEY_W );
    pCamera->mPos -= forward * MOVE_SPEED * (float)glfwGetKey( pWindow, GLFW_KEY_S );
    pCamera->mPos += left * MOVE_SPEED * (float)glfwGetKey( pWindow, GLFW_KEY_A );
    pCamera->mPos -= left * MOVE_SPEED * (float)glfwGetKey( pWindow, GLFW_KEY_D );

    Mat4 t;
    t.identity();
    t.translate( pCamera->mPos );

	pCamera->setView( r * t );
	pCamera->setViewProj( pCamera->getProj() * pCamera->getView() );
	*/
//}
