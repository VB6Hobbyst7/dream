/*
 * SpriteInstance.cpp
 *
 * Created: 25/11/2016 2016 by Ashley
 *
 * Copyright 2016 Octronic. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 */
#include "SpriteInstance.h"
#include "TextureManager.h"

namespace Dream {

  SpriteInstance::SpriteInstance(
      AssetDefinition* definition,
      Transform3D* transform) : AssetInstance(definition,transform) {
    loadExtraAttributes(mDefinition->getJson());
    mGLBlend = false;
  }

  SpriteInstance::~SpriteInstance() {}

  bool SpriteInstance::load(string projectPath) {
    string path = projectPath+mDefinition->getAssetPath();
    string directory = path.substr(0, path.find_last_of('/'));
    cout << "SpriteInstance: Loading sprite from " << path << endl;
    mTexture = TextureManager::loadTextureFromFile("sprite",directory.c_str());
    return mTexture > 0;
  }

  void SpriteInstance::loadExtraAttributes(nlohmann::json jsonData) {
    mTileWidth = static_cast<int>(
      jsonData[SPRITE_TILE_SIZE][SPRITE_TILE_WIDTH]
    );
    mSpriteWidth = mTileWidth;

    mTileHeight = static_cast<int>(
      jsonData[SPRITE_TILE_SIZE][SPRITE_TILE_HEIGHT]
    );
    mSpriteHeight = mTileHeight;
  }

  void SpriteInstance::drawGL() {
  }

} // End Dream