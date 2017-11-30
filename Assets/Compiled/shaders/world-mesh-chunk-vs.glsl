precision mediump float;

attribute vec3 position;
attribute vec2 materialTexCoord;
attribute vec2 lightAtlasTexCoord;

uniform mat4 viewProjTransform;
uniform ivec2 lightIslandOffset;
uniform ivec2 lightIslandSize;

varying vec2 fragmentMaterialTexCoord;
varying vec2 fragmentLightAtlasTexCoord;

void main()
{
	fragmentMaterialTexCoord = materialTexCoord;
	fragmentLightAtlasTexCoord = lightAtlasTexCoord;

	//fragmentLightAtlasTexCoord = 
	//	lightIslandOffset + (materialTexCoord * lightIslandSize);
	//fragmentLightAtlasTexCoord.x = 
	//	(lightIslandOffset.x + (materialTexCoord.x * (float)lightIslandSize.x)) / 4096.0; // FIXME
	//fragmentLightAtlasTexCoord.y = 
	//	(lightIslandOffset.y + (materialTexCoord.y * (float)lightIslandSize.y)) / 4096.0; // FIXME

	gl_Position = viewProjTransform * vec4(position, 1.0);
}