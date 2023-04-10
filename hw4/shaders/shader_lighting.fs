#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
    float shininess;
}; 

struct Light {
    vec3 dir;
    vec3 color; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform float useNormalMap;
uniform float useSpecularMap;
uniform float useShadow;
uniform sampler2D depthMapSampler;
uniform float useLighting;
uniform float usePCF;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMapSampler, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMapSampler, 0);
    if(usePCF > 0.5f){
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(depthMapSampler, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }    
        }
        shadow /= 9.0;
    }
    else shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}


void main()
{
	vec3 color = texture(material.diffuseSampler, TexCoord).rgb;

    // on-off by key 3 (useLighting). 
    // if useLighting is 0, return diffuse value without considering any lighting.(DO NOT CHANGE)
	if (useLighting < 0.5f){
        FragColor = vec4(color, 1.0); 
        return; 
    }

    vec3 ambient = 0.3 * light.color * color;
    vec3 norm = normalize(Normal);

    // on-off by key 2 (useShadow).
    // calculate shadow
    // if useShadow is 0, do not consider shadow.
    // if useShadow is 1, consider shadow.
    float shadow = 0.0f;
    if(useShadow > 0.5f) shadow = ShadowCalculation(FragPosLightSpace);

    // on-off by key 1 (useNormalMap).
    // if model does not have a normal map, this should be always 0.
    // if useNormalMap is 0, we use a geometric normal as a surface normal.
    // if useNormalMap is 1, we use a geometric normal altered by normal map as a surface normal.
	if(useNormalMap > 0.5f)
	{
        norm = texture(material.normalSampler, TexCoord).rgb;
        norm = normalize(norm * 2.0 - 1.0);

        float lightDistance = 15.0f;
        vec3 lightPos = -light.dir * lightDistance;
        vec3 TangentLightPos = TBN * lightPos;
        vec3 TangentViewPos = TBN * viewPos;
        vec3 TangentFragPos = TBN * FragCoord;

        vec3 lightDir = normalize(TangentFragPos- TangentLightPos);
        float diff = max(dot(-lightDir, norm), 0.0);
        vec3 diffuse = light.color * diff * (1.0 - shadow) * color;

        vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
        vec3 reflectDir = reflect(lightDir, norm);
        vec3 halfwayDir = normalize(-lightDir + viewDir);  
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        vec3 specular = light.color * spec * (1.0 - shadow) * texture(material.specularSampler, TexCoord).r;
        FragColor = vec4(ambient + diffuse + specular, 1.0);

        return;
	}

    float diff = max(dot(norm, -light.dir), 0.0);
    vec3 diffuse = light.color * diff * (1.0 - shadow) * color;
	
    // if model does not have a specular map, this should be always 0.
    // if useSpecularMap is 0, ignore specular lighting.
    // if useSpecularMap is 1, calculate specular lighting.
	if(useSpecularMap > 0.5f)
	{
        //use only red channel of specularSampler as a reflectance coefficient(k_s).

        vec3 viewDir = normalize(viewPos - FragCoord);
        vec3 reflectDir = reflect(light.dir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.color * spec * (1.0 - shadow) * texture(material.specularSampler, TexCoord).r;
        
        FragColor = vec4(ambient + diffuse + specular, 1.0);
        return;
	}

    FragColor = vec4(ambient + diffuse, 1.0);

	
}