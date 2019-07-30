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
    mShaderProgram = pCore->getRenderer()->createShaderProgram(pShaderSource);

    Mesh* pMesh = pCore->getFileStream()->getMesh("Box@n.mesh");
    int mapedMeshID = pCore->getRenderer()->mapMesh( pMesh, mShaderProgram );

    mpCamera = new Camera();
    mpCamera->init( 800, 600, 45.0f, 0.01f, 5000.0f );
    mpCamera->update();

    mpMaterial = new Material(mShaderProgram, pCore->getRenderer());

    mpModel = new Model(mapedMeshID, pMesh, mpMaterial);
    mpModel->mTransform.mPos.z = -300.0f;
    mpModel->mTransform.mPos.y = 0.0f;
}

App::~App() {
    DELETEPTR(mpCamera)
    DELETEPTR(mpMesh)
    DELETEPTR(mpModel)
    DELETEPTR(mpMaterial)
}

void App::run() {
    while( !mpCore->shouldExit() ) {
        mpModel->mTransform.mRot.y += 0.005f;

        mpCamera->update();
        mpModel->update(mpCamera);

        mpCore->update();
        mpCore->render(mpModel, true);
    }
}
