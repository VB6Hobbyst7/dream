/*
 * LuaScriptInstance
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

#include "LuaScriptInstance.h"

namespace Dream
{

    LuaScriptInstance::LuaScriptInstance(AssetDefinition* definition, Transform3D* transform)
        : IAssetInstance(definition,transform)
    {
        return;
    }

    LuaScriptInstance::~LuaScriptInstance()
    {
        if (DEBUG)
        {
            cout << "LuaScriptInstance: Destroying Object" << endl;
        }
    }

    bool LuaScriptInstance::load(string projectPath)
    {
        mAbsolutePath = projectPath+mDefinition->getAssetPath();
        if (DEBUG)
        {
            cout << "LuaScriptInstance: Script at " << mAbsolutePath << endl;
        }
        return mAbsolutePath.size() != 0;
    }

    void LuaScriptInstance::update()
    {

    }

    void LuaScriptInstance::exposeLuaApi(lua_State* state)
    {

    }

} // End of Dream