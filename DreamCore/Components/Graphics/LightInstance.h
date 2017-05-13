#ifndef LightInstance_h
#define LightInstance_h

#include "../../Constants.h"
#include "../../AssetDefinition.h"
#include "../../IAssetInstance.h"

namespace Dream {
      class LightInstance : public IAssetInstance {
          glm::vec3 mColor;
          float mIntensity;
      public:
        LightInstance(AssetDefinition*,Transform3D*);
        ~LightInstance();
        bool load(string);
        void loadExtraAttributes(nlohmann::json);

        glm::vec3 getColor();
        float getIntensity();
      void exposeLuaApi(lua_State*);
      }; // End of LightInstance
} // End of Dream

#endif /* LightInstance_h */