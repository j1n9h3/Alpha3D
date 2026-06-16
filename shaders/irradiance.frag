
#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{		
    // the sample direction equals the hemisphere's orientation 
    vec3 normal = normalize(localPos);
    vec3 up = normalize(vec3(0.0, 1, 0));
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    vec3 irradiance = vec3(0.0);
  
    float n_total = 0.0;
    float sampleDelta = 0.025;
    for(float phi = 0.0; phi < PI * 2; phi += sampleDelta){
        for(float theta = 0.0; theta < PI / 2; theta += sampleDelta){
            vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 direction = tangent.x * right + tangent.y * up + tangent.z * normal;
            irradiance += texture(environmentMap, direction).rgb * cos(theta) * sin(theta);
            n_total ++;
        }
    }

    irradiance = irradiance * PI / n_total;
  
    FragColor = vec4(irradiance, 1.0);
}