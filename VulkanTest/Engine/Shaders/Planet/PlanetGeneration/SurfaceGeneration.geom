#version 450

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 18) out;

layout (location = 0) in vec2 in_texcoord[];
layout (location = 0) out vec2 out_texcoord;

void main()
{
	for(int face = 0 ; face < 6 ; face ++)
	{
		out_texcoord = in_texcoord[0];
		gl_Position  = vec4(in_texcoord[0], 0.0, 1.0);
		gl_Layer	 = face;
		EmitVertex();

		out_texcoord = in_texcoord[1];
		gl_Position  = vec4(in_texcoord[1], 0.0, 1.0);
		gl_Layer	 = face;
		EmitVertex();

		out_texcoord = in_texcoord[2];
		gl_Position  = vec4(in_texcoord[2], 0.0, 1.0);
		gl_Layer	 = face;
		EmitVertex();
	}

	EndPrimitive();
}