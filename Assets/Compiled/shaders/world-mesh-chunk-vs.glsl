precision mediump float;

attribute vec3 position;
attribute vec2 materialTexCoord;

uniform mat4 viewProjTransform;
uniform vec2 lightIslandOffset;
uniform vec2 lightIslandSize;

varying vec2 fragmentMaterialTexCoord;
varying vec2 fragmentLightAtlasTexCoord;

void main()
{
	fragmentMaterialTexCoord = materialTexCoord;

	fragmentLightAtlasTexCoord = 
		lightIslandOffset + materialTexCoord * lightIslandSize;

	gl_Position = viewProjTransform * vec4(position, 1.0);
}