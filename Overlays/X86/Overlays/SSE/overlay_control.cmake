set(ENABLED OFF)
option(WN_ENABLE_AVX2 "Enable AVX2 Support" ${WN_ENABLE_AVX2})
if (WN_ENABLE_AVX2)
  set(WN_ENABLE_AVX ON)
endif()

option(WN_ENABLE_AVX  "Enable AVX Support" ${WN_ENABLE_AVX})
if (WN_ENABLE_AVX)
  set(WN_ENABLE_SSE4_1 ON)
endif()

option(WN_ENABLE_SSE4_1 "Enable SSE4.1 Support" ${WN_ENABLE_SSE4_1})
if (WN_ENABLE_SSE4_1)
  set(WN_ENABLE_SSE3 ON)
endif()

option(WN_ENABLE_SSE3 "Enable SSE3 Support" ${WN_ENABLE_SSE3})
if (WN_ENABLE_SSE3)
  set(WN_ENABLE_SSE2 ON)
endif()

option(WN_ENABLE_SSE2 "Enable SSE2 Support" ${WN_ENABLE_SSE2})
if(WN_ENABLE_SSE2)
  set(WN_ENABLE_SSE ON)
endif()

option(WN_ENABLE_SSE  "Enable SSE Support" ${WN_ENABLE_SSE})

if (WN_ENABLE_SSE)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} SSE)
