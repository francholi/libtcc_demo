#version 440

layout(location = 3) out vec2 vertex_uv_out;

const vec2 quad_pos[4] = { vec2( -1, -1 ), vec2( 1, -1 ), vec2( -1, 1), vec2( 1, 1) };
const vec2 quad_uvs[4] = { vec2(  0,  1 ), vec2( 1,  1 ), vec2(  0, 0), vec2( 1, 0) };

void main() {
    vertex_uv_out = quad_uvs[gl_VertexID];
    gl_Position = vec4(quad_pos[gl_VertexID], 0.0 , 1.0);
}