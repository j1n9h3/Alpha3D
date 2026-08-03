#include "scenes/Landscape.h"

#include "core/Editor.h"
#include "core/Window.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProfiler.h"

void Landscape::Load(Window& window)
{
    atmosphere.Load(window);
    volumetricCloud.Load(window);
}

void Landscape::Render(RenderContext& context)
{
    A3_PROFILE_PASS(context.profiler, "Landscape");

    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean depthWriteEnabled = GL_TRUE;
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteEnabled);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    atmosphere.Render(context);

    glDepthMask(depthWriteEnabled);
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    volumetricCloud.Render(context);

    if (blendEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
}

void Landscape::Unload()
{
    volumetricCloud.Unload();
    atmosphere.Unload();
}

void Landscape::RenderEditor(Editor& editor)
{
    editor.BeginSkyAtmosphere(atmosphere);
    editor.BeginVolumetricCloud(volumetricCloud);
}
