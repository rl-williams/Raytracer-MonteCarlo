#version 130
// ^ Change this to version 130 if you have compatibility issues

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec4 fs_Normal;
in vec4 fs_LightVector;
in vec4 fs_Color;

out vec4 out_Color;

void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Color;

    // Calculate the diffuse term
    float diffuseTerm = dot(normalize(fs_Normal), normalize(fs_LightVector));
    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm;

    // Compute final shaded color
    out_Color = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
}
