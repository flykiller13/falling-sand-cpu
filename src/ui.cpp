#include "falling-sand/ui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"
#include "falling-sand/simulation/simulation.h"

#include <unistd.h>

UI::UI() : paused(false), window_(nullptr) {
}

void UI::init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  window_ = window;
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 330");

}

void UI::update(Simulation &sim) {
  // UI
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Simulation Controls", &show_window);

  // Brush
  ImGui::SeparatorText("Brush");
  int current_brush_type = static_cast<int>(brush_type);
  if (ImGui::Combo("Brush Material", &current_brush_type, brush_types,
                   IM_ARRAYSIZE(brush_types))) {
    brush_type = static_cast<CellType>(current_brush_type);
  }
  ImGui::SliderInt("Brush Size", &brush_size, 1, 10);

  // Draw brush outline
  if (!ImGui::GetIO().WantCaptureMouse) {
    // Dont draw over imgui window
    ImDrawList *draw_list = ImGui::GetForegroundDrawList();
    ImVec2 pos = ImGui::GetMousePos();
    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    float outline_radius = (brush_size + 1) * (
                             window_width / sim.get_grid_width());
    draw_list->AddCircle(pos, outline_radius,
                         IM_COL32(255, 255, 255, 255));
  }

  // Simulation
  ImGui::SeparatorText("Simulation");
  // Stats
  ImGui::Text("Frame Time: %f fps, %.3f ms", ImGui::GetIO().Framerate,
              1000.0f / ImGui::GetIO().Framerate);
  ImGui::Text("Delta Time: %.3f ms", ImGui::GetIO().DeltaTime * 1000.0f);
  ImGui::Text("Grid: %d x %d", sim.get_grid_width(), sim.get_grid_height());
  ImGui::Text("Active Particles: %d", sim.get_active_cell_count());

  if (ImGui::Button(paused ? "Resume" : "Pause"))
    paused = !paused;
  ImGui::SameLine();
  // Clear simulation
  if (ImGui::Button("Clear"))
    sim.clear();

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
  // Terminate ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}