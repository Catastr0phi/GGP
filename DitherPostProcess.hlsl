// Credit to Nikki Murello for helping me figure out dithering

cbuffer externalData : register(b0)
{
    int pixelSize;
    float width;
    float height;
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

// Bayer array for dithering
static const int Bayer4[4 * 4] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
    3, 11, 1, 9,
    15, 7, 13, 5,
};

// Palette for dithering
static const float4 Palette[5] =
{
    float4(0, 0, 0, 1),
    float4(0.25, 0.25, 0.25, 1),
    float4(0.50, 0.50, 0.50, 1),
    float4(0.75, 0.75, 0.75, 1),
    float4(1, 1, 1, 1)
};

// Distance between two colors
float GetColorDist(float4 first, float4 second)
{
    float rDiff = first.r - second.r;
    float gDiff = first.g - second.g;
    float bDiff = first.b - second.b;
    return rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
}

// Find nearest color in provided palette
float4 NearestColor(float4 color)
{
    float shortestDist = 1000.0;
    int index = -1;

    for (int i = 0; i < 5; i++)
    {
        float dist = GetColorDist(color, Palette[i]);

        if (dist < shortestDist)
        {
            shortestDist = dist;
            index = i;
        }
    }

    return Palette[index];
}


float4 main(VertexToPixel input) : SV_TARGET
{   
    float ditherSpread = 0.2;
    
    // Coordinate of the current pixel
    float2 pixelCoord = float2(input.uv.x * width, input.uv.y * height);
    
    // Get the "new" coordinate of the pixel using the pixel size
    float x = uint(pixelCoord.x) % pixelSize;
    float y = uint(pixelCoord.y) % pixelSize;

    x = floor(pixelSize / 2.0) - x;
    y = floor(pixelSize / 2.0) - y;

    x = pixelCoord.x + x;
    y = pixelCoord.y + y;
    
    // Get dither value, will be between -0.5 and 0.5
    float ditherValue = Bayer4[4 * (uint(y / pixelSize) % 4) + (uint(x / pixelSize) % 4)];
    ditherValue *= 1.0 / 16.0;
    ditherValue -= 0.5;
    
    float4 color = Pixels.Sample(ClampSampler, float2(x, y) / float2(width, height));
    color += (ditherValue * ditherSpread);
    color = NearestColor(color);
    
    return color;
}