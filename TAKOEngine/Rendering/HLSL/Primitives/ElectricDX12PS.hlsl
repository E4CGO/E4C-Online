#include "../Model/PhongDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

// https://www.shadertoy.com/view/4scGWj
// Port of Humus Electro demo http://humus.name/index.php?page=3D&ID=35
// Not exactly right as the noise is wrong, but is the closest I could make it.
// Uses Simplex noise by Nikita Miropolskiy https://www.shadertoy.com/view/XsX3zB

/* Simplex code license
 * This work is licensed under a 
 * Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-nc-sa/3.0/
 *  - You must attribute the work in the source code 
 *    (link to https://www.shadertoy.com/view/XsX3zB).
 *  - You may not use this work for commercial purposes.
 *  - You may distribute a derivative work only under the same license.
 */


/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
float3 random3(float3 c)
{
    float j = 4096.0 * sin(dot(c, float3(17.0, 59.4, 15.0)));
    float3 r;
    r.z = frac(512.0 * j);
    j *= .125;
    r.x = frac(512.0 * j);
    j *= .125;
    r.y = frac(512.0 * j);
    return r - 0.5;
}

/* 3d simplex noise */
float simplex3d(float3 p)
{
    /* skew constants for 3d simplex functions */
    const float F3 = 0.3333333;
    const float G3 = 0.1666667;
    
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
    float3 s = floor(p + dot(p, F3));
    float3 x = p - s + dot(s, G3);
	 
	 /* calculate i1 and i2 */
    float3 e = step(0.0, x - x.yzx);
    float3 i1 = e * (1.0 - e.zxy);
    float3 i2 = 1.0 - e.zxy * (1.0 - e);
	 	
	 /* x1, x2, x3 */
    float3 x1 = x - i1 + G3;
    float3 x2 = x - i2 + 2.0 * G3;
    float3 x3 = x - 1.0 + 3.0 * G3;
	 
	 /* 2. find four surflets and store them in d */
    float4 w, d;
	 
	 /* calculate surflet weights */
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
    w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
    w *= w;
    w *= w;
    d *= w;
	 
	 /* 3. return the sum of the four surflets */
    return dot(d, 52.0);
}

float noise(float3 m)
{
    return 0.5333333 * simplex3d(m)
			+ 0.2666667 * simplex3d(2.0 * m)
			+ 0.1333333 * simplex3d(4.0 * m)
			+ 0.0666667 * simplex3d(8.0 * m);
}

// https://www.shadertoy.com/view/MfjyWt
float happy_star(float2 uv, float anim)
{
    uv = abs(uv);
    float2 pos = min(uv.xy / uv.yx, anim);
    float p = (2.0 - pos.x - pos.y);
    return (2.0 + p * (p * p - 1.5)) / (uv.x + uv.y);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    const float3 light_color = { 0.3f, 0.3f, 0.3f };
    const float test_light_factor = 5.0f;
    
    float2 uv = { pin.texcoord.x, pin.texcoord.y + 0.07 };
    //float2 uv = { pin.texcoord.x, pin.texcoord.y - 0.03};
    //float2 uv = fragCoord.xy / iResolution.xy;
    
    uv = uv * 2. - 1.0;

    float2 p = uv;
    //float2 p = pin.texcoord.xy;
    //float2 p = fragCoord.xy / iResolution.x;
    float3 p3 = float3(p, timerGlobal * 0.4);
    
    float intensity = noise(float3(p3 * 12.0 + 12.0));
                          
    float t = clamp((uv.x * -uv.x * 0.16) + 0.15, 0., 1.);
        
    // エフェクトの方向
    //float effect_direction = abs(intensity * -t + uv.x);
    float effect_direction = abs(intensity * -t + uv.y);
    
    float g = pow(effect_direction, 0.2);
                          
    float3 col = float3(1.70, 1.48, 1.78) * 0.8;
    col = col * -g + col;
    col = col * col;
    col = col * col;
    
    float3 color = col;
    //float anim = sin(test_TIME * 12.0) * 0.1 + 1.0;
    //color *= happy_star(uv, anim);
    if (color.r <= 0.9 && color.g <= 0.9 && color.b <= 0.9)
        discard; //黒色は無視する 

    color = pow(saturate(color), 32);
    
#if 1
    // ブルームが必要な場合  (static_mesh_ps.hlsl より　抜粋)

    float3 diffuse = 0;
    float3 specular = 0;
    
    //float3 L = normalize(-light_direction.xyz);
    //float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    //float3 N = normalize(pin.world_normal.xyz);

    diffuse += light_color * color.rgb * test_light_factor;
    //specular += pow(max(0, dot(N, normalize(V + L))), 128);

    // EMISSION
    float4 emissive = texture0.Sample(sampler0, pin.texcoord);
    const float emissive_intensity = 0.1;

    return float4((diffuse /* + specular +  emissive * emissive_intensity */), 1.0f);
#else
    return float4(color, 1.0);
#endif
    
}
