#include "scenes/PBRIBL_Base.h"
#include "renderer/RenderProfiler.h"
#include "renderer/RenderContext.h"

void PBRIBL_Base::Load(Window& window)
{
    BaseScene::Load(window); // glEnable
    env_map.Scan(project_path + "/assets/hdri");
    {
        ibl.Load(env_map.GetSelectedPath(), cubeMesh, to_cubemap_shader, irradiance_shader, prefilter_shader, brdf_integrate_shader);
    }
}

void PBRIBL_Base::Render(RenderContext& context)
{
    A3_PROFILE_PASS(context.profiler, "Skybox");
    Camera& camera = context.camera;
    // skybox
    glDepthFunc(GL_LEQUAL);
    skybox_shader.use();
    skybox_shader.setMat4("projection", camera.GetProjection());
    skybox_shader.setMat4("view", camera.GetView());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.envCubemap);
    skybox_shader.setInt("environmentMap", 0);
    cubeMesh.Draw();
    glDepthFunc(GL_LESS);
}

void PBRIBL_Base::Unload()
{
    scene.Clear();
}


void PBRIBL_Base::RenderEditor(Editor& editor)
{

    editor.BeginEnvironment(env_map);
    if (env_map.HasChanged()) {
        ibl.Load(
            env_map.GetSelectedPath(), cubeMesh,
            to_cubemap_shader, irradiance_shader,
            prefilter_shader, brdf_integrate_shader
        );
        env_map.ClearChanged();
    }

    BaseScene::RenderEditor(editor);

}
