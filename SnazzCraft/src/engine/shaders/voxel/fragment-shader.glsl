#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Brightness;

// Texture Sampler
uniform sampler2D texture1;

vec2 AdjustVoxelTextureCoordinate(inout vec2 TextureCoordinate);

vec4 ApplyBrightness(inout vec4 Color, float Brightness);

void main()
{
    vec2 TextureCoordinate = TexCoord;
    TextureCoordinate = AdjustVoxelTextureCoordinate(TextureCoordinate);

	vec4 texColor = texture(texture1, TextureCoordinate);
    texColor = ApplyBrightness(texColor, Brightness);

    if (texColor.a < 0.1) {
        discard;
    }
       
    FragColor = texColor;
}

vec2 AdjustVoxelTextureCoordinate(inout vec2 TextureCoordinate)
{
    TextureCoordinate.x -= TextureCoordinate.x > 1.0 ? int(TextureCoordinate.x) : 0.0;
    TextureCoordinate.y -= TextureCoordinate.y > 1.0 ? int(TextureCoordinate.y) : 0.0;

    return TextureCoordinate;
}

vec4 ApplyBrightness(inout vec4 Color, float Brightness)
{
    Color[0] *= Brightness;
    Color[1] *= Brightness;
    Color[2] *= Brightness;

    return Color;
}