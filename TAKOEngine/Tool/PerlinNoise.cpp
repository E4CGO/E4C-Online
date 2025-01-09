#include "PerlinNoise.h"

float NoiseGenerator::fract(float x)
{
    return x - floor(x);
}

DirectX::XMFLOAT2 NoiseGenerator::fract(DirectX::XMFLOAT2 v)
{
    DirectX::XMFLOAT2 R{ 0, 0 };
    R.x = v.x - floor(v.x);
    R.y = v.y - floor(v.y);
    return R;
}
DirectX::XMFLOAT3 NoiseGenerator::fract(DirectX::XMFLOAT3 v)
{
    DirectX::XMFLOAT3 R;
    R.x = v.x - floor(v.x);
    R.y = v.y - floor(v.y);
    R.z = v.z - floor(v.z);
    return R;
}
void NoiseGenerator::floor_fract(float& outFloor, float& outFract, float x)
{
    outFloor = floor(x);
    outFract = x - outFloor;
}
void NoiseGenerator::floor_fract(int& outFloor, float& outFract, float x)
{
    float f = floor(x);
    outFloor = int(f);
    outFract = x - f;
}

float NoiseGenerator::value_hash(DirectX::XMFLOAT2 p)
{
    p = 50.0f * fract(p * 0.3183099f) + DirectX::XMFLOAT2(0.71f, 0.113f);
    return -1.0f + 2.0f * fract(p.x * p.y * (p.x + p.y));
}

DirectX::XMFLOAT2 NoiseGenerator::fade(DirectX::XMFLOAT2 t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}
DirectX::XMFLOAT3 NoiseGenerator::fade(DirectX::XMFLOAT3 t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}


float NoiseGenerator::perlin_grad(int permutation, float x, float y, float z)
{
    int h = permutation & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : ((h == 12 || h == 14) ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
float NoiseGenerator::PerlinNoise(DirectX::XMFLOAT3 p, float scale)
{
    p *= scale;

    // calc floor and fract value;
    DirectX::XMINT3 fl{ 0, 0, 0 };
    DirectX::XMFLOAT3 fr;
    floor_fract(fl.x, fr.x, p.x);
    floor_fract(fl.y, fr.y, p.y);
    floor_fract(fl.z, fr.z, p.z);


    fl.x = fl.x & 255;
    fl.y = fl.y & 255;
    fl.z = fl.z & 255;
    DirectX::XMFLOAT3 curve;
    curve = fade(fr);

    int A = perlin_permutation[fl.x] + fl.y;
    int AA = perlin_permutation[A] + fl.z;
    int AB = perlin_permutation[A + 1] + fl.z;
    int B = perlin_permutation[fl.x + 1] + fl.y;
    int BA = perlin_permutation[B] + fl.z;
    int BB = perlin_permutation[B + 1] + fl.z;

    float noise = Mathf::Lerp(
        Mathf::Lerp( // x
            Mathf::Lerp(perlin_grad(perlin_permutation[AA], fr.x, fr.y, fr.z),
                perlin_grad(perlin_permutation[BA], fr.x - 1, fr.y, fr.z), curve.x),
            Mathf::Lerp(perlin_grad(perlin_permutation[AB], fr.x, fr.y - 1, fr.z),
                perlin_grad(perlin_permutation[BB], fr.x - 1, fr.y - 1, fr.z), curve.x),
            curve.y
        ),
        Mathf::Lerp( // y
            Mathf::Lerp(perlin_grad(perlin_permutation[AA + 1], fr.x, fr.y, fr.z - 1),
                perlin_grad(perlin_permutation[BA + 1], fr.x - 1, fr.y, fr.z - 1), curve.x),
            Mathf::Lerp(perlin_grad(perlin_permutation[AB + 1], fr.x, fr.y - 1, fr.z - 1),
                perlin_grad(perlin_permutation[BB + 1], fr.x - 1, fr.y - 1, fr.z - 1), curve.x),
            curve.y
        ),
        curve.z // s
    );

    return (noise + 1.0f) * 0.5f;
}


// https://www.shadertoy.com/view/3d3fWN
DirectX::XMFLOAT3 NoiseGenerator::worley_hash(DirectX::XMFLOAT3 x)
{
    DirectX::XMFLOAT3 p = fract(x * DirectX::XMFLOAT3(0.1031f, 0.11369f, 0.13787f));
    float tmp = p.x;
    p.x = p.y;
    p.y = tmp;
    DirectX::XMFLOAT3 q = p + 19.19f;

    p += p.x * q.x + p.y * q.y + p.z * q.z;
    return fract(DirectX::XMFLOAT3((p.x + p.y) * p.z, (p.x + p.z) + p.y, (p.y + p.z) + p.x)) + 2.0f - 1.0f;
}

float NoiseGenerator::WorleyNoise(DirectX::XMFLOAT3 p, float divide)
{
    DirectX::XMFLOAT3 fl{ 0, 0, 0 }, fr{ 0,0,0 };
    floor_fract(fl.x, fr.x, p.x * divide);
    floor_fract(fl.y, fr.y, p.y * divide);
    floor_fract(fl.z, fr.z, p.z * divide);

    float f = 0.0f;

    float distanceMin = 1.0f;

    for (float x = -1.0f; x < 1.0f; ++x)
    {
        for (float y = -1.0f; y < 1.0f; ++y)
        {
            for (float z = -1.0f; z < 1.0f; ++z)
            {
                DirectX::XMFLOAT3 coord = DirectX::XMFLOAT3(x, y, z);

                DirectX::XMFLOAT3 v{ 0, 0, 0 };
                v.x = fmod(fl.x + coord.x, divide);
                v.y = fmod(fl.y + coord.y, divide);
                v.z = fmod(fl.z + coord.z, divide);

                DirectX::XMFLOAT3 h = worley_hash(v) * 0.5f + 0.5f;

                DirectX::XMFLOAT3 r = coord + h - fr;

                float d = r.x * r.x + r.y * r.y;


            }
        }
    }

    return 1.0f - distanceMin;
}