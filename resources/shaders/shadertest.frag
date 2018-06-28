#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// Default value is default active texture unit
// so no need to set it if the correct texture is bound
// to GL_TEXTURE0
uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}
