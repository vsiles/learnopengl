#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Default value is default active texture unit
// so no need to set it if the correct texture is bound
// to GL_TEXTURE0
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 f1 = texture(texture1, TexCoord);
    vec4 f2 = texture(texture2, TexCoord);
    FragColor = mix(f1, f2, 0.2);
}
