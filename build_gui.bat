@echo off
REM Download and setup ImGui and GLFW if not present

if not exist ImGui (
    echo Downloading ImGui...
    curl -L -o imgui.zip https://github.com/ocornut/imgui/archive/refs/heads/master.zip
    tar -xf imgui.zip
    rename imgui-master ImGui
    del imgui.zip
)

if not exist GLFW (
    echo Downloading GLFW...
    curl -L -o glfw.zip https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip
    tar -xf glfw.zip
    rename glfw-3.3.8.bin.WIN64 GLFW
    del glfw.zip
)

REM Compile with ImGui and GLFW
echo Compiling Linear Algebra GUI...
g++ -I Eigen3 -I ImGui -I ImGui/backends -I GLFW/include -std=c++17 -o main_gui.exe main_gui.cpp Matrix.cpp ^
    ImGui/imgui.cpp ImGui/imgui_demo.cpp ImGui/imgui_draw.cpp ImGui/imgui_widgets.cpp ImGui/imgui_tables.cpp ^
    ImGui/backends/imgui_impl_glfw.cpp ImGui/backends/imgui_impl_opengl3.cpp ^
    -L GLFW/lib-vc2022 -lglfw3 -lopengl32

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run: main_gui.exe
) else (
    echo Build failed!
)
