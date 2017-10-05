precision mediump float;

uniform sampler2D spriteSheetSampler;
uniform vec4 tint;

varying vec2 texCoord;

void main()
{
	vec4 spriteSheetSample = texture2D(spriteSheetSampler, texCoord);
	gl_FragColor = spriteSheetSample * tint;
	//gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}