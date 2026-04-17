#version 450 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

layout (std140) uniform Matrices {
	mat4 view;
	mat4 proj;
};

const float magnitude = 0.4;

void GenLine(int idx) {
	gl_Position = proj * gl_in[idx].gl_Position;
	EmitVertex();
	gl_Position = proj * (gl_in[idx].gl_Position + (magnitude * vec4(gs_in[idx].normal, 0.0)));
	EmitVertex();
	EndPrimitive();
}

void main() {
	GenLine(0);
	GenLine(1);
	GenLine(2);
}
