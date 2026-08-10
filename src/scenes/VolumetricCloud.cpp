#include "scenes/VolumetricCloud.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProfiler.h"
#include "core/Window.h"
#include "stb/stb_image.h"
#include <cstdio>
#include <vector>
#include "core/Log.h"

void VolumetricCloud::updateVolumeTransform(const glm::vec3& scale, const glm::vec3& translation) {
	cloudVolume->GetTransform().SetScale(scale);
	cloudVolume->GetTransform().SetPosition(translation);
}

void VolumetricCloud::Load(Window& window) {
	BaseScene::Load(window);
	cloudVolume = &scene.AddGameObject("cloudVolume", &cubeMesh, &shader_volumetric_cloud);
	scene.SetSelected(cloudVolume->GetID());

	cloudVolume->Translate(parameters.cloudMapVolumeTranslation);
	cloudVolume->Scale(parameters.cloudMapVolumeScale);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LoadVolumeTex(lowFreqNoiseTex, 128, "assets/textures/nubis2017_lowFrexqTex_128x128/nubis2017.%03d.tga");
	LoadVolumeTex(highFreqNoiseTex, 32, "assets/textures/nubis2017_highFrexqTex_32x32/nubis-2017-high-freq.%03d.tga");	
	LoadTex(heightTex, "assets/textures/cloud_height.tga");
	LoadTex(cloudMapTex, "assets/textures/cloud_map_5.png");
}

void VolumetricCloud::Render(RenderContext& context) {
	A3_PROFILE_PASS(context.profiler, "Volumetric Cloud Cube");

	Camera& camera = context.camera;
	shader_volumetric_cloud.use();

	glm::mat4 model = cloudVolume->GetTransform().GetModelMatrix();
	glm::mat4 inverseModel = glm::inverse(model);

	// camera options
	shader_volumetric_cloud.setMat4("model", model);
	shader_volumetric_cloud.setMat4("inverseModel", inverseModel);
	shader_volumetric_cloud.setMat4("view", camera.GetView());
	shader_volumetric_cloud.setMat4("projection", camera.GetProjection());
	shader_volumetric_cloud.setVec3("cameraPos", camera.GetPosition());
	shader_volumetric_cloud.setMat4("invProjection", glm::inverse(camera.GetProjection()));
	shader_volumetric_cloud.setMat4("invView", glm::inverse(camera.GetView()));

	// light options
	const glm::vec3 lightDirection = glm::length(parameters.lightDirection) > 0.0001f ? glm::normalize(parameters.lightDirection) : glm::vec3(0.0f, 1.0f, 0.0f);
	shader_volumetric_cloud.setVec3("lightDirection", lightDirection);
	shader_volumetric_cloud.setVec3("lightColor", parameters.lightColor);
	shader_volumetric_cloud.setVec3("ambientLight", parameters.ambientLight);
	shader_volumetric_cloud.setFloat("lightIntensity", parameters.lightIntensity);
	shader_volumetric_cloud.setFloat("extinction", parameters.extinction);

	// density options
	shader_volumetric_cloud.setFloat("densityScale", parameters.densityScale);
	shader_volumetric_cloud.setFloat("anvilBias", parameters.anvilBias);
	shader_volumetric_cloud.setFloat("cloudCoverageBlend", parameters.cloudCoverageBlend);
	shader_volumetric_cloud.setFloat("cloudMapScale", parameters.cloudMapScale);

	// noise options
	shader_volumetric_cloud.setBool("useLowFreqNoise", parameters.useLowFreqNoise);
	shader_volumetric_cloud.setBool("useHighFreqNoise", parameters.useHighFreqNoise);
	shader_volumetric_cloud.setFloat("lowFreqNoiseScale", parameters.lowFreqNoiseScale);
	shader_volumetric_cloud.setFloat("highFreqNoiseScale", parameters.highFreqNoiseScale);
	shader_volumetric_cloud.setFloat("erosionStrength", parameters.erosionStrength);


	// animation options
	shader_volumetric_cloud.setFloat("cloudSpeed", parameters.cloudSpeed);
	shader_volumetric_cloud.setVec3("windDirection", parameters.windDirection);
	shader_volumetric_cloud.setFloat("time", static_cast<float>(glfwGetTime()));
	shader_volumetric_cloud.setFloat("cloudTopOffset", parameters.cloudTopOffset);



	// ray marching options
	shader_volumetric_cloud.setInt("primarySteps", parameters.primarySteps);
	shader_volumetric_cloud.setInt("lightSteps", parameters.lightSteps);
	shader_volumetric_cloud.setFloat("rayJitterStrength", parameters.rayJitterStrength);
	shader_volumetric_cloud.setFloat("transmittanceCutoff", parameters.transmittanceCutoff);
	shader_volumetric_cloud.setFloat("phaseG", parameters.phaseG);


	// binding textures
	shader_volumetric_cloud.setInt("lowFreqNoiseTex", 0);
	shader_volumetric_cloud.setInt("highFreqNoiseTex", 1);
	shader_volumetric_cloud.setInt("cloudMapTex", 2);
	shader_volumetric_cloud.setInt("heightTex", 3);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, lowFreqNoiseTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, highFreqNoiseTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cloudMapTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, heightTex);
	

	GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
	GLint previousCullMode = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &previousCullMode);
	GLint previousDepthFunction = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunction);
	GLboolean depthWriteEnabled = GL_TRUE;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteEnabled);
	GLboolean depthClampEnabled = glIsEnabled(GL_DEPTH_CLAMP);

	glm::vec3 localCameraPos = glm::vec3(inverseModel * glm::vec4(camera.GetPosition(), 1.0f));

	constexpr float epsilon = 0.0001f;

	bool cameraInsideVolume = localCameraPos.x >= -0.5f - epsilon &&
		localCameraPos.x <= 0.5f + epsilon &&
		localCameraPos.y >= -0.5f - epsilon &&
		localCameraPos.y <= 0.5f + epsilon &&
		localCameraPos.z >= -0.5f - epsilon &&
		localCameraPos.z <= 0.5f + epsilon;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(cameraInsideVolume ? GL_FRONT : GL_BACK);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	cloudVolume->Draw();

	glDepthMask(depthWriteEnabled);
	glDepthFunc(previousDepthFunction);
	glCullFace(previousCullMode);
	if (cullEnabled) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (depthClampEnabled) glEnable(GL_DEPTH_CLAMP); else glDisable(GL_DEPTH_CLAMP);
}

void VolumetricCloud::Unload() {
	glDisable(GL_BLEND);
	const unsigned int textures[] = {
		lowFreqNoiseTex, highFreqNoiseTex
	};
	glDeleteTextures(1, &lowFreqNoiseTex);
	glDeleteTextures(1, &highFreqNoiseTex);
	glDeleteTextures(1, &cloudMapTex);
	glDeleteTextures(1, &heightTex);

	lowFreqNoiseTex = 0;
	highFreqNoiseTex = 0;
	cloudMapTex = 0;
	heightTex = 0;
}


void VolumetricCloud::RenderEditor(Editor& editor) {
	editor.BeginVolumetricCloud(*this);
	BaseScene::RenderEditor(editor);
}
