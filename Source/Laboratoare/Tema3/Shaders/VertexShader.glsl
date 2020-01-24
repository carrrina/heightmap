#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform sampler2D heightMap;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform int type_of_light;
uniform float cut_off_angle;

out vec2 texcoord;
out float light;

void main()
{
	texcoord = v_texture_coord;

	float u = texcoord.x;
	float v = texcoord.y;
	float height = texture2D(heightMap, vec2(u, v)).r * 90;

	//float texelSize = vec2(1.0f / Model.x, 1.0f / Model.y);
	//float heightRight = texture2D(heightMap, vec2(u + texelSize.x, v)).r;
	//float heightUp = texture2D(heightMap, vec2(u, v + texelSize.y)).r;

	//float Hx = height - heightRight;
	//float Hz = height - heightUp;

	//normal = normalize(vec3(Hx, 1, Hz));

	vec3 world_position = (Model * vec4(v_position, 1)).xyz;
	vec3 world_normal = normalize(mat3(Model) * v_normal);

	vec3 N = normalize(world_normal);
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

	float ambient_light = 0.25 * material_kd;
	float diffuse_light = material_kd * max(dot(N, L), 0);
	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light = material_ks * pow(max(dot(normalize(N), H), 0), material_shininess);
	}

	light = 0;

	if(type_of_light == 1) {
		float cut_off = radians(cut_off_angle);
		float spot_light = dot(-L, light_direction);
		float spot_light_limit = cos(cut_off);
		
		if (spot_light > cos(cut_off))
		{
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);
			light = ambient_light + light_att_factor * (diffuse_light + specular_light);  
		}	
		else
			light = ambient_light;
	}
	else {
		light = ambient_light + diffuse_light + specular_light;
	}

	gl_Position = Projection * View * Model * vec4(v_position.x, height, v_position.z, 1.0);
}
