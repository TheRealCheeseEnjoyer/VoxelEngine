#include "Skybox.h"
#include "../include/stb_image.h"

#include <string>
#include <glad/glad.h>
#include <pstl/unseq_backend_simd.h>

float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox() {
    std::string faces[] = {
        "assets/skybox/yellowcloud_ft.jpg",
        "assets/skybox/yellowcloud_bk.jpg",
        "assets/skybox/yellowcloud_dn.jpg",
        "assets/skybox/yellowcloud_up.jpg",
        "assets/skybox/yellowcloud_rt.jpg",
        "assets/skybox/yellowcloud_lf.jpg",
    };
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenTextures(1, &skyboxTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        stbi_set_flip_vertically_on_load(true);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {

            fprintf(stderr, "Cubemap tex failed to load at path: %s\n", faces[i].c_str());
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



    skyboxShader = new Shader("shaders/skybox.vsh", "shaders/skybox.fsh");
    glBindVertexArray(0);
}

void Skybox::draw(const Camera& camera) const {
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(skyboxVAO);
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, camera.Position);
    skyboxShader->use();
    skyboxShader->setMat4("view", camera.GetViewMatrix());
    skyboxShader->setMat4("projection", camera.GetProjectionMatrix());
    skyboxShader->setMat4("model", model);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    glBindVertexArray(0);
}

Skybox::~Skybox() {
    delete skyboxShader;
    glDeleteTextures(1, &skyboxTextureId);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
}
