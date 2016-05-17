/*
* Dream::Plugins::Scripting::Chai::ChaiScriptInstance
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

#include "ChaiScriptInstance.h"

namespace Dream {
namespace Asset {
namespace Instance {
namespace Script {
namespace ChaiScript {
    
    ChaiScriptInstance::ChaiScriptInstance(chaiscript::ChaiScript* engine) : Dream::Asset::Instance::AssetInstance() {
        mEngine = engine;
    }

    ChaiScriptInstance::ChaiScriptInstance(chaiscript::ChaiScript* engine, std::string objectUUID, const std::string script, bool fromFile) : Dream::Asset::Instance::AssetInstance(){
		mEngine = engine;
	    setObjectUUID(objectUUID);
	    if (fromFile) {
			setScriptFromFile(script);
	    } else {
	        setScriptFromString(script);
	    }
	}

	ChaiScriptInstance::~ChaiScriptInstance(void) {}
	
    void ChaiScriptInstance::setScriptFromString(const std::string script) {
		mEngine->eval(script);
	}

	void ChaiScriptInstance::setScriptFromFile(const std::string scriptFile) {
		mEngine->eval_file(scriptFile);
	}

    std::string ChaiScriptInstance::getObjectUUID() {
		return mObjectUUID;
	}

	void ChaiScriptInstance::setObjectUUID(std::string objectUUID) {
		mObjectUUID = objectUUID;
	}
	
} // End of ChaiScript
} // End of Script
} // End of Instance
} // End of Asset
} // End of Dream