/*
* Dream::Asset::Animation::Test::TestFrameDelta
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

#include "TestFrameDelta.h"
#include "../../../src/Asset/Instance/Animation/Frame.h"

namespace Dream {
	namespace Asset {
		namespace Animation {
			namespace Test {
				TestFrameDelta::TestFrameDelta() : Dream::Unit::Unit("Dream::Asset::Animation::FrameDelta"){
				}

				TestFrameDelta::~TestFrameDelta() {
				}

				void TestFrameDelta::run(void) {
					header();
					testComputeFrameDelta();
				}

				void TestFrameDelta::testComputeFrameDelta() {
                    Dream::Asset::Instance::Animation::Frame *frame =
                        new Dream::Asset::Instance::Animation::Frame(0);
                    
					Dream::Asset::Instance::Animation::FrameDelta *delta1 =
                        new Dream::Asset::Instance::Animation::FrameDelta(0,DA_OP_LINEAR);
                    
					Dream::Asset::Instance::Animation::FrameDelta *delta2 =
                        new Dream::Asset::Instance::Animation::FrameDelta(0,DA_OP_LINEAR);

					frame->addFrameDelta(delta1);
					frame->addFrameDelta(delta2);

					assertNotNull("Frame Delta Computed for Frame",delta1->computeFrameDelta(delta1,delta2,10,5));
					return;
				}

			} // End of Test
		} // End of Animation
	} // End of Asset
} // End of Dream