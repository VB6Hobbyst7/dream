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

#include "Uuid.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>

using namespace std::chrono;

namespace Dream
{
    uint32_t Uuid::generateUuid()
    {
        srand(static_cast<unsigned int>(high_resolution_clock::now().time_since_epoch().count()));
        uint32_t retval = 0;
        while (retval == 0)
        {
           retval = static_cast<uint32_t>(rand());
        }
        return retval;
    }
}
