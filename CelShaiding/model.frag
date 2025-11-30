#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objColor; // сократил название

void main()
{
    // фон
    float amb_str = 0.1;
    vec3 ambient = amb_str * lightColor;
  
    // диффузное освещение
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // float diff = max(dot(norm, lightDire), 1.0);
    float diff = max(dot(norm, lightDir), 0.0); 
    vec3 diffuse = diff * lightColor;
    
    // фонг
    float specularStrenght = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    
    // 32 это степень блеска
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
    vec3 specular = specularStrenght * spec * lightColor;  
        
    // итог
    vec3 res = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(res, 1.0);
}