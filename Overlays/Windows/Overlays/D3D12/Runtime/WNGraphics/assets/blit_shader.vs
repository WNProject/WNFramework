
void main( uint id : SV_VertexID, out float4 position : SV_Position, out float2 texcoord : TEXCOORD0) {
    
        texcoord.x = (id == 2) ?  2.0 :  0.0;
        texcoord.y = (id == 1) ?  2.0 :  0.0;
 
        position = float4(texcoord * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
}