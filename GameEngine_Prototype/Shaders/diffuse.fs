#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
//uniform vec3 objectColor;

uniform vec4 MainColor;
uniform sampler2D Texture;


void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec4 result = vec4((ambient + diffuse + specular),1.0) * MainColor;

    vec4 texel;// = texture(Texture, TexCoord);
    if(float(textureSize(Texture,0).x) > 1)
    {
         texel = texture(Texture, TexCoord);
    }
    else
    {
        texel = vec4(1.0,1.0,1.0,1.0);
    }

    FragColor = texel * result;
}