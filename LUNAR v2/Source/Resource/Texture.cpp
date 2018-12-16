#include "../../Headers/Resource/Texture.h"
#include "../../Headers/Math/Types.h"

using namespace osomi;

Texture::~Texture(){ glDeleteTextures(1, &id); }
void Texture::bind(unsigned int targ) { glBindTexture(targ, id); }
void Texture::unbind(unsigned int targ) { glBindTexture(targ, 0); }