#include "Matrix.h"
#include <iostream>
#include <vector>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

// Global state for the application
struct AppState {
    Matrix matrixA{1, 1};
    Matrix matrixB{1, 1};
    int matrixARows = 2, matrixACols = 2;
    int matrixBRows = 2, matrixBCols = 2;
    int selectedFeature = 0;
    int inputMode = 0; // 0 = manual, 1 = CSV
    std::string csvFilename = "";
    std::string statusMessage = "";
    bool showResult = false;
    std::string resultText = "";
};

void initializeMatrices(AppState& state) {
    std::vector<std::vector<double>> dataA(state.matrixARows, std::vector<double>(state.matrixACols, 1.0));
    std::vector<std::vector<double>> dataB(state.matrixBRows, std::vector<double>(state.matrixBCols, 1.0));
    state.matrixA = Matrix(dataA);
    state.matrixB = Matrix(dataB);
}

int main() {
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "Linear Algebra Toolkit", NULL, NULL);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    AppState state;
    initializeMatrices(state);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Linear Algebra Toolkit", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::Text("Linear Algebra Toolkit - Matrix Operations");
        ImGui::Separator();

        // Left panel: Input section
        ImGui::BeginChild("Input", ImVec2(300, 0), true);
        ImGui::TextUnformatted("Matrix Input");
        ImGui::Separator();

        ImGui::RadioButton("Manual Input##input", &state.inputMode, 0);
        ImGui::RadioButton("CSV File##input", &state.inputMode, 1);

        if (state.inputMode == 0) {
            ImGui::Text("Matrix A Dimensions:");
            ImGui::SliderInt("Rows##A", &state.matrixARows, 1, 10);
            ImGui::SliderInt("Cols##A", &state.matrixACols, 1, 10);

            static std::vector<float> matrixAData;
            if (matrixAData.size() != state.matrixARows * state.matrixACols)
                matrixAData.resize(state.matrixARows * state.matrixACols, 1.0f);

            ImGui::Text("Matrix A Values:");
            for (int i = 0; i < state.matrixARows; ++i) {
                for (int j = 0; j < state.matrixACols; ++j) {
                    ImGui::PushID(i * state.matrixACols + j);
                    ImGui::InputFloat("##matA", &matrixAData[i * state.matrixACols + j]);
                    ImGui::PopID();
                    if (j < state.matrixACols - 1) ImGui::SameLine();
                }
            }

            if (ImGui::Button("Load Matrix A", ImVec2(-1, 0))) {
                std::vector<std::vector<double>> data(state.matrixARows, std::vector<double>(state.matrixACols));
                for (int i = 0; i < state.matrixARows; ++i)
                    for (int j = 0; j < state.matrixACols; ++j)
                        data[i][j] = matrixAData[i * state.matrixACols + j];
                state.matrixA = Matrix(data);
                state.statusMessage = "Matrix A loaded successfully";
            }

            ImGui::Spacing();
            ImGui::Text("Matrix B Dimensions:");
            ImGui::SliderInt("Rows##B", &state.matrixBRows, 1, 10);
            ImGui::SliderInt("Cols##B", &state.matrixBCols, 1, 10);

            static std::vector<float> matrixBData;
            if (matrixBData.size() != state.matrixBRows * state.matrixBCols)
                matrixBData.resize(state.matrixBRows * state.matrixBCols, 1.0f);

            ImGui::Text("Matrix B Values:");
            for (int i = 0; i < state.matrixBRows; ++i) {
                for (int j = 0; j < state.matrixBCols; ++j) {
                    ImGui::PushID(100 + i * state.matrixBCols + j);
                    ImGui::InputFloat("##matB", &matrixBData[i * state.matrixBCols + j]);
                    ImGui::PopID();
                    if (j < state.matrixBCols - 1) ImGui::SameLine();
                }
            }

            if (ImGui::Button("Load Matrix B", ImVec2(-1, 0))) {
                std::vector<std::vector<double>> data(state.matrixBRows, std::vector<double>(state.matrixBCols));
                for (int i = 0; i < state.matrixBRows; ++i)
                    for (int j = 0; j < state.matrixBCols; ++j)
                        data[i][j] = matrixBData[i * state.matrixBCols + j];
                state.matrixB = Matrix(data);
                state.statusMessage = "Matrix B loaded successfully";
            }
        } else {
            ImGui::Text("CSV Filename for A:");
            static char csvFileA[256] = "";
            ImGui::InputText("##csvA", csvFileA, sizeof(csvFileA));
            if (ImGui::Button("Load A from CSV", ImVec2(-1, 0))) {
                try {
                    state.matrixA = Matrix::fromCSV(csvFileA);
                    state.statusMessage = "Matrix A loaded from CSV";
                } catch (const std::exception& e) {
                    state.statusMessage = std::string("Error: ") + e.what();
                }
            }

            ImGui::Spacing();
            ImGui::Text("CSV Filename for B:");
            static char csvFileB[256] = "";
            ImGui::InputText("##csvB", csvFileB, sizeof(csvFileB));
            if (ImGui::Button("Load B from CSV", ImVec2(-1, 0))) {
                try {
                    state.matrixB = Matrix::fromCSV(csvFileB);
                    state.statusMessage = "Matrix B loaded from CSV";
                } catch (const std::exception& e) {
                    state.statusMessage = std::string("Error: ") + e.what();
                }
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();

        // Right panel: Operations section
        ImGui::BeginChild("Operations", ImVec2(0, 0), true);
        ImGui::TextUnformatted("Operations");
        ImGui::Separator();

        if (ImGui::Button("Addition (A + B)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA + state.matrixB;
                result.print();
                state.statusMessage = "Addition computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Subtraction (A - B)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA - state.matrixB;
                result.print();
                state.statusMessage = "Subtraction computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Multiplication (A * B)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA * state.matrixB;
                result.print();
                state.statusMessage = "Multiplication computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("RREF (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.rref();
                result.print();
                state.statusMessage = "RREF computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("REF (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.ref();
                result.print();
                state.statusMessage = "REF computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Determinant (A)", ImVec2(-1, 0))) {
            try {
                double det = state.matrixA.determinant();
                state.statusMessage = "Determinant: " + std::to_string(det);
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Rank (A)", ImVec2(-1, 0))) {
            try {
                int rank = state.matrixA.rank();
                state.statusMessage = "Rank: " + std::to_string(rank);
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Linear Independence (A)", ImVec2(-1, 0))) {
            try {
                bool indep = state.matrixA.isLinearlyIndependent();
                state.statusMessage = indep ? "Columns are linearly independent" : "Columns are linearly dependent";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Basis (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.basis();
                result.print();
                state.statusMessage = "Basis computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Eigenvalues (A)", ImVec2(-1, 0))) {
            try {
                auto eigvals = state.matrixA.eigenvalues();
                std::string msg = "Eigenvalues: ";
                for (double v : eigvals) msg += std::to_string(v) + " ";
                state.statusMessage = msg;
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Eigenvectors (A)", ImVec2(-1, 0))) {
            try {
                auto eigvecs = state.matrixA.eigenvectors();
                state.statusMessage = "Eigenvectors computed (check console)";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Inverse (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.inverse();
                result.print();
                state.statusMessage = "Inverse computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Orthogonal/Orthonormal (A)", ImVec2(-1, 0))) {
            try {
                bool ortho = state.matrixA.isOrthogonal();
                bool orthonorm = state.matrixA.isOrthonormal();
                state.statusMessage = (ortho ? "Orthogonal: Yes" : "Orthogonal: No") + 
                                     std::string(orthonorm ? ", Orthonormal: Yes" : ", Orthonormal: No");
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Gram-Schmidt (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.gramSchmidt();
                result.print();
                state.statusMessage = "Gram-Schmidt computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Diagonalization (A)", ImVec2(-1, 0))) {
            try {
                Matrix P(state.matrixA.data.rows(), state.matrixA.data.cols());
                Matrix D = state.matrixA.diagonalize(P);
                D.print();
                P.print();
                state.statusMessage = "Diagonalization computed successfully (check console)";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("SVD (A)", ImVec2(-1, 0))) {
            try {
                auto [U, S, V] = state.matrixA.svd();
                U.print();
                S.print();
                V.print();
                state.statusMessage = "SVD computed successfully (check console)";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        if (ImGui::Button("Transpose (A)", ImVec2(-1, 0))) {
            try {
                Matrix result = state.matrixA.transpose();
                result.print();
                state.statusMessage = "Transpose computed successfully";
                state.showResult = true;
            } catch (const std::exception& e) {
                state.statusMessage = std::string("Error: ") + e.what();
            }
        }

        ImGui::EndChild();

        ImGui::Separator();

        // Status message
        if (!state.statusMessage.empty()) {
            ImGui::TextWrapped("Status: %s", state.statusMessage.c_str());
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
