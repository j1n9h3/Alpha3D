#pragma once

#include <glad/glad.h>

#include "scenes/BaseScene.h"
#include "scenes/SkyAtmosphere.h"
#include "scenes/VolumetricCloud.h"

class Landscape : public BaseScene
{
public:
    void Load(Window& window) override;
    void Render(RenderContext& context) override;
    void Unload() override;
    void RenderEditor(Editor& editor) override;

    std::string GetName() const override { return name; }

    SkyAtmosphere& GetAtmosphere() { return atmosphere; }
    const SkyAtmosphere& GetAtmosphere() const { return atmosphere; }
    VolumetricCloud& GetVolumetricCloud() { return volumetricCloud; }
    const VolumetricCloud& GetVolumetricCloud() const { return volumetricCloud; }

private:
    std::string name = "landscape";
    SkyAtmosphere atmosphere;
    VolumetricCloud volumetricCloud;
};
