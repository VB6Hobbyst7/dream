/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "../../SharedAssetInstance.h"

using namespace std;
using namespace glm;

namespace Dream
{
    struct RGB;
    class ModelMesh;
    class ShaderInstance;
    class MaterialDefinition;
    class TextureInstance;
    class Camera;

    class MaterialInstance : public SharedAssetInstance
    {
    public:
        MaterialInstance(MaterialDefinition* def, ProjectRuntime* rt);
        ~MaterialInstance() override;

        void addMesh(ModelMesh* mesh);
        void clearMeshes();
        size_t countMeshes();

        bool operator==(MaterialInstance& other);
        void debug();
        void logMeshes();
        void drawGeometryPass(Camera*);
        void drawShadowPass(ShaderInstance* shader);

        bool load() override;

        TextureInstance* getDiffuseTexture() const;
        void setDiffuseTexture(TextureInstance* diffuseTexture);

        TextureInstance* getSpecularTexture() const;
        void setSpecularTexture(TextureInstance* specularTexture);

        TextureInstance* getNormalTexture() const;
        void setNormalTexture(TextureInstance* normalTexture);

        TextureInstance* getDisplacementTexture() const;
        void setDisplacementTexture(TextureInstance* displacementTexture);

        ShaderInstance* getShader() const;
        void setShader(ShaderInstance* shader);

        vec3 getColorDiffuse() const;
        void setColorDiffuse(vec3 colorDiffuse);

        vec3 getColorAmbient() const;
        void setColorAmbient(vec3 colorAmbient);

        vec3 getColorSpecular() const;
        void setColorSpecular(vec3 colorSpecular);

        vec3 getColorEmissive() const;
        void setColorEmissive(vec3 colorEmissive);

        vec3 getColorReflective() const;
        void setColorReflective(vec3 colorReflective);

        float getShininessStrength() const;
        void setShininessStrength(float shininessStrength);

        bool getIgnore() const;
        void setIgnore(bool ignore);

    protected:
        float mOpacity = 0.0f;
        float mBumpScaling = 0.0f;
        float mHardness = 0.0f;
        float mReflectivity = 0.0f;
        float mShininessStrength = 0.0f;
        float mRefracti = 0.0f;
        bool mIgnore = false;

        vec3 mColorDiffuse;
        vec3 mColorAmbient;
        vec3 mColorSpecular;
        vec3 mColorEmissive;
        vec3 mColorReflective;

        TextureInstance* mDiffuseTexture;
        TextureInstance* mSpecularTexture;
        TextureInstance* mNormalTexture;
        TextureInstance* mDisplacementTexture;
        ShaderInstance*  mShader;

        vector<ModelMesh*> mUsedBy;

        vec3 rgbToVec3(RGB color);
    };
}
