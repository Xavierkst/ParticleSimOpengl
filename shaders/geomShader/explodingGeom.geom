#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	in vec2 texCoords;
} gs_in[];

out GS_OUT {
	vec2 texCoords;
} gs_out;

uniform float time;

layout (std140) uniform Matrices {
 	mat4 view;
 	mat4 proj;
 };


// get the 3 vertices of a given triangle
vec3 getNormal() {
	vec4 v1 = gl_in[0].gl_Position;
	vec4 v2 = gl_in[1].gl_Position;
	vec4 v3 = gl_in[2].gl_Position;
	
	vec4 a = v2 - v1;
	vec4 b = v3 - v1;
	// note the order of the cross prod
	return normalize(cross(vec3(a), vec3(b)));
}

vec4 explode(vec4 position, vec3 normal) {
	// translate the pos vertex in the direction of the normal
	float magnitude = 2.0;
	vec3 dir = magnitude * normal * ((sin(time) + 1.0) / 2.0);
	return position + vec4(dir, 0.0);
}

void main() {
	vec3 norm = getNormal();
	gl_Position = proj * explode(gl_in[0].gl_Position, norm);
	gs_out.texCoords = gs_in[0].texCoords; 
	EmitVertex();
	gl_Position = proj * explode(gl_in[1].gl_Position, norm);
	gs_out.texCoords = gs_in[1].texCoords; 
	EmitVertex();
	gl_Position = proj * explode(gl_in[2].gl_Position, norm);
	gs_out.texCoords = gs_in[2].texCoords; 
	EmitVertex();
	EndPrimitive();
}