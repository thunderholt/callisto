precision mediump float;

attribute vec2 sizeMultiplier;

uniform vec2 position;
uniform vec2 size;
uniform vec2 viewportSize;
/*uniform vec2 xAxis;
uniform vec2 yAxis;*/
uniform vec2 spritePosition;
uniform vec2 spriteSize;
uniform vec2 spriteSheetSize;

varying vec2 texCoord;

void main(void) 
{
	vec2 screenSpacePosition = 
		position + (size * sizeMultiplier);

	vec2 normalisedScreenSpacePosition =
		screenSpacePosition / viewportSize;

	/*screenSpacePosition =
		screenSpacePosition - position - (size / 2.0);

	screenSpacePosition =
		((xAxis * screenSpacePosition.x) + (yAxis * screenSpacePosition.y));

	screenSpacePosition =
		guiSpacePosition + position + (size / 2.0);*/


	vec2 clipSpacePosition = 
		vec2(-1, 1) + 
		((normalisedScreenSpacePosition * vec2(1.0, -1.0) * 2.0));

	texCoord = (spritePosition + (spriteSize * sizeMultiplier)) /* vec2(1.0, -1.0)*/ / spriteSheetSize;

	gl_Position = vec4(clipSpacePosition, 0.0, 1.0);
}