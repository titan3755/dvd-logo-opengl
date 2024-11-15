#include <shader_manager.h>

ShaderManager::ShaderManager()
{
	this->vertexShaderSource = nullptr;
	this->fragmentShaderSource = nullptr;
	this->shaderProgram = 0;
}

ShaderManager::~ShaderManager()
{
	deleteShaderProgram();
}

bool ShaderManager::createShaderProgram()
{
	shaderProgram = glCreateProgram();
	return shaderProgram != 0;
}

const char* ShaderManager::shaderReaderFromFile(const char* shaderPath, const char* shaderType)
{
	std::ifstream shaderFile;
	shaderFile.open(shaderPath);
	if (!shaderFile.is_open())
	{
		std::cerr << "Failed to open file: " << shaderPath << std::endl;
		return nullptr;
	}
	std::string shaderSource;
	std::string line;
	while (std::getline(shaderFile, line))
	{
		shaderSource += line + "\n";
	}
	shaderFile.close();
	if (shaderType == "vertex")
	{
		vertexShaderSource = shaderSource.c_str();
	}
	else if (shaderType == "fragment")
	{
		fragmentShaderSource = shaderSource.c_str();
	}
	return shaderSource.c_str();
}

bool ShaderManager::shaderWriterToFile(const char* shaderPath, const char* shaderSource)
{
	std::ofstream shaderFile;
	shaderFile.open(shaderPath);
	if (!shaderFile.is_open())
	{
		std::cerr << "Failed to open file: " << shaderPath << std::endl;
		return false;
	}
	shaderFile << shaderSource;
	shaderFile.close();
	return true;
}

bool ShaderManager::compileShaders()
{
	const char* vertexShaderSource = shaderReaderFromFile(vertexShaderPath, "vertex");
	const char* fragmentShaderSource = shaderReaderFromFile(fragmentShaderPath, "fragment");

	if (vertexShaderSource == nullptr || fragmentShaderSource == nullptr)
	{
		return false;
	}

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
		return false;
	}

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
		return false;
	}

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: " << infoLog << std::endl;
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void ShaderManager::useShaderProgram()
{
	glUseProgram(shaderProgram);
}

unsigned int ShaderManager::getShaderProgram()
{
	return shaderProgram;
}

void ShaderManager::deleteShaderProgram()
{
	glDeleteProgram(shaderProgram);
}




