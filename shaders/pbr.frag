#version 330 core

// material
struct Material {
    sampler2D albedo1;
    sampler2D roughness1;
    sampler2D normal1;
}; 

uniform vec3 objectColor;

uniform Material material;

// light
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

in mat3 TBN;

uniform vec3 lightPos;
uniform vec3 viewPos;



void main()
{
    vec3 albedoColor = vec3(texture(material.albedo1, TexCoords));
    vec3 normal = texture(material.normal1, TexCoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);

    float roughness = texture(material.roughness1, TexCoords).g;
    float shininess = mix(256.0, 2.0, roughness);

    vec3 lightDir = normalize(lightPos - FragPos);


    vec3 ambient = light.ambient * albedoColor;

    vec3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0) * albedoColor;

    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), shininess) * albedoColor;

    float distance = length(lightPos - FragPos);
    
    float constant  = 1.0;
    float linear    = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    diffuse  *= attenuation;
    specular *= attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}