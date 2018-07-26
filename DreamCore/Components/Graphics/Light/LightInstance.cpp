#include "LightInstance.h"

#include "LightDefinition.h"
#include "../../../Scene/SceneObject/SceneObjectRuntime.h"

namespace Dream
{
    LightInstance::LightInstance
    (
        LightDefinition* definition,
        SceneObjectRuntime* transform
    ) : IAssetInstance(definition,transform),
        ILoggable("LightInstance"),
        mColor(glm::vec3(0.0f,0.0f,0.0f)),
        mIntensity(0.0f)

    {
        loadExtraAttributes(definition->getJson());
    }

    LightInstance::~LightInstance
    ()
    {
        auto log = getLog();
        log->info("Destroying Object" );
        return;
    }

    bool
    LightInstance::load
    (string projectDir)
    {
        mLoaded = true;
        return mLoaded;
    }

    void
    LightInstance::loadExtraAttributes
    (nlohmann::json json)
    {
        auto log = getLog();
        nlohmann::json jsonColor = json[Constants::ASSET_ATTR_LIGHT_COLOR];
        if (!jsonColor.is_null())
        {
            mColor.r   = static_cast<float>(jsonColor[Constants::RED]);
            mColor.g   = static_cast<float>(jsonColor[Constants::GREEN]);
            mColor.b   = static_cast<float>(jsonColor[Constants::BLUE]);
            mIntensity = static_cast<float>(jsonColor[Constants::ALPHA]);
        }
        else
        {
            log->error("Unable to find color in json" );
        }
    }

    glm::vec3
    LightInstance::getColor
    ()
    {
        return mColor;
    }

    float
    LightInstance::getIntensity
    ()
    {
        return mIntensity;
    }

} // End of Dream
