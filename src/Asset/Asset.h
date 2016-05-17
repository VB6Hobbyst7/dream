/*
* Asset
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

#ifndef ASSET_H
#define ASSET_H


#include <iostream>
#include <map>

#include "Instance/AssetInstance.h"
#include "../JSON/json.hpp"

#define ASSET_UUID   "uuid"
#define ASSET_NAME   "name"
#define ASSET_TYPE   "type"
#define ASSET_FORMAT "format"

#define DIR_PATH_SEP        "/"
#define ASSET_DIR        "asset"
#define ASSET_DIR_MODEL  "model"
#define ASSET_DIR_AUDIO  "audio"
#define ASSET_DIR_SCRIPT "script"

#define ASSET_TYPE_MODEL  "Model"
#define ASSET_TYPE_AUDIO  "Audio"
#define ASSET_TYPE_SCRIPT "Script"

namespace Dream {
	namespace Asset {
		class Asset {
			std::map<std::string,std::string> mAttributes;
		public:
			Asset(void);
			Asset(nlohmann::json);
			~Asset(void);
			
			std::pair<std::string,std::string> mapPair(std::string,std::string);

			void setUUID(std::string);
			std::string getUUID();

			void setName(std::string);
			std::string getName();

			void setType(std::string);
			std::string getType();

			void setFormat(std::string);
			std::string getFormat();
		}; // End of Asset
	} // End of Asset
} // End of Dream

#endif // End of ASSET_H