#version 330 core

struct SpotLight {

	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cut_off;
	float outer_cut_off;

	float constant;
	float linear;
	float quadratic;

};