// Editor.cpp
#pragma once
#include "core/Editor.h"

#include "scene/GameObject.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/nanosvgrast.h"
#include "core/Log.h"

#include "renderer/Environment.h"
#include "core/LogSink.h"

#include "scenes/BaseScene.h"
#include "scenes/SkyAtmosphere.h"
#include "utils/Recorder.h"

static GLuint LoadSVGIcon(const char* path, int w, int h) {
	NSVGimage* image = nsvgParseFromFile(path, "px", 96);
	if (!image) return 0;

	NSVGrasterizer* rast = nsvgCreateRasterizer();
	unsigned char* pixels = new unsigned char[w * h * 4];
	nsvgRasterize(rast, image, 0, 0, w / image->width, pixels, w, h, w * 4);

	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pixels;
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
	return texID;
}



void SetupImGuiStyle()
{
	// Unreal style by dev0-1 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.4f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;       // Blender 直角窗口
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None; // Blender 没有左上角折叠按钮
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(6.0f, 4.0f);  // 稍宽，Blender 按钮更宽松
	style.FrameRounding = 3.0f;                 // 轻微圆角
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	//style.IndentSpacing = 12.0f;       // Blender 缩进更小
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;       // Blender 滚动条细
	style.ScrollbarRounding = 0.0f;        // 直角滚动条
	style.GrabMinSize = 8.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);  // 垂直居中，Blender 列表项对齐
	style.TabBarBorderSize = 0.0f;
	style.ChildBorderSize = 0.0f;
	style.TreeLinesSize = 1.5f;

	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.98f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.58f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.58f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.45f, 0.75f, 0.60f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.90f, 0.58f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
	style.Colors[ImGuiCol_TreeLines] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);  // 加深线的颜色
	style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);  // 选中 tab 上的横条
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}


void Editor::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	m_IconView = LoadSVGIcon("assets/icons/camera.view.svg", 64, 64);
	m_IconTransform = LoadSVGIcon("assets/icons/camera.transform.svg", 64, 64);
	m_IconRotate = LoadSVGIcon("assets/icons/camera.rotate.svg", 64, 64);
	m_IconScale = LoadSVGIcon("assets/icons/camera.scale.svg", 64, 64);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

	this->font_name = "assets/fonts/Inter-Medium.ttf";

	this->font_small = io.Fonts->AddFontFromFileTTF(this->font_name.c_str(), 13.f * xscale);
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Medium.ttf", 14.0f * xscale);

    ImGui::GetStyle().ScaleAllSizes(xscale);

    LOG_INFO(Editor, "editor created x{} scale", xscale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

	SetupImGuiStyle();
}

Editor::~Editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void Editor::ShowToast(const std::string& message, float duration) {
	toast_message = message;
	toast_timer = duration;
	toast_duration = duration;
}

