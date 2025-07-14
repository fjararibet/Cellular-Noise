#pragma once

const char *const VERTEX_SHADER_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);        
}
)";

const char *const SMOOTH_VORONOIT_FRAG_SRC = R"(
// The MIT License
// Copyright © 2014 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Smooth Voronoi - avoiding aliasing, by replacing the usual min() function, which is
// discontinuous, with a smooth version. That can help preventing some aliasing, and also
// provides with more artistic control of the final procedural textures/models.

// More Voronoi shaders:
//
// Exact edges:  https://www.shadertoy.com/view/ldl3W8
// Hierarchical: https://www.shadertoy.com/view/Xll3zX
// Smooth:       https://www.shadertoy.com/view/ldB3zc
// Voronoise:    https://www.shadertoy.com/view/Xd23Dh

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;
uniform bool fadeIn;
uniform float fadeStrength;
uniform float smoothness;
uniform float scale;
uniform float speed;
uniform float colorFrequency;
uniform float colorPhase;
uniform float offsetR;
uniform float offsetG;
uniform float offsetB;
uniform float phaseSpread;
uniform bool showIsoLines;
uniform float isoLineThickness;
uniform float isoLineIntensity;
uniform float isoLineSpacing;

float hash1(float n) { 
    return fract(sin(n) * 43758.5453);
}
vec2 hash2(vec2 p) {
    p = vec2(
        dot(p, vec2(127.1, 311.7)), 
        dot(p, vec2(269.5, 183.3))
    );
    return fract(sin(p) * 43758.5453); 
}

vec4 voronoi(in vec2 x, float w) {
    vec2 n = floor(x);
    vec2 f = fract(x);

    vec4 m = vec4(8.0, 0.0, 0.0, 0.0);
    for(int i=-2; i<=2; i++)
    for(int j=-2; j<=2; j++)
    {
        vec2 g = vec2(float(i), float(j));
        vec2 o = hash2(n + g);
		
        // animate
        o = 0.5 + 0.5*sin(u_time * speed + phaseSpread * o);

        // distance to cell		
        float d = length(g - f + o);
		
        // cell color
        vec3 col = 0.5 + 0.5*sin(
            hash1(dot(n+g,vec2(7.0,113.0)))
            * colorFrequency + colorPhase + vec3(offsetR,offsetG,offsetB));

        // in linear space
        col = col*col;
        
        // w controls the smoothness
        // do the smooth min for colors and distances		
        float h = smoothstep(-1.0, 1.0, (m.x - d) / w);
        m.x = mix(m.x, d, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
        m.yzw = mix(m.yzw, col, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
    }
    // Compute mouse as a cell
    vec2 mouse_point = u_mouse / u_resolution.y;
    vec2 mouse_x = scale * mouse_point;
    vec2 mouse_cell = floor(mouse_x);
    vec2 mouse_offset = fract(mouse_x);
    float d = length(mouse_x - x);
            
    // cell color
    vec3 col = 0.5 + 0.5*sin(
        hash1(dot(mouse_cell,vec2(13.0,117.0)))
        * colorFrequency + colorPhase + vec3(offsetR,offsetG,offsetB));

    // make brighter
    col *= 2.0;

    // in linear space
    col = col*col;
    
    // w controls the smoothness
    // do the smooth min for colors and distances		
    float h = smoothstep(-1.0, 1.0, (m.x - d) / w);
    m.x = mix(m.x, d, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
    m.yzw = mix(m.yzw, col, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
    
    return m;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 p = fragCoord / u_resolution.y;
    vec4 v = voronoi(scale * p, smoothness);

    // gamma
    vec3 color = sqrt(v.yzw);
	
    if (fadeIn) {
        color *= mix(1., 0., fadeStrength * v.x);
    }
    else {
        color *= mix(v.x, 0., fadeStrength);
    }
	
    if (showIsoLines) {
        color -= step(isoLineThickness,abs(sin(isoLineSpacing*v.x)))*isoLineIntensity;
    }
    fragColor = vec4(color, 1.0);
}

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec4 fragColor;

    mainImage(fragColor, fragCoord);

    gl_FragColor = fragColor;
}

)";
