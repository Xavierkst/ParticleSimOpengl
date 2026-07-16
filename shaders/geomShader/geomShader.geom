#version 450 core

// Example layout of gl_in:
// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[]

layout (points) in;
layout (triangle_strip, max_vertices = 10) out;

in VS_OUT {
	vec3 color;
} gs_in[];

out GS_OUT {
	vec3 gColor;
}gs_out;

void build_house(vec4 position) {
	// only 1 input vertex (elem 0) since we're drawing point by point
	gs_out.gColor = gs_in[0].color; 
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
	// The color gets interpolated with the roof vertex being the most white / snowy 
	gs_out.gColor = vec3(1.0, 1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}

void main() {
	// note: we access elem 0 of gl_in[] array. Its an array bec. 
	// geometry shader receives all vertices of a primitive as its input
	build_house(gl_in[0].gl_Position);
	// gs_out.gColor = gs_in[0].color;
}