#version 130
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec3 u_LightPos;
uniform vec3 u_LightCol;
uniform vec3 u_CamPos;

in vec3 vs_Position;
in vec3 vs_Normal;
in vec3 vs_Color;

out vec4 fs_Normal;
out vec4 fs_LightVector;
out vec4 fs_Color;
out vec4 fs_CamPos;
out vec4 fs_Position;
out vec4 fs_LightCol;

void main()
{
	vec4 lightPos = vec4(u_LightPos, 1);

    fs_Color = vec4(vs_Color * u_LightCol, 1);
    fs_Normal = u_ModelInvTr * vec4(vs_Normal, 0);

	fs_CamPos = vec4(u_CamPos, 1);
	fs_Position = vec4(vs_Position, 1);

    vec4 modelposition = u_Model * vec4(vs_Position, 1);

    // Set up our vector for the light
    fs_LightVector = lightPos - modelposition;
	fs_LightCol = vec4(u_LightCol, 1);

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;
}
