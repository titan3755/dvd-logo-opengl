#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D textureOne;
uniform vec3 colorModifier;
void main()
{
	vec4 texCl = texture(textureOne, TexCoord);
	FragColor = texCl * vec4(colorModifier, 1.0);
}