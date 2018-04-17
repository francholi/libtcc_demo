#version 440
// input from Vertex Shader
layout(location = 3) in vec2 output_uv;
// output here
layout(location = 0) out vec4 fragment_color;
// read from this texture

const vec3 eye = vec3(0,0,0);
const vec3[4] corners = { 
    vec3(-5,-5,-1),  // top left
    vec3( 5,-5,-1),  // top right
    vec3(-5, 5,-1),  // bottom left
    vec3( 5, 5,-1),  // bottom right
     
};

float sphere_test(in vec3 rayDir, in vec3 rayOrigin, in vec4 centre_radius);
vec4 shade(vec3 point, vec3 normal, vec3 light, vec3 color);

void main() {
    vec3 ray_ori = vec3(0,0,0);
    vec3 ray_dir = normalize(
        mix(
         mix(corners[0], corners[1],output_uv.x),
         mix(corners[2], corners[3],output_uv.x),
         output_uv.y
        ) - ray_ori);
    
    vec4 sphere0 = vec4(0,0,-75,50); // (center3 , radius1)
    float t = sphere_test(ray_dir, ray_ori, sphere0);

    if (t > 0) {
        vec3 intersect = ray_ori + ray_dir*t;
        vec3 normal = normalize(intersect - sphere0.xyz);
        fragment_color = shade(intersect, normal, vec3(0,0,0), 
                               vec3(1.0,0.8,0.0));
    }
    else
        fragment_color = vec4(0.0,0.0,0.0,1.0);
}

float sphere_test(in vec3 rayDir, in vec3 rayOrigin, in vec4 centre_radius)
{
	vec3 omc = rayOrigin - centre_radius.xyz;
	float b = dot(rayDir,omc);
	float c = dot(omc,omc) - (centre_radius.w*centre_radius.w);
	float tosqroot = b*b - c;
	if (tosqroot > 0.0)
	{
		float root = sqrt(tosqroot);
		return min(-b-root,-b+root);
	}
	return -1;
}

vec4 shade(vec3 point, vec3 normal, vec3 light, vec3 color)
{
    vec3 surface_to_light = normalize(light - point);
    float diff = max(dot(surface_to_light, normal),0.0);
    return vec4(color * diff, 1.0);
}
