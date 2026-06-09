#include "SkyBox.h"

SkyBox::SkyBox(Actor* parent_,
    const char* posX_, const char* negX_,
    const char* posY_, const char* negY_,
    const char* posZ_, const char* negZ_) 
    
    : Actor(parent_),
    posX(posX_), negX(negX_),
    posY(posY_), negY(negY_),
    posZ(posZ_), negZ(negZ_),
    textureID(0)
{
}

SkyBox::~SkyBox() {}

bool SkyBox::OnCreate() {
    // build mesh and shader
    Ref<MeshComponent>   mesh = GetComponent<MeshComponent>();
    Ref<ShaderComponent> shader = GetComponent<ShaderComponent>();
    if (!mesh->OnCreate())   return false;
    if (!shader->OnCreate()) return false;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    const char* faces[6] = { posX, negX, posY, negY, posZ, negZ };
    GLenum targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i = 0; i < 6; i++) {
        SDL_Surface* surface = IMG_Load(faces[i]);
        if (!surface) {
            std::cout << "Skybox: failed to load " << faces[i] << "\n";
            return false;
        }
        int mode = SDL_BYTESPERPIXEL(surface->format) == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(targets[i], 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_DestroySurface(surface);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return true;
}

void SkyBox::OnDestroy() {
    if (textureID) glDeleteTextures(1, &textureID);
}

void SkyBox::Render() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    GetComponent<MeshComponent>()->Render();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}