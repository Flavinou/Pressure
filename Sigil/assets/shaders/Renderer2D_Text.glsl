// Renderer2D - MSDF Text Shader

#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput v_Output;
layout (location = 2) out flat int v_EntityID;

void main()
{
    v_Output.Color = a_Color;
    v_Output.TexCoord = a_TexCoord;
    v_EntityID = a_EntityID;

    gl_Position = ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput v_Input;
layout (location = 2) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_FontAtlasTexture;

float screenPxRange() {
	const float pxRange = 4.0; // set to distance field's pixel range
	vec2 unitRange = vec2(pxRange) / vec2(textureSize(u_FontAtlasTexture, 0));
	vec2 screenTextSize = vec2(1.0) / fwidth(v_Input.TexCoord);
	return max(0.5 * dot(unitRange, screenTextSize), 1.0);
}

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec3 msd = texture(u_FontAtlasTexture, v_Input.TexCoord).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange() * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
	o_Color = mix(bgColor, v_Input.Color, opacity);

	if (o_Color.a == 0.0)
		discard;

	o_EntityID = v_EntityID;
}
