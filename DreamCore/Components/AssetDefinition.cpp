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

#include <string>
#include  <sstream>
#include "AssetDefinition.h"
#include <regex>
#include "../Project/ProjectDefinition.h"

namespace Dream
{
    AssetDefinition::AssetDefinition
    (ProjectDefinition* parent, const json &jsonDef)
        : Definition(jsonDef),
          mProjectDefinition(parent)
    {
        #ifdef DREAM_LOG
        setLogClassName("AssetDefinition");
        getLog()->trace("Constructing {}", getNameAndUuidString());
        #endif
    }

    AssetDefinition::~AssetDefinition
    ()
    {

        #ifdef DREAM_LOG
        getLog()->trace("Destructing {}", getNameAndUuidString());
        #endif
    }

    AssetType
    AssetDefinition::getAssetType
    ()
    {
        return Constants::getAssetTypeEnumFromString(mJson[Constants::ASSET_TYPE]);
    }

    void
    AssetDefinition::setType
    (string type)
    {
        mJson[Constants::ASSET_TYPE] = type;
    }

    string
    AssetDefinition::getType
    ()
    {
        if (mJson[Constants::ASSET_TYPE].is_null())
        {
            mJson[Constants::ASSET_TYPE] = "";
        }
        return mJson[Constants::ASSET_TYPE];
    }

    void
    AssetDefinition::setFormat
    (string format)
    {
        mJson[Constants::ASSET_FORMAT] = format;
    }

    string
    AssetDefinition::getFormat
    ()
    {
        if (mJson[Constants::ASSET_FORMAT].is_null())
        {
            mJson[Constants::ASSET_FORMAT] = "";
        }

        return mJson[Constants::ASSET_FORMAT];
    }

    bool
    AssetDefinition::isTypeAnimation
    ()
    {
        return getType() == Constants::ASSET_TYPE_ANIMATION;
    }

    bool
    AssetDefinition::isTypeLight
    ()
    {
        return getType() == Constants::ASSET_TYPE_LIGHT;
    }

    bool AssetDefinition::isTypeMaterial()
    {
       return getType() == Constants::ASSET_TYPE_MATERIAL;
    }

    bool
    AssetDefinition::isTypeFont
    ()
    {
        return getType() == Constants::ASSET_TYPE_FONT;
    }

    bool
    AssetDefinition::isTypePhysicsObject
    ()
    {
        return getType() == Constants::ASSET_TYPE_PHYSICS_OBJECT;
    }

    bool
    AssetDefinition::isTypeTexture
    ()
    {
        return getType() == Constants::ASSET_TYPE_TEXTURE;
    }

    bool
    AssetDefinition::isTypePath
    ()
    {
        return getType() == Constants::ASSET_TYPE_PATH;
    }

    bool
    AssetDefinition::isTypeParticleEmitter
    ()
    {
       return getType() == Constants::ASSET_TYPE_PARTICLE_EMITTER;
    }

    bool
    AssetDefinition::isTypeAudio
    ()
    {
        return getType() == Constants::ASSET_TYPE_AUDIO;
    }

    bool
    AssetDefinition::isTypeModel
    ()
    {
        return getType() == Constants::ASSET_TYPE_MODEL;
    }

    bool
    AssetDefinition::isTypeScript
    ()
    {
        return getType() == Constants::ASSET_TYPE_SCRIPT;
    }

    bool
    AssetDefinition::isTypeShader
    ()
    {
        return getType() == Constants::ASSET_TYPE_SHADER;
    }

    string
    AssetDefinition::getGroup
    ()
    {
        if (mJson[Constants::ASSET_ATTR_GROUP].is_null())
        {
            mJson[Constants::ASSET_ATTR_GROUP] = Constants::ASSET_ATTR_GROUP_DEFAULT;
        }
        return mJson[Constants::ASSET_ATTR_GROUP];
    }

    void
    AssetDefinition::setGroup
    (string group)
    {
        mJson[Constants::ASSET_ATTR_GROUP] = group;
    }


    ProjectDefinition*
    AssetDefinition::getProject
    ()
    {
        return mProjectDefinition;
    }

    AssetDefinition*
    AssetDefinition::duplicate
    ()
    {
        auto newAD = mProjectDefinition->createNewAssetDefinition(getAssetType());
        newAD->mJson = mJson;
        newAD->setUuid(Uuid::generateUuid());
        string name = newAD->getName();
        regex numRegex("(\\d+)$");
        cmatch match;
        string resultStr;
        auto num = -1;

        if (regex_search(name.c_str(),match,numRegex))
        {
            resultStr = match[0].str();
            num = atoi(resultStr.c_str());
        }

        if (num > -1)
        {
            num++;
            name = name.substr(0,name.length()-resultStr.length());
            name.append(std::to_string(num));
            newAD->setName(name);
        }
        else
        {
            newAD->setName(getName()+".1");
        }

        return newAD;
    }

}
