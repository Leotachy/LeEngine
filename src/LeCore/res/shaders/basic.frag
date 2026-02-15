


#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixVal;



void main()
{
	vec2 TexCoord2 = vec2(TexCoord.x, TexCoord.y);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord2), mixVal);
	FragColor = FragColor * vec4(ourColor, 1.0);
}
