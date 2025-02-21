#version 330

// Input
in vec3 f_color;
in vec3 f_position;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    float noise_value = f_position.y /2 ;
    vec3 color1 = vec3(0.0980392157, 0.2, 0); // Verde
    vec3 color2 = vec3(0.6314, 0.3373, 0.0824); // maro
    vec3 final_color = mix(color1, color2, noise_value);

    out_color = vec4(final_color, 1.0); 
}
