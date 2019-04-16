#version 440 core

layout (triangles) in;

layout(line_strip, max_vertices=8) out;

uniform float uNormalLength;
uniform mat4 uModelViewProjectionMatrix;

uniform vec4 uVertexNormalColor;
uniform vec4 uFaceNormalColor;

uniform bool uVertexNormalE;
uniform bool uFaceNormalE;

out vec4 vertex_color;

in Vertex {
	vec3 normal;
} vertex[];

void main() {
	if (uVertexNormalE) {
		for (int i = 0; i < gl_in.length(); i++) {
			vec3 P = gl_in[i].gl_Position.xyz;
			vec3 N = vertex[i].normal.xyz;

			gl_Position = uModelViewProjectionMatrix * vec4(P, 1.0);
			vertex_color = uVertexNormalColor;
			EmitVertex();

			gl_Position = uModelViewProjectionMatrix * vec4(P + N * (uNormalLength / 2.0f), 1.0);
			vertex_color = uVertexNormalColor;
			EmitVertex();

			EndPrimitive();
		}
	}

	if (uFaceNormalE) {
		vec3 P0 = gl_in[0].gl_Position.xyz;
		vec3 P1 = gl_in[1].gl_Position.xyz;
		vec3 P2 = gl_in[2].gl_Position.xyz;
  
		vec3 V0 = P0 - P1;
		vec3 V1 = P2 - P1;
  
		vec3 N = cross(V1, V0);
		N = normalize(N);

		vec3 P = (P0+P1+P2) / 3.0;
  
		gl_Position = uModelViewProjectionMatrix * vec4(P, 1.0);
		vertex_color = uFaceNormalColor;
		EmitVertex();
  
		gl_Position = uModelViewProjectionMatrix * vec4(P + N * uNormalLength, 1.0);
		vertex_color = uFaceNormalColor;
		EmitVertex();
		EndPrimitive();
	}
}