#pragma once

const char *const VERTEX_SHADER_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);        
}
)";
const char *const COLOR_VERTEX_SHADER_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);        
  ourColor = aColor;        
}
)";

const char *const ORANGE_FRAG_SHADER_SRC = R"(
#version 330 core
out vec4 FragColor;
void main() {
  FragColor = vec4(196.0f / 255, 178.0f / 255, 138.0f / 255, 1.0f);
}
)";

const char *const VORONOI_FRAG_SRC = R"(
// Author: @patriciogv
// Title: 4 cells DF

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color = vec3(.0);

    // Cell positions
    vec2 point[5];
    point[0] = vec2(0.83,0.75);
    point[1] = vec2(0.60,0.07);
    point[2] = vec2(0.28,0.64);
    point[3] =  vec2(0.31,0.26);
    point[4] = u_mouse/u_resolution;

    float m_dist = 1.;  // minimum distance

    // Iterate through the points positions
    for (int i = 0; i < 5; i++) {
        float dist = distance(st, point[i]);

        // Keep the closer distance
        m_dist = min(m_dist, dist);
    }

    // Draw the min distance (distance field)
    color += m_dist;

    // Show isolines
    // color -= step(.7,abs(sin(50.0*m_dist)))*.3;

    gl_FragColor = vec4(color,1.0);
}
)";

const char *const CELL_VORONOI_FRAG_SRC = R"(
// Author: @patriciogv
// Title: CellularNoise

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color = vec3(.0);

    // Scale
    st *= 3.;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.;  // minimum distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

            // Animate the point
            point = 0.5 + 0.5*sin(u_time + 6.23*point);

            // Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;

    // Draw cell center
    color += 1.-step(.02, m_dist);

    // Draw grid
    color.r += step(.98, f_st.x) + step(.98, f_st.y);

    // Show isolines
    // color -= step(.7,abs(sin(27.0*m_dist)))*.5;

    gl_FragColor = vec4(color,1.0);
}
)";

const char *const DYNAMIC_CELL_VORONOI_FRAG_SRC = R"(
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color = vec3(.0);

    // Scale
    st *= 3.;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float min_dist1 = 1.;  // minimum distance
    float min_dist2 = 1.;  // second closest

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

            // Animate the point
            point = 0.5 + 0.5*sin(u_time + 6.23*point);

            // Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            if (dist < min_dist1) {
                min_dist2 = min_dist1;
                min_dist1 = dist;
            } else if (dist < min_dist2) {
                min_dist2 = dist;
            }

        }
    }

    float edge_thresh = 0.01;

    if (abs(min_dist1 - min_dist2) < edge_thresh) {
        color = vec3(0.8, 0.8, 0.2); // yellow
    } else {
        color += min_dist1;
        color += 1.0 - step(0.02, min_dist1);
        color.b -= step(1.0, min_dist1);
    }

    // Draw cell center
    color += 1.-step(.02, min_dist1);

    // Show isolines
    // color -= step(.7,abs(sin(27.0*min_dist1)))*.5;

    gl_FragColor = vec4(color,1.0);
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
uniform bool fadeIn;
uniform float fadeStrength;
uniform float smoothness;
uniform float scale;
uniform float speed;

float hash1( float n ) { return fract(sin(n)*43758.5453); }
vec2  hash2( vec2  p ) { p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) ); return fract(sin(p)*43758.5453); }

// The parameter w controls the smoothness
vec4 voronoi( in vec2 x, float w )
{
    vec2 n = floor(x);
    vec2 f = fract(x);

    vec4 m = vec4( 8.0, 0.0, 0.0, 0.0 );
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        vec2 g = vec2(float(i), float(j));
        vec2 o = hash2(n + g);
		
        // animate
        o = 0.5 + 0.5*sin(u_time * speed + 6.2831*o);

        // distance to cell		
        float d = length(g - f + o);
		
        // cell color
        vec3 col = 0.5 + 0.5*sin(hash1(dot(n+g,vec2(7.0,113.0)))*2.5 + 3.5 + vec3(2.0,3.0,0.0));
        // in linear space
        col = col*col;
        
        // do the smooth min for colors and distances		
        float h = smoothstep(-1.0, 1.0, (m.x - d) / w);
        m.x = mix(m.x, d, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
        m.yzw = mix(m.yzw, col, h) - h * (1.0 - h) * w / (1.0 + 3.0 * w);
    }
	
	return m;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2  p = fragCoord/u_resolution.y;
    vec4 v = voronoi(scale*p, smoothness);

    // gamma
    vec3 col = sqrt(v.yzw);
	
    if (fadeIn) {
        col *= 1.0 - fadeStrength * v.x;
    }
    else {
        col *= mix(v.x, 0., fadeStrength);
    }
	
    fragColor = vec4(col, 1.0 );
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec4 fragColor;

    mainImage(fragColor, fragCoord);  // Call your function

    gl_FragColor = fragColor;         // Output to screen
}

)";
