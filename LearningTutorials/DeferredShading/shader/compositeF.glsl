#version 430

out vec4 FragColor;

uniform sampler2D uPointLightSampler;
uniform sampler2D uDirectionalLightSampler;
uniform vec2 uScreenSize;

vec2 calculateTexCoord()
{
    return gl_FragCoord.xy / uScreenSize;
}

void main()
{
	vec2 TexCoord = calculateTexCoord();
	vec4 PointLightColor = texture(uPointLightSampler, TexCoord);
	vec4 DirectionalLightColor = texture(uDirectionalLightSampler, TexCoord);

	FragColor = PointLightColor + DirectionalLightColor;
}