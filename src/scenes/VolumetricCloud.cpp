#include "scenes/VolumetricCloud.h"
#include "renderer/RenderContext.h"
#include "renderer/RenderProfiler.h"
#include "core/Window.h"
#include "stb/stb_image.h"
#include <cstdio>
#include <vector>
#include "core/Log.h"

bool VolumetricCloud::LoadCloudMapTexture() {
	int width = 0, height = 0, channels = 0;
	unsigned char* data = stbi_load("assets/textures/cloud-map.png", &width, &height, &channels, 4);
	if (!data) {
		LOG_ERROR(Texture, "Reading cloud map texture failed!");
		return false;
	}

	glGenTextures(1, &cloudMapTexture);
	glBindTexture(GL_TEXTURE_2D, cloudMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return true;
}

bool VolumetricCloud::LoadHeightMapTexture(unsigned int& texture, const char* fileName) {
	int width = 0, height = 0, channels = 0;
	unsigned char* data = stbi_load(fileName, &width, &height, &channels, 1);
	if (!data) {
		LOG_ERROR(Texture, "Reading cloud height texture {} failed!", fileName);
		return false;
	}

	GLint previousUnpackAlignment = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
	stbi_image_free(data);
	return true;
}

bool VolumetricCloud::UsesNoisePreviewScale(int mode) {
	return mode == 1 || mode == 2 || mode == 3 || mode == 7;
}

void VolumetricCloud::RemoveModeTransform(int mode) {
	if (!test_object)
		return;

	if (mode >= 4 && mode <= 6) {
		test_object->Translate(-parameters.cloudMapVolumeTranslation);
		test_object->Scale(glm::vec3(1.0f) / parameters.cloudMapVolumeScale);
	} else if (UsesNoisePreviewScale(mode)) {
		test_object->Scale(glm::vec3(1.0f / noisePreviewScale));
	}
}

void VolumetricCloud::ApplyModeTransform(int mode) {
	if (!test_object)
		return;

	if (mode >= 4 && mode <= 6) {
		test_object->Scale(parameters.cloudMapVolumeScale);
		test_object->Translate(parameters.cloudMapVolumeTranslation);
	} else if (UsesNoisePreviewScale(mode)) {
		test_object->Scale(glm::vec3(noisePreviewScale));
	}
}

void VolumetricCloud::SetNoiseMode(int mode) {
	if (mode == parameters.noiseMode)
		return;

	RemoveModeTransform(parameters.noiseMode);
	ApplyModeTransform(mode);
	parameters.noiseMode = mode;
}

void VolumetricCloud::SetCloudMapVolumeTransform(const glm::vec3& scale, const glm::vec3& translation) {
	if (test_object && parameters.noiseMode >= 4 && parameters.noiseMode <= 6) {
		test_object->Translate(-parameters.cloudMapVolumeTranslation);
		test_object->Scale(glm::vec3(1.0f) / parameters.cloudMapVolumeScale);
		test_object->Scale(scale);
		test_object->Translate(translation);
	}

	parameters.cloudMapVolumeScale = scale;
	parameters.cloudMapVolumeTranslation = translation;
}

bool VolumetricCloud::LoadNoiseTexture(unsigned int& texture, int channel) {
	if (channel < 0 || channel >= noiseTextureChannels) {
		LOG_ERROR(Texture, "Invalid volumetric cloud texture channel {}!", channel);
		return false;
	}

	std::vector<unsigned char> volume(
		noiseTextureSize * noiseTextureSize * noiseTextureSize);

	for (int z = 0; z < noiseTextureSize; ++z) {
		char fileName[64];
		snprintf(fileName, sizeof(fileName), "assets/textures/nubis2017.%03d.tga", z + 1);

		int width = 0, height = 0, channels = 0;
		unsigned char* slice = stbi_load(fileName, &width, &height, &channels, 4);
		if (!slice || width != noiseTextureSize || height != noiseTextureSize) {
			stbi_image_free(slice);
			LOG_ERROR(Texture, "Reading volumetric cloud texture {}, channel {} failed!", z + 1, channel);
			return false;
		}

		for (int y = 0; y < noiseTextureSize; ++y) {
			for (int x = 0; x < noiseTextureSize; ++x) {
				const int volumeIndex = x + y * noiseTextureSize + z * noiseTextureSize * noiseTextureSize;
				const int sliceIndex = (x + y * noiseTextureSize) * noiseTextureChannels;
				volume[volumeIndex] = slice[sliceIndex + channel];
			}
		}
		stbi_image_free(slice);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8,
		noiseTextureSize, noiseTextureSize, noiseTextureSize,
		0, GL_RED, GL_UNSIGNED_BYTE, volume.data());
	glGenerateMipmap(GL_TEXTURE_3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glBindTexture(GL_TEXTURE_3D, 0);
	return true;
}

bool VolumetricCloud::LoadHighFrequencyNoiseTexture() {
	std::vector<unsigned char> volume(highFrequencyNoiseTextureSize * highFrequencyNoiseTextureSize * highFrequencyNoiseTextureSize * 3);
	for (int z = 0; z < highFrequencyNoiseTextureSize; ++z) {
		char fileName[80];
		snprintf(fileName, sizeof(fileName), "assets/textures/nubis-2017-high-freq.%03d.tga", z + 1);
		int width = 0, height = 0, channels = 0;
		unsigned char* slice = stbi_load(fileName, &width, &height, &channels, 4);
		if (!slice || width != highFrequencyNoiseTextureSize || height != highFrequencyNoiseTextureSize) {
			stbi_image_free(slice);
			LOG_ERROR(Texture, "Reading high-frequency cloud texture {} failed!", z + 1);
			return false;
		}
		for (int y = 0; y < highFrequencyNoiseTextureSize; ++y) {
			for (int x = 0; x < highFrequencyNoiseTextureSize; ++x) {
				const int volumeIndex = (x + y * highFrequencyNoiseTextureSize + z * highFrequencyNoiseTextureSize * highFrequencyNoiseTextureSize) * 3;
				const int sliceIndex = (x + y * highFrequencyNoiseTextureSize) * 4;
				volume[volumeIndex] = slice[sliceIndex];
				volume[volumeIndex + 1] = slice[sliceIndex + 1];
				volume[volumeIndex + 2] = slice[sliceIndex + 2];
			}
		}
		stbi_image_free(slice);
	}

	glGenTextures(1, &highFrequencyNoiseTexture);
	glBindTexture(GL_TEXTURE_3D, highFrequencyNoiseTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, highFrequencyNoiseTextureSize, highFrequencyNoiseTextureSize, highFrequencyNoiseTextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, volume.data());
	glGenerateMipmap(GL_TEXTURE_3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glBindTexture(GL_TEXTURE_3D, 0);
	return true;
}

void VolumetricCloud::Load(Window& window) {
	BaseScene::Load(window);
	test_object = &scene.AddGameObject("test_object", &cubeMesh, &shader_volumetric_cloud);
	scene.SetSelected(test_object->GetID());
	ApplyModeTransform(parameters.noiseMode);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LoadNoiseTexture(perlinWorleyNoiseTexture, 0);
	LoadNoiseTexture(worleyNoise1, 1);
	LoadNoiseTexture(worleyNoise2, 2);
	LoadNoiseTexture(worleyNoise3, 3);
	LoadHighFrequencyNoiseTexture();
	LoadCloudMapTexture();
	LoadHeightMapTexture(cumulusHeightTexture, "assets/textures/cumulus.png");
	LoadHeightMapTexture(cumulonimbusHeightTexture, "assets/textures/cumulonumbis.png");
	LoadHeightMapTexture(stratusHeightTexture, "assets/textures/stratus.png");
	LoadHeightMapTexture(baseReduceTexture, "assets/textures/base_reduce.png");
}

void VolumetricCloud::Render(RenderContext& context) {
	A3_PROFILE_PASS(context.profiler, "Volumetric Cloud Cube");

	Camera& camera = context.camera;
	shader_volumetric_cloud.use();

	glm::mat4 model = test_object->GetTransform().GetModelMatrix();
	glm::mat4 inverseModel = glm::inverse(model);

	shader_volumetric_cloud.setMat4("model", model);
	shader_volumetric_cloud.setMat4("inverseModel", inverseModel);
	shader_volumetric_cloud.setMat4("view", camera.GetView());
	shader_volumetric_cloud.setMat4("projection", camera.GetProjection());
	shader_volumetric_cloud.setVec3("cameraPos", camera.GetPosition());
	const glm::vec3 lightDirection = glm::length(parameters.lightDirection) > 0.0001f ? glm::normalize(parameters.lightDirection) : glm::vec3(0.0f, 1.0f, 0.0f);
	shader_volumetric_cloud.setVec3("lightDirection", lightDirection);
	shader_volumetric_cloud.setVec3("lightColor", parameters.lightColor);
	shader_volumetric_cloud.setVec3("ambientLight", parameters.ambientLight);
	shader_volumetric_cloud.setFloat("lightIntensity", parameters.lightIntensity);
	shader_volumetric_cloud.setFloat("densityScale", parameters.densityScale);
	shader_volumetric_cloud.setFloat("extinction", parameters.extinction);
	shader_volumetric_cloud.setFloat("shapeScale", parameters.shapeScale);
	shader_volumetric_cloud.setFloat("detailScale", parameters.detailScale);
	shader_volumetric_cloud.setFloat("erosionStrength", parameters.erosionStrength);
	shader_volumetric_cloud.setVec3("windDirection", parameters.windDirection);
	shader_volumetric_cloud.setFloat("cloudSpeed", parameters.cloudSpeed);
	shader_volumetric_cloud.setFloat("cloudTopOffset", parameters.cloudTopOffset);
	shader_volumetric_cloud.setFloat("anvilBias", parameters.anvilBias);
	shader_volumetric_cloud.setFloat("cloudCoverageBlend", parameters.cloudCoverageBlend);
	shader_volumetric_cloud.setFloat("time", static_cast<float>(glfwGetTime()));
	shader_volumetric_cloud.setFloat("cubeNoiseScale", parameters.cubeNoiseScale);
	shader_volumetric_cloud.setFloat("cubeDetailStrength", parameters.cubeDetailStrength);
	shader_volumetric_cloud.setFloat("cubeDensityThreshold", parameters.cubeDensityThreshold);
	shader_volumetric_cloud.setFloat("cubeEdgeSoftness", parameters.cubeEdgeSoftness);
	shader_volumetric_cloud.setFloat("cubeBottomFade", parameters.cubeBottomFade);
	shader_volumetric_cloud.setFloat("cubeTopFade", parameters.cubeTopFade);
	shader_volumetric_cloud.setInt("maxSteps", parameters.maxSteps);
	shader_volumetric_cloud.setInt("lightSteps", parameters.lightSteps);
	shader_volumetric_cloud.setFloat("rayJitterStrength", parameters.rayJitterStrength);
	shader_volumetric_cloud.setFloat("transmittanceCutoff", parameters.transmittanceCutoff);
	shader_volumetric_cloud.setFloat("phaseG", parameters.phaseG);
	shader_volumetric_cloud.setInt("MODE", parameters.noiseMode);
	shader_volumetric_cloud.setInt("perlinWorleyNoiseTexture", 2);
	shader_volumetric_cloud.setInt("worleyNoise1", 3);
	shader_volumetric_cloud.setInt("worleyNoise2", 4);
	shader_volumetric_cloud.setInt("worleyNoise3", 5);
	shader_volumetric_cloud.setInt("cloudMapTexture", 6);
	shader_volumetric_cloud.setInt("cumulusHeightTexture", 7);
	shader_volumetric_cloud.setInt("cumulonimbusHeightTexture", 8);
	shader_volumetric_cloud.setInt("stratusHeightTexture", 9);
	shader_volumetric_cloud.setInt("baseReduceTexture", 10);
	shader_volumetric_cloud.setInt("highFrequencyNoiseTexture", 11);

	shader_volumetric_cloud.setMat4("invProjection", glm::inverse(camera.GetProjection()));
	shader_volumetric_cloud.setMat4("invView", glm::inverse(camera.GetView()));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, perlinWorleyNoiseTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, worleyNoise1);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_3D, worleyNoise2);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, worleyNoise3);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, cloudMapTexture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, cumulusHeightTexture);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, cumulonimbusHeightTexture);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, stratusHeightTexture);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, baseReduceTexture);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_3D, highFrequencyNoiseTexture);
	glActiveTexture(GL_TEXTURE0);

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

	test_object->Draw();

	glDepthMask(depthWriteEnabled);
	glDepthFunc(previousDepthFunction);
	glCullFace(previousCullMode);
	if (cullEnabled) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (depthClampEnabled) glEnable(GL_DEPTH_CLAMP); else glDisable(GL_DEPTH_CLAMP);
}

void VolumetricCloud::Unload() {
	glDisable(GL_BLEND);
	const unsigned int textures[] = {
		perlinWorleyNoiseTexture, worleyNoise1, worleyNoise2, worleyNoise3
	};
	glDeleteTextures(noiseTextureChannels, textures);
	glDeleteTextures(1, &highFrequencyNoiseTexture);
	glDeleteTextures(1, &cloudMapTexture);
	const unsigned int heightTextures[] = {
		cumulusHeightTexture, cumulonimbusHeightTexture, stratusHeightTexture, baseReduceTexture
	};
	glDeleteTextures(4, heightTextures);
	perlinWorleyNoiseTexture = 0;
	worleyNoise1 = 0;
	worleyNoise2 = 0;
	worleyNoise3 = 0;
	highFrequencyNoiseTexture = 0;
	cloudMapTexture = 0;
	cumulusHeightTexture = 0;
	cumulonimbusHeightTexture = 0;
	stratusHeightTexture = 0;
	baseReduceTexture = 0;
}


void VolumetricCloud::RenderEditor(Editor& editor) {
	editor.BeginVolumetricCloud(*this);
	BaseScene::RenderEditor(editor);
}
