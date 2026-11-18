
#define IKernelSize 150

uniform extern float4  Kernel[IKernelSize];
uniform extern float3  vNr; // North
uniform extern float3  vUp; // Up
uniform extern float3  vCp; // Forward (East)
uniform extern float2  fD;
uniform extern float   fIntensity;
uniform extern bool    bUp;

sampler tCube;
sampler tSrc;


float3 Paraboloidal_to_World(float3 i)
{
	i.xy *= 1.1f;
	float  d = (1.0f - dot(i.xy, i.xy)) * 0.5f;
	float3 p = normalize(float3(i.xy, d));
	return (vCp*p.x) + (vNr*p.y) + (vUp * p.z * i.z);
}


float4 PSPreInteg(float x : TEXCOORD0, float y : TEXCOORD1) : COLOR
{
	float3 color = 0;
	float2 p = float2(x, y);
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			float3 c = tex2D(tSrc, p + (float2(i, j) - 4) * fD).rgb;
			color += c * dot(c, float3(0.4, 0.6, 0.3));
		}
	}
	return float4(color * (1.0f/64.0f), 1);
}


float4 PSInteg(float x : TEXCOORD0, float y : TEXCOORD1, float2 sc : VPOS) : COLOR
{
	float2 qw = float2(x, y) * 2.0f - 1.0f;
	float3 vz = Paraboloidal_to_World(float3(qw.xy, (bUp ? 1 : -1)));

	float3 q = lerp(vUp, vCp, frac(x * 21)); // Randomize rotation
	float3 w = lerp(q, vNr, frac(y * 17));  // Randomize rotation
	float3 vx = normalize(cross(vz, w));
	float3 vy = normalize(cross(vz, vx));

	float3 sum = 0;

	for (int i = 0; i < IKernelSize; i++) {
		float3 d = (vx*Kernel[i].x) + (vy*Kernel[i].y) + (vz*Kernel[i].z);
		sum += texCUBE(tCube, d).rgb;
	}
	
	return float4(sqrt(sum * fIntensity * (0.7f / IKernelSize)), 1.0f);
}


float4 PSPostBlur(float x : TEXCOORD0, float y : TEXCOORD1) : COLOR
{
	float3 color = 0;
	float sum = 0;
	float2 p = float2(x, y);
	static const float a[] = { 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4 };
	for (int i = -3; i <= 3; i++) {
		for (int k = -3; k <= 3; k++) {
			float q = a[i + 3] * a[k + 3];
			sum += q;
			color += tex2D(tSrc, p + float2(i, k) * fD).rgb * q;
		}
	}
	
	return float4(color / sum, 1);
}