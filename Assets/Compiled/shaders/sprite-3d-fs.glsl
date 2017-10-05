precision highp float;

uniform sampler2D spriteSheetSampler;

varying vec2 texCoord;

void main(void) 
{
    vec4 spriteSheetSample = texture2D(spriteSheetSampler, texCoord);
	gl_FragColor = spriteSheetSample;
}