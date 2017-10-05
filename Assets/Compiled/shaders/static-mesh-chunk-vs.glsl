precision mediump float;

attribute vec3 position;
attribute vec2 materialTexCoord;

uniform mat4 worldTransform;
uniform mat4 viewProjTransform;

varying vec2 fragmentMaterialTexCoord;

void main()
{
	fragmentMaterialTexCoord = materialTexCoord;
	gl_Position = viewProjTransform * worldTransform * vec4(position, 1.0);
}