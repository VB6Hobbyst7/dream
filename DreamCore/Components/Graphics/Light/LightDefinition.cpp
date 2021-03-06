/*
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.y()nu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 */

#include "LightDefinition.h"


namespace Dream
{
    LightDefinition::LightDefinition
    (ProjectDefinition* pd, const json &js)
        : AssetDefinition(pd,js)
    {
        #ifdef DREAM_LOG
        setLogClassName("LightDefinition");
        getLog()->trace("Constructing");
        #endif
    }

    LightDefinition::~LightDefinition()
    {
        #ifdef DREAM_LOG
        getLog()->trace("Destructing");
        #endif
    }

    void
    LightDefinition::setDiffuse
    (const Vector3& diffuse)
    {
        if (mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE] = json::object();
        }

        mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::RED]   = diffuse.x();
        mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::GREEN] = diffuse.y();
        mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::BLUE]  = diffuse.z();
    }

    Vector3
    LightDefinition::getDiffuse
    ()
    {
        Vector3 diffuse;
        if (mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE]=json::object();
            mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::RED] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::GREEN] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::BLUE] = 0.0f;
        }
        diffuse.setX(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::RED]));
        diffuse.setY(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::GREEN]));
        diffuse.setZ(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_DIFFUSE][Constants::BLUE]));
        return diffuse;
    }

    void
    LightDefinition::setAmbient
    (const Vector3& ambient)
    {
        if (mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT] = json::object();
        }
        mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::RED]   = ambient.x();
        mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::GREEN] = ambient.y();
        mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::BLUE]  = ambient.z();
    }

    Vector3
    LightDefinition::getAmbient
    ()
    {
        Vector3 ambient;
        if (mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT]=json::object();
            mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::RED] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::GREEN] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::BLUE] = 0.0f;
        }
        ambient.setX(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::RED]));
        ambient.setY(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::GREEN]));
        ambient.setZ(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_AMBIENT][Constants::BLUE]));
        return ambient;
    }

    void
    LightDefinition::setSpecular
    (const Vector3& specular)
    {
        if (mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR] = json::object();
        }
        mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::RED]   = specular.x();
        mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::GREEN] = specular.y();
        mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::BLUE]  = specular.z();
    }

    Vector3
    LightDefinition::getSpecular()
    {
        Vector3 specular;
        if (mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR]=json::object();
            mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::RED] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::GREEN] = 0.0f;
            mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::BLUE] = 0.0f;
        }
        specular.setX(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::RED]));
        specular.setY(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::GREEN]));
        specular.setZ(static_cast<float>(mJson[Constants::ASSET_ATTR_LIGHT_SPECULAR][Constants::BLUE]));
        return specular;
    }

    void LightDefinition::setType(LightType type)
    {
        string typeStr;
        switch (type)
        {
           case LT_DIRECTIONAL:
                typeStr = Constants::ASSET_FORMAT_LIGHT_DIRECTIONAL;
                break;
           case LT_POINT:
                typeStr = Constants::ASSET_FORMAT_LIGHT_POINT;
                break;
           case LT_SPOTLIGHT:
                typeStr = Constants::ASSET_FORMAT_LIGHT_SPOTLIGHT;
                break;
            case LT_NONE:
                typeStr = "";
        }
        setFormat(typeStr);
    }

    LightType
    LightDefinition::getType
    ()
    {
        LightType type;
        if (getFormat() == Constants::ASSET_FORMAT_LIGHT_POINT)
        {
            type = LT_POINT;
        }
        else if (getFormat() == Constants::ASSET_FORMAT_LIGHT_DIRECTIONAL)
        {
            type = LT_DIRECTIONAL;
        }
        else if (getFormat() == Constants::ASSET_FORMAT_LIGHT_SPOTLIGHT)
        {
            type = LT_SPOTLIGHT;
        }
        else
        {
            type = LT_NONE;
        }
        return type;
    }

    void LightDefinition::setCutOff(float cutOff)
    {
        mJson[Constants::ASSET_ATTR_LIGHT_CUTOFF] = cutOff;
    }

    float LightDefinition::getCutOff()
    {
        if(mJson[Constants::ASSET_ATTR_LIGHT_CUTOFF].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_CUTOFF] = 0.0f;
        }
        return mJson[Constants::ASSET_ATTR_LIGHT_CUTOFF];
    }

    void LightDefinition::setOuterCutOff(float outerCutOff)
    {
        mJson[Constants::ASSET_ATTR_LIGHT_OUTER_CUTOFF] = outerCutOff;
    }

    float LightDefinition::getOuterCutOff()
    {
        if(mJson[Constants::ASSET_ATTR_LIGHT_OUTER_CUTOFF].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_OUTER_CUTOFF] = 0.0f;
        }
        return mJson[Constants::ASSET_ATTR_LIGHT_OUTER_CUTOFF];
    }

    void LightDefinition::setConstant(float ant)
    {
        mJson[Constants::ASSET_ATTR_LIGHT_CONSTANT] = ant;
    }

    float LightDefinition::getConstant()
    {
        if(mJson[Constants::ASSET_ATTR_LIGHT_CONSTANT].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_CONSTANT] = 0.0f;
        }
        return mJson[Constants::ASSET_ATTR_LIGHT_CONSTANT];
    }

    void LightDefinition::setLinear(float linear)
    {
        mJson[Constants::ASSET_ATTR_LIGHT_LINEAR] = linear;
    }

    float LightDefinition::getLinear()
    {
        if(mJson[Constants::ASSET_ATTR_LIGHT_LINEAR].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_LINEAR] = 0.0f;
        }
        return mJson[Constants::ASSET_ATTR_LIGHT_LINEAR];
    }

    void LightDefinition::setQuadratic(float quadratic)
    {
        mJson[Constants::ASSET_ATTR_LIGHT_QUADRATIC] = quadratic;
    }

    float LightDefinition::getQuadratic()
    {
        if(mJson[Constants::ASSET_ATTR_LIGHT_QUADRATIC].is_null())
        {
            mJson[Constants::ASSET_ATTR_LIGHT_QUADRATIC] = 0.0f;
        }
        return mJson[Constants::ASSET_ATTR_LIGHT_QUADRATIC];
    }

    float LightDefinition::getDiffuseBlue()
    {
        return getDiffuse().z();
    }

    void LightDefinition::setDiffuseBlue(float diffuseBlue)
    {
        auto current = getDiffuse();
        current.setZ(diffuseBlue);
        setDiffuse(current);
    }

    float LightDefinition::getDiffuseGreen()
    {
        return getDiffuse().y();
    }

    void LightDefinition::setDiffuseGreen(float diffuseGreen)
    {
        auto current = getDiffuse();
        current.setY(diffuseGreen);
        setDiffuse(current);
    }

    float LightDefinition::getDiffuseRed()
    {
        return getDiffuse().x();
    }

    void LightDefinition::setDiffuseRed(float diffuseRed)
    {
        auto current = getDiffuse();
        current.setX(diffuseRed);
        setDiffuse(current);
    }

    float LightDefinition::getAmbientBlue()
    {
        return getAmbient().z();
    }

    void LightDefinition::setAmbientBlue(float blue)
    {
        auto current = getAmbient();
        current.setZ(blue);
        setAmbient(current);
    }

    float LightDefinition::getAmbientGreen()
    {
        return getAmbient().y();
    }

    void LightDefinition::setAmbientGreen(float ambientGreen)
    {
        auto current = getAmbient();
        current.setY(ambientGreen);
        setAmbient(current);
    }

    float LightDefinition::getAmbientRed()
    {
        return getAmbient().x();
    }

    void LightDefinition::setAmbientRed(float ambientRed)
    {
        auto current = getAmbient();
        current.setX(ambientRed);
        setAmbient(current);
    }

    float LightDefinition::getSpecularBlue()
    {
        return getSpecular().z();
    }

    void LightDefinition::setSpecularBlue(float specularBlue)
    {
        auto current = getSpecular();
        current.setZ(specularBlue);
        setSpecular(current);
    }

    float LightDefinition::getSpecularGreen()
    {
        return getSpecular().y();
    }

    void LightDefinition::setSpecularGreen(float specularGreen)
    {
        auto current = getSpecular();
        current.setY(specularGreen);
        setSpecular(current);
    }

    float LightDefinition::getSpecularRed()
    {
        return getSpecular().x();
    }

    void LightDefinition::setSpecularRed(float specularRed)
    {
        auto current = getSpecular();
        current.setX(specularRed);
        setSpecular(current);
    }
}
