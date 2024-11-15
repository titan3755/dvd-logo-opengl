#include <application.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Application::Application(int width, int height, const char* title)
{
    this->window = nullptr;
    this->width = width;
    this->height = height;
    this->title = title;
    this->isRunning = true;
}

Application::~Application()
{
    glfwTerminate();
}

void Application::fpsCalculate()
{
    static double previousSeconds = glfwGetTime();
    static int frameCount;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime();

    elapsedSeconds = currentSeconds - previousSeconds;

    if (elapsedSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        char tmp[128];
        sprintf_s(tmp, "%s @ fps: %.2f, ms/frame: %.2f", title, fps, msPerFrame);
        glfwSetWindowTitle(window, tmp);

        frameCount = 0;
    }

    frameCount++;
}

void Application::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        isRunning = false;
    }
}

void Application::printOpenGLInfo()
{
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    const GLubyte* vendor = glGetString(GL_VENDOR);

    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;
    std::cout << "GLSL version: " << glslVersion << std::endl;
    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "Reached application loop..." << std::endl;
    std::cout << "OpenGL initialized successfully!" << std::endl;
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::run()
{
    std::cout << "Running application and initializing OpenGL..." << std::endl;

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ------------------------------------------------------------------------------->>
    // shader manager implementation
    std::cout << "Creating shader program..." << std::endl;
    bool success = true;
    ShaderManager shaderManager;
    success = shaderManager.createShaderProgram();
    if (!success)
    {
        std::cerr << "Failed to create shader program!" << std::endl;
        return;
    }
    success = shaderManager.compileShaders();
    if (!success)
    {
        std::cerr << "Failed to compile shaders!" << std::endl;
        return;
    }
    std::cout << "Shader program created successfully!" << std::endl;
    // ------------------------------------------------------------------------------->>

    float vertices[] = {
        // positions          // colors           // texture coords
        0.15f,  0.15f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // top right
        0.15f, -0.15f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
       -0.15f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
       -0.15f, 0.15f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("C:\\Users\\mahmu\\Desktop\\codez\\vs\\cpp\\dvd-logo-opengl\\dvd-logo-opengl\\src\\assets\\dvd-logo.png", &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Failed to load texture!" << std::endl;
        return;
    }

    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
	else format = GL_RGB;

	std::cout << "Format: " << format << " :: nrChan: " << nrChannels << std::endl; // "Format: 6407    

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    printOpenGLInfo();

	float positionModifier[] = { 0.0f, 0.0f, 0.0f };
	float colorModifier[] = { 0.0f, 0.0f, 0.0f };

    shaderManager.useShaderProgram();
    glUniform1i(glGetUniformLocation(shaderManager.getShaderProgram(), "textureOne"), 0);
	glUniform3fv(glGetUniformLocation(shaderManager.getShaderProgram(), "positionModifier"), 1, positionModifier);
	glUniform3fv(glGetUniformLocation(shaderManager.getShaderProgram(), "colorModifier"), 1, colorModifier);

    while (!glfwWindowShouldClose(window) && isRunning)
    {
		positionModifier[0] += 0.0001f;
		positionModifier[1] += 0.0001f;
		if (positionModifier[0] > 0.5f)
			positionModifier[0] = -0.5f;
		if (positionModifier[1] > 0.5f)
			positionModifier[1] = -0.5f;
        fpsCalculate();
        processInput();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderManager.useShaderProgram();
		glUniform3fv(glGetUniformLocation(shaderManager.getShaderProgram(), "positionModifier"), 1, positionModifier);
		glUniform3fv(glGetUniformLocation(shaderManager.getShaderProgram(), "colorModifier"), 1, colorModifier);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

