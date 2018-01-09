precision mediump float;

uniform sampler2D diffuseSampler;
uniform sampler2D lightAtlasSampler;

varying vec2 fragmentMaterialTexCoord;
varying vec2 fragmentLightAtlasTexCoord;

void main()
{
	vec4 diffuseSample = texture2D(diffuseSampler, fragmentMaterialTexCoord);

	vec4 lightAtlasSample = texture2D(lightAtlasSampler, fragmentLightAtlasTexCoord);

	gl_FragColor = diffuseSample * (lightAtlasSample * 2.0);
}