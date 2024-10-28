/*
 * Computer Graphics Assignment1
 * @author: iwaxi_dy
 * main code part
*/
#include "header.h"

const unsigned WND_WIDTH  = 1200;
const unsigned WND_HEIGHT = 900;

// variables
float deltaFrame = 0.0f, lastFrame = 0.0f, currentFrame = 0.0f;  // deltaFrame
float lastPosX = WND_WIDTH / 2.0f, lastPosY = WND_HEIGHT / 2.0f; // mouse position
bool firstMouse = true, isMouseRelase = false;                   // mouse state
bool isMouseLeft = false, isMouseRight = false;
float keySensitivity = 4.0f;                                     // sensitivity

glm::vec3 displacement = glm::vec3(0.0f, 0.0f, 0.0f);            // model matrix parameters
glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
float rotate = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // camera

// call back functions
void error_callback(int error_code, const char* description);
void frameBuffer_callback(GLFWwindow* window, int width, int height);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scoll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

int main() {
    // error message
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    GLFWwindow* window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, "LearnOpenGL: camera test", nullptr, nullptr);

    if (!window) {
        std::cout << "Failed to create glfw window!\n";
        glfwTerminate();
        return -1;
    }

    // capture mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set callback functions
    glfwSetFramebufferSizeCallback(window, frameBuffer_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scoll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";
        return -1;
    }

    // Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // stbi flip y-axis
    // stbi_set_flip_vertically_on_load(true); // here, the texture is upside down
    
    // shader
    Shader shader("model.vs", "model.fs");

    // model
    Model ourModel(FileSystem::getPath("resource/model/creeper/Creeper.obj"));

    // imgui implementation
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // enable gamepad controls

    // setup gui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();
        ImGui::Text("OpenGL Config");
        ImGui::SliderFloat("rotate_angle", &rotate, -120.0f, 120.0f);
        ImGui::SliderFloat3("displacement", &displacement.x, -5.0f, 5.0f);
        ImGui::SliderFloat("scale", &scale.x, -2.0f, 2.0f);
        ImGui::SliderFloat("FOV", &camera.fov_zoom, 1.0f, 60.0f);
        ImGui::SliderFloat("PICTH", &camera.pitch, -89.0f, 89.0f);
        ImGui::SliderFloat("YAW", &camera.yaw, -180.0f, 180.0f);
        ImGui::SliderFloat3("camera_position", &camera.position.x, -5.0f, 5.0f);

        processInput(window);

        currentFrame = glfwGetTime();
        deltaFrame = currentFrame - lastFrame;
        lastFrame = currentFrame;

        shader.use();

        glm::mat4 model(1.0f);
        model = glm::translate(model, displacement);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::rotate(model, glm::radians((float)glfwGetTime() * 20.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        auto view = camera.getViewMatrix();
        auto projection = glm::perspective(glm::radians(camera.fov_zoom), (float) WND_WIDTH / WND_HEIGHT, 0.1f, 100.0f);

        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        ourModel.Draw(shader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

void error_callback(int error_code, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error_code, description);
}

void frameBuffer_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) { // smoothly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        displacement.z += deltaFrame * keySensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        displacement.z -= deltaFrame * keySensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        displacement.x -= deltaFrame * keySensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        displacement.x += deltaFrame * keySensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        rotate += deltaFrame * 50.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        rotate -= deltaFrame * 50.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        float offset = deltaFrame * 0.5f; 
        scale += glm::vec3(offset, offset, offset);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        float offset = deltaFrame * 0.5f; 
        scale -= glm::vec3(offset, offset, offset);
    }
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        static bool isFullScreen = false;
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        if (!isFullScreen) {
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, 100, 100, WND_WIDTH, WND_HEIGHT, 0);
        }
        isFullScreen = !isFullScreen;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isMouseRight){
        if (firstMouse) {
            lastPosX = xpos;
            lastPosY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastPosX;
        float yoffset = lastPosY - ypos;
        camera.processMouseMovement(xoffset, yoffset);
    } else if (isMouseLeft) {
        if (firstMouse) {
            lastPosX = xpos;
            lastPosY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastPosX;
        float yoffset = lastPosY - ypos;
        camera.processCameraPosition(xoffset, yoffset);
    }
    lastPosX = xpos;
    lastPosY = ypos;
}

void mouse_scoll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods) {
    if (!isMouseRelase) {
        if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS){
            isMouseLeft = true;
        } 
        if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE){
            isMouseLeft = false;
        } 
        if (key == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS){
            isMouseRight = true;
        } 
        if (key == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE){
            isMouseRight = false;
        }
    }
    if (key == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
        isMouseRelase = !isMouseRelase;
        if (isMouseRelase) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}