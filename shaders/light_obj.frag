#version 330 core

// material
struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
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

uniform vec3 lightPos;
uniform vec3 viewPos;



void main()
{

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // ambient light
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    
    // specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    
    // diffuse light
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));

    float distance = length(lightPos - FragPos);
    
    float constant  = 1.0;
    float linear    = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}