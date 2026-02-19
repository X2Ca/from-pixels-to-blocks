#version 330 core
in vec3 dist;
in vec4 position;
in vec4 minmax;
out vec4 FragColor;
void main()
{
    float d = min(dist.x, min(dist.y, dist.z));


    if (d < 0.02f){
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        //FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
        // Normalisation :
        float h = (position.y - minmax[0]) / (minmax[1] - minmax[0]);
        vec3 color = mix(vec3(0.26,0.0,0.32), vec3(1.0,1.0,0.15), h);
        FragColor = vec4(color, 1.0f);
    }
    
}
