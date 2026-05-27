#version 330 core

// material
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 


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

uniform vec3 lightPos;

uniform vec3 objectColor;

uniform vec3 viewPos;


void main()
{

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // ambient light
    vec3 ambient  = light.ambient * material.ambient;
    
    // specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    // diffuse light
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * material.diffuse);

    float distance = length(lightPos - FragPos);
    
    float constant  = 1.0;
    float linear    = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // diffuse  *= attenuation;
    // specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}