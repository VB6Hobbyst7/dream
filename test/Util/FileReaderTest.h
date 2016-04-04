/*
* JSONFileReaderTest
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

#ifndef FILEREADERTEST_H
#define FILEREADERTEST_H

#include "../../src/Unit/Unit.h"
#define TEST_JSON "test/Ash's Test Project.json"

namespace Dream {
	namespace Util {
		namespace Test {
			class FileReaderTest : public Dream::Unit::Unit {
			public:
				FileReaderTest(void);
				~FileReaderTest(void);
				void run();
				void testReadIntoStringStream();
				void testReadIntoBinaryStream();
			}; // End of JSONFileReaderTest
		} // End of Test
	} // End of Util
} // End of Dream

#endif // End of FILEREADERTEST_H
