
struct renderpass_constants {
  float2 renderpass_size;
};

[[vk::binding(0, 1)]]
ConstantBuffer<renderpass_constants> rp_constants : register(b10, space0);