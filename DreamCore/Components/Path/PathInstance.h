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

#pragma once

#include <memory>
#include <vector>

#include "PathDefinition.h"
#include "../IAssetInstance.h"

using namespace tinyspline;
using namespace std;
using namespace nlohmann;

namespace Dream
{
    class PathInstance : public IAssetInstance
    {

    public:
        PathInstance(
            PathDefinition*,
            SceneObjectRuntime*
        );
        ~PathInstance() override;
        bool load(string) override;

        double getUStep() const;
        void setUStep(double uStep);
        vector<vec3> getSplinePoints() const;
        vector<quat> getSplineTangents() const;
        vec3 getSplinePoint(int) const;
        void recalculate();

        void generate();
        const static int SPLINE_DIMENSIONS;
        const static int SPLINE_DEGREES;

        size_t getCurrentIndex() const;
        void setCurrentIndex(size_t currentIndex);

        Transform3D stepPath();

        bool getWrapPath() const;
        void setWrapPath(bool wrapPath);
        quat getHeading(vec3 point, vec3 v1, vec3 v2);

        vector<pair<vec3, vec3> > getSplineDerivatives() const;

    private:
        bool mWrapPath;
        size_t mCurrentIndex;
        double mUStep;
        vector<vec3> mSplinePoints;
        vector<pair<vec3,vec3>> mSplineDerivatives;
        vector<quat> mSplineTangents;

    }; // End of PathInstance

} // End of Dream