//void Editor::BeginFrame()
void Editor::BeginFrame(Viewport* viewport, Recorder* recorder)
{
    ImGui_ImplOpenGL3_NewFrame();
    
	ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();


	// 1.1 视窗
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // style

	ImGui::Begin("Viewport");

	ImVec2 viewportPos = ImGui::GetCursorScreenPos();
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	viewport->Resize(viewportSize.x, viewportSize.y);

	ImGui::Image((void*)(intptr_t)viewport -> GetColorTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	this->isViewportHovered = ImGui::IsItemHovered();

	// —— 录制取景框 ——
	if (recorder && recorder->GetShowGuide()) {
		float recAspect = (float)recorder->GetWidth() / recorder->GetHeight();
		float vpAspect = viewportSize.x / viewportSize.y;

		float w, h;
		if (vpAspect > recAspect) { h = viewportSize.y; w = h * recAspect; }
		else { w = viewportSize.x; h = w / recAspect; }

		ImVec2 rectMin(viewportPos.x + (viewportSize.x - w) * 0.5f,
			viewportPos.y + (viewportSize.y - h) * 0.5f);
		ImVec2 rectMax(rectMin.x + w, rectMin.y + h);

		ImDrawList* dl = ImGui::GetWindowDrawList();

		ImU32 white = IM_COL32(255, 160, 0, 230);
		ImU32 whiteThin = IM_COL32(255, 160, 0, 230);

		// 外框
		dl->AddRect(rectMin, rectMax, white, 0.0f, 0, 5.0f);

		// 九宫格三分线
		float thirdX = w / 3.0f;
		float thirdY = h / 3.0f;
		for (int i = 1; i <= 2; ++i) {
			float x = rectMin.x + thirdX * i;
			dl->AddLine(ImVec2(x, rectMin.y), ImVec2(x, rectMax.y), whiteThin, 3.0f);
			float y = rectMin.y + thirdY * i;
			dl->AddLine(ImVec2(rectMin.x, y), ImVec2(rectMax.x, y), whiteThin, 3.0f);
		}

		// 尺寸标注
		char buf[32];
		snprintf(buf, sizeof(buf), "%dx%d", recorder->GetWidth(), recorder->GetHeight());
		dl->AddText(ImVec2(rectMin.x + 4, rectMin.y + 4), white, buf);
	}


	ImGui::End();

	ImGui::PopStyleVar(); // style
}

void Editor::BeginCamera(Camera & camera) {
	ImGui::Begin("Camera");

	ImGui::Text("View:");

	auto fovToMm = [](float fov_deg, float sensor_diag = 43.27f) -> float {
		float fov_rad = glm::radians(fov_deg);
		return (sensor_diag / 2.0f) / std::tan(fov_rad / 2.0f);
	};

	auto mmToFov = [](float mm, float sensor_diag = 43.27f) -> float {
		return glm::degrees(2.0f * std::atan((sensor_diag / 2.0f) / mm));
	};

	float fov = camera.GetFov();
	float mm = fovToMm(fov);

	if (ImGui::SliderFloat("Focal Length", &mm, fovToMm(170.0f), fovToMm(10.0f), "%.1f mm")) {
		mm = glm::clamp(mm, 1.0f, 500.0f); // 防止极端值
		camera.SetFov(mmToFov(mm));
	}

	ImGui::Text("Movement:");

	glm::vec3 position = camera.GetPosition();
	if (ImGui::InputFloat3("Position", &position.x, "%.2f")) {
		camera.SetPosition(position);
	}
	ImGui::InputFloat("Speed", &camera.moveSpeed, 0.5f, 1.0f, "%.1f");


	ImGui::End();
}

void Editor::BeginDetails(GameObject& game_object) {
	Transform& transform = game_object.GetTransform();

	ImGui::Begin("Details");

	ImGui::Text(game_object.GetName().c_str());

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Location:");
		ImGui::DragFloat("X##trans_x", &transform.position[0], 0.005f, -FLT_MAX, +FLT_MAX, "%.2f m");
		ImGui::DragFloat("Y##trans_y", &transform.position[1], 0.005f, -FLT_MAX, +FLT_MAX, "%.2f m");
		ImGui::DragFloat("Z##trans_z", &transform.position[2], 0.005f, -FLT_MAX, +FLT_MAX, "%.2f m");
		ImGui::Spacing();

		ImGui::Text("Rotation:");
		ImGui::DragFloat("X##rotate_x", &transform.rotation[0], 0.2f, -FLT_MAX, +FLT_MAX, "%.0f deg");
		ImGui::DragFloat("Y##rotate_y", &transform.rotation[1], 0.2f, -FLT_MAX, +FLT_MAX, "%.0f deg");
		ImGui::DragFloat("Z##rotate_z", &transform.rotation[2], 0.2f, -FLT_MAX, +FLT_MAX, "%.0f deg");

		float scale = transform.scale[0];
		ImGui::Text("Scale:");
		ImGui::DragFloat("Scale##scale", &scale, 0.05f, -FLT_MAX, +FLT_MAX, "XYZ %.3f");

		transform.SetScale(glm::vec3(scale));

		transform.SyncToMatrix();
	}


	if (ImGui::CollapsingHeader("Animate", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Rotation Speed:");
		ImGui::SliderFloat("X##rotate_speed_x", &game_object.rotate_speed_x, -90.0f, 90.0f, "%.0f deg/s");
		ImGui::SliderFloat("Y##rotate_speed_y", &game_object.rotate_speed_y, -90.0f, 90.0f, "%.0f deg/s");
		ImGui::SliderFloat("Z##rotate_speed_z", &game_object.rotate_speed_z, -90.0f, 90.0f, "%.0f deg/s");
	}

	if (ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen)) {

		if (game_object.light) {
			ImGui::Text("Light:");
			static float light_intensity_low = 0.0f, light_intensity_high = 20.0f;
			ImGui::ColorEdit3("Color##1", (float*)&(*game_object.light).color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
			ImGui::SliderScalar("Intensity", ImGuiDataType_Float, &(*game_object.light).intensity, &light_intensity_low, &light_intensity_high, "%.1f lm");
		}

		if (game_object.pbr_sphere) {
			ImGui::Text("PBR Parameters:");
			static float pbr_roughness_low = 0.01f, pbr_roughness_high = 1.0f;
			static float pbr_metallic_low = 0.0f, pbr_metallic_high = 1.0f;
			ImGui::ColorEdit3("Albedo##pbr_albedo", (float*)&(*game_object.pbr_sphere).albedo, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
			ImGui::SliderScalar("Roughness", ImGuiDataType_Float, &(*game_object.pbr_sphere).roughness, &pbr_roughness_low, &pbr_roughness_high, "%.2f");
			ImGui::SliderScalar("Metallic", ImGuiDataType_Float, &(*game_object.pbr_sphere).metallic, &pbr_metallic_low, &pbr_metallic_high, "%.2f");
		}

		if (!game_object.light && !game_object.pbr_sphere) {
			ImGui::Text("This object has no specific properties.");
		}

	}

	ImGui::End();

}


void Editor::EndFrame()
{
	if (toast_timer > 0.0f) {
		toast_timer -= ImGui::GetIO().DeltaTime;
		if (toast_timer < 0.0f) {
			toast_timer = 0.0f;
		}

		float alpha = toast_timer < toast_fade_duration ? toast_timer / toast_fade_duration : 1.0f;
		alpha = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos;

		ImVec2 window_pos(
			work_pos.x + 20.0f,
			work_pos.y + 40.0f
		);

		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.85f * alpha);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoInputs;

		if (ImGui::Begin("PhotoToast", nullptr, flags)) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha));
			ImGui::TextUnformatted(toast_message.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		if (toast_timer < 0.0f) {
			toast_timer = 0.0f;
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


bool Editor::Hover() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool Editor::WantCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}


void Editor::BeginEnvironment(Environment& env) {

	ImGui::Begin("Environment");

	ImGui::Text("Cube Maps:");

	const auto& names = env.GetNames();

	// 把 vector<string> 转成 ImGui 需要的格式
	std::vector<const char*> items;
	for (const auto& n : names) items.push_back(n.c_str());

	int selected = env.GetSelected();
	if (ImGui::Combo("HDRI", &selected, items.data(), (int)items.size()))
		env.Select(selected);

	ImGui::End();
}

void Editor::BeginSkyAtmosphere(SkyAtmosphere& sky) {
	SkyAtmosphereParameters& p = sky.parameters;
	ImGui::Begin("Sky Atmosphere");

	if (ImGui::CollapsingHeader("Sun", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Light Direction", &p.lightDirection.x, 0.01f, -1.0f, 1.0f, "%.2f");
		if (glm::length(p.lightDirection) < 0.001f)
			p.lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
		ImGui::SliderFloat("Light Intensity", &p.lightIntensity, 0.0f, 100.0f, "%.1f");
	}

	if (ImGui::CollapsingHeader("Planet", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("Camera Height", &p.cameraHeight, 100.0f, 1.0f, 1000000.0f, "%.0f m");
		ImGui::DragFloat("Planet Radius", &p.planetRadius, 1000.0f, 1.0f, FLT_MAX, "%.0f m");
		ImGui::DragFloat("Atmosphere Radius", &p.atmosphereRadius, 1000.0f, p.planetRadius + 1.0f, FLT_MAX, "%.0f m");
		p.atmosphereRadius = glm::max(p.atmosphereRadius, p.planetRadius + 1.0f);
	}

	if (ImGui::CollapsingHeader("Scattering", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Use Rayleigh Scattering", &p.useRayleigh);
		ImGui::Checkbox("Use Mie Scattering", &p.useMie);
		ImGui::Checkbox("Use Absorption", &p.useAbsorption);
	}

	if (ImGui::CollapsingHeader("Ray Marching", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderInt("Primary Steps", &p.primarySteps, 1, 128);
		ImGui::SliderInt("Light Steps", &p.lightSteps, 1, 128);
	}
	
	if (ImGui::CollapsingHeader("Accelerating", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Use Transmittance LUT", &p.useTransmittanceLUT);
	}

	ImGui::End();
}

void Editor::BeginLog() {
	ImGui::Begin("Log");

	if (ImGui::Button("Clear")) LogBuffer::Get().Clear();
	ImGui::SameLine();
	if (ImGui::Button("Copy")) {
		std::string fullLog;
		for (const auto& entry : LogBuffer::Get().GetEntries())
			fullLog += entry.message;
		ImGui::SetClipboardText(fullLog.c_str());
	}

	ImGui::BeginChild("log_scroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& entry : LogBuffer::Get().GetEntries()) {
		ImVec4 color;
		switch (entry.level) {
		case LogLevel::Trace: color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
		case LogLevel::Info:  color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); break;
		case LogLevel::Warn:  color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f); break;
		case LogLevel::Error: color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}
	if (LogBuffer::Get().ShouldScrollToBottom()) {
		ImGui::SetScrollHereY(1.0f);
		LogBuffer::Get().ClearScroll();
	}
	ImGui::EndChild();
	ImGui::End();
}

// Editor.cpp
void Editor::BeginRecorder(Recorder& recorder) {

	bool locked = recorder.IsRecording();

	ImGui::Begin("Output");

	static const char* labels[] = {
		"1280x720", "1920x1080", "2560x1440", "3840x2160",
		"1440x2560 (Vertical)", "1080x1920 (Vertical)", "720x1280 (Vertical)", "1080x1080 (Square)",
		"Custom"
	};
	static const int dims[][2] = {
		{1280,720}, {1920,1080}, {2560,1440}, {3840,2160},
		{1440, 2560}, {1080,1920}, {720,1280}, {1080,1080}
	};
	static int sel = 1;
	static int custom[2] = { 1920, 1080 };
	const int customIndex = IM_ARRAYSIZE(labels) - 1;

	if (ImGui::CollapsingHeader("Output Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (locked) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Combo("Resolution", &sel, labels, IM_ARRAYSIZE(labels))) {
			if (sel != customIndex && !recorder.IsRecording()) {
				recorder.Destroy();
				recorder.Init(dims[sel][0], dims[sel][1]);
			}
		}
		if (sel == customIndex) {
			ImGui::InputInt2("Custom Size", custom);
			if (!recorder.IsRecording() && ImGui::Button("Apply Custom Resolution", ImVec2(ImGui::CalcItemWidth(), 0))) {
				int w = custom[0] > 0 ? custom[0] : 1;
				int h = custom[1] > 0 ? custom[1] : 1;
				recorder.Destroy();
				recorder.Init(w, h);
			}
		}

		bool guide = recorder.GetShowGuide();
		if (ImGui::Checkbox("Show Framing Guide", &guide))
			recorder.SetShowGuide(guide);

		if (locked) {
			ImGui::EndDisabled();
		}
	}

	if (ImGui::CollapsingHeader("Recorder", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (locked) {
			ImGui::BeginDisabled();
		}

		static char file_name[256] = "record/output.mp4";
		ImGui::InputText("Filename##Video", file_name, sizeof(file_name));

		// 帧率
		static int fps = 60;
		ImGui::InputInt("FPS", &fps);
		fps = fps < 1 ? 1 : (fps > 240 ? 240 : fps);

		// 码率 (Mbps)
		static int bitrate = 20;
		ImGui::InputInt("Bitrate (Mbps)", &bitrate);
		bitrate = bitrate < 1 ? 1 : bitrate;

		if (locked) {
			ImGui::EndDisabled();
		}

		if (recorder.IsRecording()) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("Stop Recording", ImVec2(ImGui::CalcItemWidth(), 0)))
				recorder.StopRecording();
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::Text("Recording...");
			ShowToast(std::string("Video Recording!"), 1.25f);

		}
		else {
			if (ImGui::Button("Start Record", ImVec2(ImGui::CalcItemWidth(), 0)))
				recorder.StartRecording(file_name, fps, bitrate);
		}
	}

	if (ImGui::CollapsingHeader("Capture", ImGuiTreeNodeFlags_DefaultOpen)) {
		static char photo_name[256] = "record/photo.png";
		ImGui::InputText("Filename##Photo", photo_name, sizeof(photo_name));

		if (ImGui::Button("Capture Photo", ImVec2(ImGui::CalcItemWidth(), 0))) {
			recorder.RequestPhoto(photo_name);
			ShowToast(std::string("Photo Captured!"), 1.25f);
		}
	}

	ImGui::End();
}

void Editor::BeginSceneSelect(BaseScene*& currentScene, std::vector<BaseScene*>& scenes, Window& window) {
	ImGui::Begin("Scene");

	if (ImGui::BeginCombo("Load Scene", currentScene->GetName().c_str())) {
		for (auto& scene : scenes) {
			bool selected = (scene == currentScene);
			if (ImGui::Selectable(scene->GetName().c_str(), selected)) {
				currentScene->Unload();
				currentScene = scene;
				currentScene->Load(window);
			}
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}


void Editor::BeginHierarchy(Scene& scene) {
	ImGui::Begin("Scene");
	static ImGuiTreeNodeFlags base_flags =
		ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
	GameObject* selected = scene.GetSelected();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.55f, 0.90f, 0.1f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.55f, 0.90f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.45f, 0.80f, 0.0f));

	static int item_index = 0; // 在 DrawGroup 调用前重置

	auto DrawObject = [&](GameObject* obj, int index) {
		ImGui::PushID(obj->GetID());
		ImGuiTreeNodeFlags flags = base_flags | ImGuiTreeNodeFlags_Leaf;

		// 交替背景
		float frame_padding_y = ImGui::GetStyle().FramePadding.y;
		float row_height = ImGui::GetTextLineHeightWithSpacing() + frame_padding_y * 2;

		if (index % 2 == 0) {
			ImVec2 row_min = ImGui::GetCursorScreenPos();
			row_min.x = ImGui::GetWindowPos().x;
			ImVec2 row_max = ImVec2(
				ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),
				row_min.y + row_height
			);
			ImGui::GetWindowDrawList()->AddRectFilled(row_min, row_max, IM_COL32(255, 255, 255, 6));
		}

		if (obj == selected) {
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.45f, 0.80f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.55f, 0.90f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.55f, 0.90f, 0.4f));
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.90f, 0.90f, 0.6f));
		}
		bool open = ImGui::TreeNodeEx(obj->GetName().c_str(), flags);
		if (obj == selected) ImGui::PopStyleColor(3);  else ImGui::PopStyleColor(1);
		if (ImGui::IsItemClicked()) scene.SetSelected(obj->GetID());
		if (open) ImGui::TreePop();
		ImGui::PopID();
		};

	auto DrawGroup = [&](const char* group_name, auto predicate) {
		ImGui::PushID(group_name);

		bool group_has_selected = false;
		if (selected) {
			for (const auto& obj : scene.GetGameObjects()) {
				if (predicate(obj.get()) && obj.get() == selected) {
					group_has_selected = true;
					break;
				}
			}
		}

		ImGuiTreeNodeFlags group_flags = base_flags | ImGuiTreeNodeFlags_DefaultOpen;
		if (group_has_selected) {
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.45f, 0.80f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.55f, 0.90f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.55f, 0.90f, 0.2f));
			group_flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool open = ImGui::TreeNodeEx(group_name, group_flags);

		if (group_has_selected)
			ImGui::PopStyleColor(3);

		if (open) {
			int index = 0;
			for (const auto& obj : scene.GetGameObjects()) {
				if (predicate(obj.get()))
					DrawObject(obj.get(), index++);
			}
			ImGui::TreePop();
		}
		
		ImGui::PopID();
	};

	bool root_has_selected = (selected != nullptr);
	ImGuiTreeNodeFlags root_flags = base_flags | ImGuiTreeNodeFlags_DefaultOpen;
	if (root_has_selected) {
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.20f, 0.45f, 0.80f, 0.12f));        // 最淡
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.55f, 0.90f, 0.12f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.55f, 0.90f, 0.12f));
		root_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool root_open = ImGui::TreeNodeEx("Scene Collection", root_flags);

	if (root_has_selected)
		ImGui::PopStyleColor(3);

	if (root_open) {
		DrawGroup("Lights", [](GameObject* o) { return o->light.has_value(); });
		DrawGroup("PBR Test Objects", [](GameObject* o) { return o->pbr_sphere.has_value(); });
		DrawGroup("Objects", [](GameObject* o) { return !o->light.has_value() && !o->pbr_sphere.has_value(); });
		ImGui::TreePop();
	}

	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor(3);
	ImGui::End();
}
