#version 410 core
layout (location = 0) out vec3 color_texture;

uniform sampler2D position_texture;
uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D mra_texture;
uniform sampler2D ssao_texture;

uniform vec3 camera_position;

const int MAX_LIGHTS = 16;
uniform int light_num = 3;
uniform vec3 light_positions[MAX_LIGHTS];
uniform vec3 light_colors[MAX_LIGHTS];



in vec2 if_uv;

uniform samplerCube irradianceMap;

const float PI = 3.14159265359;

struct DirLight {
    float intensity;
    vec3 direction;

    vec3 color;
};

struct PointLight {
    float intensity;
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 color;
};

struct SpotLight {
	float intensity;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 color;
};

uniform PointLight pointLight[MAX_LIGHTS];
uniform DirLight dirLight[MAX_LIGHTS];

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) //funkcja rozk�adu wektor�w normalnych Trowbridge-Reitz GGX
{
		float a      = roughness * roughness;
        float a2     = a*a;
        float NdotH  = max(dot(N, H), 0.0);
        float NdotH2 = NdotH*NdotH;

        float nom    = a2;
        float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
        denom        = PI * denom * denom;

        return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
        float r = (roughness + 1.0);
        float k = (r*r) / 8.0;

        float num   = NdotV;
        float denom = NdotV * (1.0 - k) + k;

        return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float ggx2  = GeometrySchlickGGX(NdotV, roughness);
        float ggx1  = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
}

vec3 CalcDirLight(DirLight light, vec3 V, vec3 N, float roughness, float metallic, vec3 albedo, vec3 F0)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);
    vec3 radiance = light.color * light.intensity;

// Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); 
    
    vec3 numerator    = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
        
    //sum radiations
	float NdotL = max(dot(N, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL;  
}

vec3 CalcPointLight(PointLight light, vec3 World_position, vec3 V, vec3 N, float roughness, float metallic, vec3 albedo, vec3 F0){
    vec3 L = normalize (light.position - World_position);
	vec3 H = normalize(V + L);
	float distance = length(light.position - World_position);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	vec3 radiance = light.color * attenuation * light.intensity;

// Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); 
    
    vec3 numerator    = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular     = numerator / max(denominator, 0.001);

    vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
        
    //sum radiations
	float NdotL = max(dot(N, L), 0.0);
	return (kD * albedo / PI + specular) * radiance * NdotL; 
}

void main()
{
    vec3 World_position = texture(position_texture, if_uv).rgb;
    vec3 albedo = texture(albedo_texture, if_uv).rgb;
    vec3 mra = texture(mra_texture, if_uv).rgb;
	float metallic = mra.r;
	float roughness = mra.g;
	float ao = mra.b;
    float ssao = texture(ssao_texture, if_uv).r;

    vec3 N = normalize(texture(normal_texture, if_uv).rgb * 2.0 - 1.0);
    vec3 V = normalize(camera_position - World_position);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    //The reflectance equation
	vec3 Lo = vec3(0.0);
    for(int i = 0; i < light_num && i <MAX_LIGHTS; ++i) 
    {

        ////radiation
        //vec3 L = normalize(light_positions[i] - World_position);
        //vec3 H = normalize(V + L);
        //
        //float distance    = length(light_positions[i] - World_position);
        //float attenuation = 1.0 / ((distance * distance));
        //vec3 radiance     = light_colors[i] * intensity * attenuation; 
        //
        ////cook-torrance BRDF
        //float cosTheta = dot(H, V); //v - kierunek patrzenia
		//vec3 F = fresnelSchlick(max(cosTheta,0.0), F0);
		//float NDF = DistributionGGX(N, H, roughness);
		//float G   = GeometrySmith(N, V, L, roughness);
        //
        //
		//vec3 numerator    = NDF * G * F;
		//float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		//vec3 specular     = numerator / max(denominator, 0.001);
        //
		//vec3 kS = F;
		//vec3 kD = vec3(1.0) - kS;
		//kD *= 1.0 - metallic;
        //    
        ////sum radiations
		//float NdotL = max(dot(N, L), 0.0);
        //
		//Lo += (kD * albedo / PI + specular) * radiance * NdotL;

		Lo += CalcPointLight(pointLight[i], World_position, V, N, roughness, metallic, albedo, F0);
    }

        //Lo += CalcDirLight(DirLight(2.0, vec3(0.8, -0.6, 0.6), vec3(0.0, 0.0, 0.0), mat4(1.0), vec3(7.0f, 6.0f, 7.0f)), V, N, roughness, metallic, albedo, F0);
    vec3 ambient = vec3(0.03) * albedo ;
    vec3 color   = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    color_texture = color;
}