#version 130
// ^ Change this to version 130 if you have compatibility issues

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec4 fs_Normal;
in vec4 fs_LightVector;
in vec4 fs_Color;
in vec4 fs_CamPos;
in vec4 fs_Position;
in vec4 fs_LightCol;

out vec4 out_Color;

void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Color;

    // Calculate the diffuse term
    float diffuseTerm = dot(normalize(fs_Normal), normalize(fs_LightVector));
    diffuseTerm = clamp(diffuseTerm, 0, 1);

	// Calculate the specular term
	float n = 100;
	vec4 viewVec = normalize(fs_Position - fs_CamPos);
	vec4 reflectVec = normalize(viewVec * fs_Normal);
	float specularTerm = pow(dot(viewVec, reflectVec), n);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm + specularTerm;

    // Compute final shaded color
    out_Color = vec4(diffuseColor.rgb * lightIntensity + (fs_LightCol.rgb * specularTerm), diffuseColor.a);
}
