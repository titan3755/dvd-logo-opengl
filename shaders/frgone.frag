#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureOne;
void main()
{
	FragColor = texture(textureOne, TexCoord);
}