#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objColor;

void main()
{
    //1. Ambient (‘он) 
    float amb_str = 0.2; 
    vec3 ambient = amb_str * lightColor;
  
    //2. Diffuse (ќсновной свет) 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    //угол света как число от -1 до 1
    float NdotL = dot(norm, lightDir);
    

        // вместо плавного света - ступеньки
    float intensity;
    
    if (NdotL > 0.95) 
        intensity = 1.0;       // самый €ркий
    else if (NdotL > 0.5) 
        intensity = 0.7;       // средний
    else if (NdotL > 0.25) 
        intensity = 0.4;       // потемнее
    else 
        intensity = 0.1;       // тень
        
    vec3 diffuse = intensity * lightColor;
    
    //3. Specular (Ѕлики)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    
    float specRaw = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
    
        // блик
    float specularStrenght = 0.5;
    float specCutoff = 0.0;
    
    if (specRaw > 0.5)
        specCutoff = 1.0;
    else
        specCutoff = 0.0;
        
    vec3 specular = specularStrenght * specCutoff * lightColor;  
        
    vec3 res = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(res, 1.0);
}