precision mediump float;

attribute vec3 position;
attribute vec2 materialTexCoord;

uniform mat4 viewProjTransform;

varying vec2 fragmentMaterialTexCoord;

void main()
{
	fragmentMaterialTexCoord = materialTexCoord;
	gl_Position = viewProjTransform * vec4(position, 1.0);
}