#version 430

out vec4 FragColor;

uniform sampler2D uColorSampler;
uniform vec2	  uScreenSize;

vec2 calculateTexCoord()
{
    return gl_FragCoord.xy / uScreenSize;
}

void main()
{
	vec2 TexCoord = calculateTexCoord();
	FragColor = texture(uColorSampler, TexCoord);
}