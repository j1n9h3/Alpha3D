// Editor.cpp
#include "core/Editor.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/nanosvgrast.h"


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
	style.IndentSpacing = 12.0f;       // Blender 缩进更小
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
	style.WindowBorderSize = 0.0f;
	style.ChildBorderSize = 0.0f;

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


}


Editor::Editor(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	m_IconView = LoadSVGIcon("assets/icons/camera.view.svg", 64, 64);
	m_IconTransform = LoadSVGIcon("assets/icons/camera.transform.svg", 64, 64);
	m_IconRotate = LoadSVGIcon("assets/icons/camera.rotate.svg", 64, 64);
	m_IconScale = LoadSVGIcon("assets/icons/camera.scale.svg", 64, 64);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

	this->font_small = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Medium.ttf", 13.f * xscale);
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Medium.ttf", 14.0f * xscale);

    ImGui::GetStyle().ScaleAllSizes(xscale);

    LOG_INFO(Editor, "Success creating editor with x{} scale", xscale);

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

void Editor::BeginFrame()
{

    ImGui_ImplOpenGL3_NewFrame();
    
	ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	ImGui::Begin("Panel");

	if (ImGui::CollapsingHeader("Program Info", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("%s", glGetString(GL_RENDERER));
		
		static float fpsHistory[100] = {};
		static int fpsIndex = 0;
		fpsHistory[fpsIndex++ % 100] = ImGui::GetIO().Framerate;
		ImGui::PlotLines("FPS", fpsHistory, 100, fpsIndex, nullptr, 0, 200);

		float avgFps = 0;
		for (float f : fpsHistory) avgFps += f;
		avgFps /= 100;
		ImGui::Text("FPS: %.1f", avgFps);

		ImGui::Text("Program Running Time: %.1f s", (float)glfwGetTime());

		GLint totalMem = 0, availMem = 0;
		glGetIntegerv(0x9048, &totalMem);
		glGetIntegerv(0x9049, &availMem);
		if (totalMem > 0) {
			ImGui::Text("VRAM: %.1f / %.1f GB", ((float)totalMem - availMem) / (1024 * 1024), (float)totalMem / (1024 * 1024));
		}
		else {
			ImGui::Text("VRAM: N/A");
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




void Editor::OnImGuiCamera(Camera& camera, ImGuizmo::OPERATION& gizmoOp) {

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

	float headbarWidth = viewport->Size.x / 3 * 2;
	float menuBarHeight = ImGui::GetFrameHeight();

	ImGui::SetNextWindowPos(ImVec2(
		viewport->Pos.x + 2.0f,
		viewport->Pos.y)
	);

	float menuBarH = ImGui::GetFrameHeight();
	ImGui::SetNextWindowBgAlpha(0.0f);
	constexpr ImGuiWindowFlags kToolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
	if (ImGui::Begin("##toolbar", nullptr, kToolbarFlags)) {

		
		auto modeBtn = [&](GLuint icon, const char* id, ImGuizmo::OPERATION op, bool last = false) {
			bool active = (gizmoOp == op);
			if (active)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			ImVec4 tint = active ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f)    // 激活：白色/原色
				: ImVec4(0.5f, 0.5f, 0.5f, 1.0f);    // 未激活：变暗

			if (ImGui::ImageButton(id, (ImTextureID)(intptr_t)icon, ImVec2(32, 32),
				ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), tint))
				gizmoOp = op;
			
			//if (!last) ImGui::Dummy(ImVec2(0, 4.0f));

			ImGui::PopStyleColor();
			//ImGui::SameLine();
		};

		modeBtn(m_IconView, "##view", GIZMO_NONE);
		modeBtn(m_IconTransform, "##t", ImGuizmo::TRANSLATE);
		modeBtn(m_IconRotate, "##r", ImGuizmo::ROTATE);
		modeBtn(m_IconScale, "##s", ImGuizmo::SCALE, true);

		//modeBtn("Rotate##r", ImGuizmo::ROTATE);
		//modeBtn("Scale##s", ImGuizmo::SCALE);


		/*const char* projLabel = camera.IsOrtho() ? "Ortho" : "Persp";
		if (ImGui::Button(projLabel)) camera.SetOrtho(!camera.IsOrtho());


		if (!camera.IsOrtho()) {
			ImGui::SameLine();
			float fov = camera.GetFov();
			ImGui::SetNextItemWidth(140.0f);
			if (ImGui::SliderFloat("FOV", &fov, 10.0f, 170.0f))
				camera.SetFov(fov);
		}*/

	}
	ImGui::PopStyleVar(3);
	ImGui::End();
}


void Editor::OnImGuiLight(glm::vec3& ambient, float& intensity, glm::vec3& lightColor, bool& lightOn)
{
	ImGui::Begin("Panel");

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Light On", &lightOn);

		if (lightOn) {
			ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
			ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
			ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 300.0f);
		}
	}
	ImGui::End();
}


void Editor::OnImGuiScene(Scene& scene, bool& wireframe) {
	ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_NoCollapse);

	if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("Draw Mode");
		if (ImGui::Checkbox("Wireframe", &wireframe)) {
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
		}
		
		ImGui::Separator();

		ImGui::SeparatorText("Hierarchy");
		for (auto& entityPtr : scene.GetEntities()) {
			Entity* e = entityPtr.get();
			Entity* sel = scene.GetSelected();
			bool isSelected = sel && (sel->GetID() == e->GetID());
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen |
				(isSelected ? ImGuiTreeNodeFlags_Selected : 0);
			ImGui::TreeNodeEx((void*)(intptr_t)e->GetID(), flags, "%s", e->GetName().c_str());
			if (ImGui::IsItemClicked())
				scene.SetSelected(e->GetID());
		}
	}

	if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
		Entity* sel = scene.GetSelected();
		if (!sel) {
			ImGui::TextDisabled("No entity selected.");
		}
		else {
			ImGui::Text("%s", sel->GetName().c_str());
			ImGui::Separator();
			Transform& t = sel->GetTransform();
			t.SyncFromMatrix();
			bool changed = false;
			changed |= ImGui::DragFloat3("Position", &t.position.x, 0.01f);
			changed |= ImGui::DragFloat3("Rotation", &t.rotation.x, 0.5f);
			changed |= ImGui::DragFloat3("Scale", &t.scale.x, 0.01f, 0.001f);
			if (changed)
				t.SyncToMatrix();
		}
	}

	ImGui::End();
}

void Editor::OnImGuiPBR(float& roughness, float& metallic) {
	ImGui::Begin("Panel");

	if (ImGui::CollapsingHeader("PBR", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("PBR roughness", &roughness, 0.1f, 1.0f);
		ImGui::SliderFloat("PBR metallic", &metallic, 0.0f, 1.0f);
	}
	ImGui::End();
}