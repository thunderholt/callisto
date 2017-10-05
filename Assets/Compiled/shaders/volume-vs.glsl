precision mediump float;

attribute vec3 position;

uniform mat4 worldTransform;
uniform mat4 viewProjTransform;

void main()
{
	gl_Position = viewProjTransform * worldTransform * vec4(position, 1.0);
}