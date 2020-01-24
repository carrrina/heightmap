#version 330
 
in vec2 texcoord;
in float light;

uniform sampler2D colorMap;

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 color = texture2D(colorMap, texcoord);

	if(color.a < 0.5f)
		discard;

	out_color = color * light;
}