/*
* Resource
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

#ifndef RESOURCE_H
#define RESOURCE_H


#include <iostream>
#include <map>

#include "Instance/ResourceInstance.h"
#include "../JSON/json.hpp"

#define RESOURCE_JSON_UUID   "uuid"
#define RESOURCE_JSON_NAME   "name"
#define RESOURCE_JSON_TYPE   "type"
#define RESOURCE_JSON_FORMAT "format"

#define DIR_PATH_SEP        "/"
#define RESOURCE_DIR        "resource"
#define RESOURCE_DIR_MODEL  "model"
#define RESOURCE_DIR_AUDIO  "audio"
#define RESOURCE_DIR_SCRIPT "script"

#define RESOURCE_TYPE_MODEL  "Model"
#define RESOURCE_TYPE_AUDIO  "Audio"
#define RESOURCE_TYPE_SCRIPT "Script"

namespace Dream {
	namespace Resource {
		class Resource {
			std::map<std::string,std::string> mAttributes;
		public:
			Resource(void);
			Resource(nlohmann::json);
			virtual ~Resource(void) {};

			void setUUID(std::string);
			std::string getUUID();

			void setName(std::string);
			std::string getName();

			virtual void generateAbsolutePaths(std::string, std::string) = 0;
			virtual Instance::ResourceInstance* createInstance() = 0;
		}; // End of Resource
	} // End of Resource
} // End of Dream

#endif // End of RESOURCE_H
