#include "scenes/VolumetricCloud.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProfiler.h"
#include "core/Window.h"
#include "stb/stb_image.h"
#include <cstdio>
#include "core/Log.h"

void VolumetricCloud::Load(Window& window) {
	BaseScene::Load(window);
	test_object = &scene.AddGameObject("test_object", &cubeMesh, &shader_volumetric_cloud);
	scene.SetSelected(test_object->GetID());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1, &volumeTexture);
	std::vector<unsigned char> volume(128 * 128 * 128, 0);
	bool volumeLoaded = true;
	for (int z = 0; z < 128; ++z) {
		char fileName[64];
		snprintf(fileName, sizeof(fileName), "assets/textures/nubis2017.%03d.tga", z + 1);
		int width = 0, height = 0, channels = 0;
		unsigned char* slice = stbi_load(fileName, &width, &height, &channels, 4);
		if (!slice || width != 128 || height != 128) {
			volumeLoaded = false;
			stbi_image_free(slice);
			LOG_ERROR(Texture, "Reading volumetric cloud texture {} failed!", z);
			break;
		}
		for (int y = 0; y < 128; ++y)
			for (int x = 0; x < 128; ++x)
				volume[x + y * 128 + z * 128 * 128] = slice[(x + y * 128) * 4 + 0];
		stbi_image_free(slice);
	}
	if (volumeLoaded) {
		glBindTexture(GL_TEXTURE_3D, volumeTexture);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 128, 128, 128, 0, GL_RED, GL_UNSIGNED_BYTE, volume.data());
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}

void VolumetricCloud::Render(RenderContext& context) {
	A3_PROFILE_PASS(context.profiler, "Volumetric Cloud Cube");

	Camera& camera = context.camera;
	shader_volumetric_cloud.use();

	// MAIN initializes the camera at the origin looking along +Z.
	glm::mat4 model = test_object->GetTransform().GetModelMatrix();

	shader_volumetric_cloud.setMat4("model", model);
	shader_volumetric_cloud.setMat4("inverseModel", glm::inverse(model));
	shader_volumetric_cloud.setMat4("view", camera.GetView());
	shader_volumetric_cloud.setMat4("projection", camera.GetProjection());
	shader_volumetric_cloud.setVec3("cameraPos", camera.GetPosition());
	shader_volumetric_cloud.setVec3("lightDirection", glm::normalize(glm::vec3(-0.4f, 0.8f, 0.5f)));
	shader_volumetric_cloud.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.85f));
	shader_volumetric_cloud.setFloat("densityScale", 2.0f);
	shader_volumetric_cloud.setFloat("extinction", 2.0f);
	shader_volumetric_cloud.setInt("maxSteps", 64);
	shader_volumetric_cloud.setInt("lightSteps", 8);
	shader_volumetric_cloud.setInt("volumeTexture", 2);

	shader_volumetric_cloud.setMat4("invProjection", glm::inverse(camera.GetProjection()));
	shader_volumetric_cloud.setMat4("invView", glm::inverse(camera.GetView()));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glActiveTexture(GL_TEXTURE0);

	// getting params before
	GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);

	GLint previousCullMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &previousCullMode);

	GLboolean depthWriteEnabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteEnabled);

	glm::mat4 inverseModel = glm::inverse(test_object->GetTransform().GetModelMatrix());

	glm::vec3 localCameraPos = glm::vec3(
		inverseModel * glm::vec4(camera.GetPosition(), 1.0f)
	);

	constexpr float epsilon = 0.0001f;

	bool cameraInsideVolume =
		localCameraPos.x >= -0.5f - epsilon &&
		localCameraPos.x <= 0.5f + epsilon &&
		localCameraPos.y >= -0.5f - epsilon &&
		localCameraPos.y <= 0.5f + epsilon &&
		localCameraPos.z >= -0.5f - epsilon &&
		localCameraPos.z <= 0.5f + epsilon;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(cameraInsideVolume ? GL_FRONT : GL_BACK);
	glDepthMask(GL_FALSE);

	test_object->Draw();

	// »Ö¸´Ö®Ç°µÄ×´Ì¬
	glDepthMask(depthWriteEnabled);
	glCullFace(previousCullMode);

	if (cullEnabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void VolumetricCloud::Unload() {
	glDisable(GL_BLEND);
	glDeleteTextures(1, &volumeTexture);
	volumeTexture = 0;
}


void VolumetricCloud::RenderEditor(Editor& editor) {
	BaseScene::RenderEditor(editor);
}