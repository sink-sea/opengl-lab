#include "header.h"

const unsigned WND_WIDTH  = 800;
const unsigned WND_HEIGHT = 600;

float deltaFrame = 0.0f, lastFrame = 0.0f, currentFrame = 0.0f;
float lastPosX = WND_WIDTH / 2.0f, lastPosY = WND_HEIGHT / 2.0f;
bool firstMouse = true;


bool isMouseLeft = false, isMouseRight = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void error_callback(int error_code, const char* description);
void frameBuffer_callback(GLFWwindow* window, int width, int height);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scoll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";
        return -1;
    }

    // Z-Buffer
    glEnable(GL_DEPTH_TEST);\

    // stbi flip y-axis
    stbi_set_flip_vertically_on_load(true);
    
    // shader
    Shader shader("model.vs", "model.fs");

    // model
    Model ourModel(FileSystem::getPath("resource/model/creeper/creeper.obj"));
    // Model ourModel(FileSystem::getPath("resource//model/backpack/backpack.obj"));
    // Model ourModel(FileSystem::getPath("resource/model/armor/Artorias.obj"));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        currentFrame = glfwGetTime();
        deltaFrame = currentFrame - lastFrame;
        lastFrame = currentFrame;

        shader.use();

        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * 20.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        auto view = camera.getViewMatrix();
        auto projection = glm::perspective(glm::radians(camera.fov_zoom), (float) WND_WIDTH / WND_HEIGHT, 0.1f, 100.0f);

        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void error_callback(int error_code, const char* description) {
    fprintf(stderr, "Error: %d %s\n", error_code, description);
}

void frameBuffer_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
        camera.processKeyboard(CAMERA_MOVEMENT::FORWARD, deltaFrame);
    }
    if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
        camera.processKeyboard(CAMERA_MOVEMENT::BACKWARD, deltaFrame);
    }
    if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
        camera.processKeyboard(CAMERA_MOVEMENT::LEFT, deltaFrame);
    }
    if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
        camera.processKeyboard(CAMERA_MOVEMENT::RIGHT, deltaFrame);
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
    if (isMouseLeft){
        if (firstMouse) {
            lastPosX = xpos;
            lastPosY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastPosX;
        float yoffset = lastPosY - ypos;
        camera.processMouseMovement(xoffset, yoffset);
    }
    lastPosX = xpos;
    lastPosY = ypos;
}

void mouse_scoll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods) {
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
        isMouseLeft = false;
    }
}