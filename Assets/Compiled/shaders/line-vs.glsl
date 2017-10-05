attribute vec3 position;

uniform mat4 viewProjTransform;
uniform vec3 translation;
uniform vec3 scale;

void main(void) 
{
	vec3 temp = position * scale + translation;

    gl_Position = viewProjTransform * vec4(temp, 1.0);
}