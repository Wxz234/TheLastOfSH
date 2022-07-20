struct VS_OUT {
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD;
};
VS_OUT main(uint VertexID: SV_VertexID) {
	VS_OUT Out;
	Out.texcoord = float2((VertexID << 1) & 2, VertexID & 2);
	Out.position = float4(Out.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	return Out;
}