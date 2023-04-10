#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoord;
out vec3 FragCoord;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform float useNormalMap;
uniform mat4 lightSpaceMatrix;

void main()
{
	TexCoord = aTexCoord;

	// on-off by key 1 (useNormalMap).
    // if model does not have a normal map, this should be always 0.
    // if useNormalMap is 0, we use a geometric normal as a surface normal.
    // if useNormalMap is 1, we use a geometric normal altered by normal map as a surface normal.
	if (useNormalMap > 0.5){
		mat3 normalMatrix = transpose(inverse(mat3(world)));
		vec3 T = normalize(normalMatrix * aTangent);
		vec3 N = normalize(normalMatrix * aNormal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		
		TBN = transpose(mat3(T, B, N));    
	}

	FragCoord = vec3(world * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(world))) * aNormal;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragCoord, 1.0);
	
	gl_Position = projection * view * world * vec4(aPos, 1.0f);
}
