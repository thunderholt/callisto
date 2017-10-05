precision mediump float;

uniform sampler2D diffuseSampler;

varying vec2 fragmentMaterialTexCoord;

void main()
{
	vec4 diffuseSample = texture2D(diffuseSampler, fragmentMaterialTexCoord);
	gl_FragColor = diffuseSample;
}