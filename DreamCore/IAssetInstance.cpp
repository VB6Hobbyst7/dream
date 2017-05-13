/*
* AssetInstance
*
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

#include "Uuid.h"
#include "IAssetInstance.h"

namespace Dream
{
    IAssetInstance::IAssetInstance(AssetDefinition* definition, Transform3D* transform)
        : ILuaExposable()
    {
        mDefinition = definition;
        mTransform = transform;
        mLoaded = false;
        mName = mDefinition->getName();
        mUuid = Uuid::generateUuid();
    }

    IAssetInstance::~IAssetInstance()
    {
    }

    string IAssetInstance::getName()
    {
        return mDefinition->getName();
    }

    string IAssetInstance::getUuid()
    {
        return mUuid;
    }

    string IAssetInstance::getNameAndUuidString()
    {
        return getName() + " (" + getUuid() + ")";
    }

    Transform3D* IAssetInstance::getTransform()
    {
        return mTransform;
    }

    string IAssetInstance::getAbsolutePath()
    {
        return mAbsolutePath;
    }

    void IAssetInstance::setAbsolutePath(string path)
    {
        mAbsolutePath = path;
    }

    bool IAssetInstance::getLoadedFlag()
    {
        return mLoaded;
    }

    void IAssetInstance::setLoadedFlag(bool loaded)
    {
        mLoaded = loaded;
    }

    void IAssetInstance::exposeLuaApi(lua_State* state)
    {

    }
} // End of Dream