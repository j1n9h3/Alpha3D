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


//void Editor::BeginFrame()
void Editor::BeginFrame(Viewport* viewport)
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

	ImGui::End();

	ImGui::PopStyleVar(); // style

	ImGui::ShowDemoWindow();
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


	ImGui::InputFloat("Speed", &camera.moveSpeed, 0.5f, 1.0f, "%.1f");


	ImGui::End();
}

void Editor::BeginDetails(GameObject& game_object) {
	Transform& transform = game_object.GetTransform();

	ImGui::Begin("Details");

	ImGui::Text(game_object.GetName().c_str());

	if (ImGui::CollapsingHeader("Transform")) {
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

	if (ImGui::CollapsingHeader("Inspector")) {

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

void Editor::BeginMainMenu(BaseScene*& currentScene, std::vector<BaseScene*>& scenes, Window& window) {
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

	// 根节点:只要有任何物体被选中就高亮
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