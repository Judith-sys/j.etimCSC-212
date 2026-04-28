#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Vertex Shader Source
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec2 TexCoord;
    
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    uniform float time;
    
    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        // Simple pulsing ambient lighting
        float ambient = 0.5 + sin(time * 2.0) * 0.2;
        FragColor = texColor * ambient;
    }
)";

// Function to compile shaders
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

// Function to create shader program
unsigned int createShaderProgram() {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// Function to load texture
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Pyramid - 3D with Texture", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Pyramid vertices: position (x, y, z) and texture coordinates (u, v)
    // The pyramid has 5 vertices: 4 base corners + 1 apex
    float vertices[] = {
        // Front face (triangle)
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // bottom-right
         0.0f,  0.5f,  0.0f,   0.5f, 1.0f,  // apex

         // Right face (triangle)
          0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // bottom-left
          0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // bottom-right
          0.0f,  0.5f,  0.0f,   0.5f, 1.0f,  // apex

          // Back face (triangle)
           0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // bottom-left
          -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // bottom-right
           0.0f,  0.5f,  0.0f,   0.5f, 1.0f,  // apex

           // Left face (triangle)
           -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // bottom-left
           -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // bottom-right
            0.0f,  0.5f,  0.0f,   0.5f, 1.0f,  // apex

            // Base (two triangles forming a square)
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // back-left
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // back-right
             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // front-right

             0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // front-right
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,  // front-left
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f   // back-left
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load texture
    // Create a procedural texture since we can't guarantee an image file
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Create a stone-like procedural texture
    int texWidth = 256, texHeight = 256;
    unsigned char* texData = new unsigned char[texWidth * texHeight * 3];

    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            int index = (y * texWidth + x) * 3;
            // Create noise pattern for stone texture
            float noise1 = sin(x * 0.05f) * cos(y * 0.05f);
            float noise2 = sin(x * 0.1f + 1.0f) * cos(y * 0.12f + 2.0f);
            float noise3 = sin((x * 0.2f + y * 0.15f)) * 0.5f + 0.5f;

            unsigned char value = (unsigned char)(80 + noise1 * 40 + noise2 * 30 + noise3 * 30);
            texData[index + 0] = value;      // R
            texData[index + 1] = value - 20; // G
            texData[index + 2] = value - 40; // B (stone has cool tones)
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] texData;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int shaderProgram = createShaderProgram();

    // Setup matrices
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    double lastTime = glfwGetTime();
    float angle = 0.0f;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        angle += 50.0f * deltaTime; // Rotate 50 degrees per second
        if (angle > 360.0f) angle -= 360.0f;

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Model matrix with rotation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Tilt slightly

        // Use shader program
        glUseProgram(shaderProgram);

        // Set uniforms
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        unsigned int timeLoc = glGetUniformLocation(shaderProgram, "time");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(timeLoc, (float)currentTime);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        // Draw pyramid
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 18); // 18 vertices = 6 triangles

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}