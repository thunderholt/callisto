precision mediump float;

attribute vec2 sizeMultiplier;

uniform vec3 position;
uniform vec2 size;
uniform vec2 centerOfRotation;
uniform mat2 rotationTransform;
uniform vec2 spritePosition;
uniform vec2 spriteSize;
uniform vec2 spriteSheetSize;
uniform mat4 viewTransform;
uniform mat4 projTransform;

varying vec2 texCoord;

void main()
{
	// Calculate the position offset.
	vec2 positionOffset = size * sizeMultiplier;
	positionOffset -= vec2(size.x / 2.0, size.y / 2.0); // Center the sprite.
	positionOffset -= centerOfRotation;
	positionOffset = rotationTransform * positionOffset;
	positionOffset += centerOfRotation;

	// Calculate the view-space position.
	vec3 viewSpacePosition = vec3(viewTransform * vec4(position, 1.0));
	viewSpacePosition += vec3(positionOffset, 0.0);

	// Calculate the texture coordinate.
	texCoord = (spritePosition + (spriteSize * sizeMultiplier));
	texCoord.y = spriteSize.y - texCoord.y;
 	texCoord /= spriteSheetSize;

 	// Convert the view-space position to screen-space.
	gl_Position = projTransform * vec4(viewSpacePosition, 1.0);	
}