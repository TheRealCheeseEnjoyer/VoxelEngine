#ifndef SKYBOX_H
#define SKYBOX_H
#include "../include/Shader.h"
#include "../include/Camera.h"

class Skybox {
    unsigned int skyboxTextureId;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;

    Shader* skyboxShader = nullptr;
public:
    Skybox();
    void draw(const Camera& camera) const;
    ~Skybox();
};

#endif //SKYBOX_H
