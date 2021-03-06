// clang-format off
#if 0
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float
; TEXCOORD                 0   xy          1     NONE   float
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
;
; Pipeline Runtime Information:
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective
; TEXCOORD                 0                 linear
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0
;
; Buffer Definitions:
;
; cbuffer cb
; {
;
;   struct cb
;   {
;
;       float2 texel_size;                            ; Offset:    0
;
;   } cb;                                             ; Offset:    0 Size:     8
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; cb                                cbuffer      NA          NA     CB0            cb0     1
; g_sampler                         sampler      NA          NA      S0             s0     1
; g_texture                         texture     f32          2d      T0             t0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;   output 3 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%"class.Texture2D<vector<float, 4> >" = type { <4 x float>, %"class.Texture2D<vector<float, 4> >::mips_type" }
%"class.Texture2D<vector<float, 4> >::mips_type" = type { i32 }
%struct.SamplerState = type { i32 }
%cb = type { <2 x float> }
%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%struct.input = type { <4 x float>, <2 x float> }
%struct.output = type { <4 x float> }

@"\01?g_texture@@3V?$Texture2D@V?$vector@M$03@@@@A" = external constant %"class.Texture2D<vector<float, 4> >", align 4
@"\01?g_sampler@@3USamplerState@@A" = external constant %struct.SamplerState, align 4
@cb = external constant %cb

define void @main() {
  %g_texture_texture_2d = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 0, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %g_sampler_sampler = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 3, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %cb_cbuffer = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %1 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %cb_cbuffer, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %4 = extractvalue %dx.types.CBufRet.f32 %3, 0
  %5 = extractvalue %dx.types.CBufRet.f32 %3, 1
  %.i0 = fmul fast float %4, 5.000000e-01
  %.i1 = fmul fast float %5, 5.000000e-01
  %.i01 = fadd fast float %.i0, %1
  %.i12 = fadd fast float %.i1, %2
  %6 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %g_texture_texture_2d, %dx.types.Handle %g_sampler_sampler, float %.i01, float %.i12, float undef, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %7 = extractvalue %dx.types.ResRet.f32 %6, 0
  %8 = extractvalue %dx.types.ResRet.f32 %6, 1
  %9 = extractvalue %dx.types.ResRet.f32 %6, 2
  %10 = extractvalue %dx.types.ResRet.f32 %6, 3
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %7)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %8)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #2

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.typeAnnotations = !{!12, !25}
!dx.viewIdState = !{!29}
!dx.entryPoints = !{!30}

!0 = !{!"dxcoob 2019.05.00"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 4}
!3 = !{!"ps", i32 6, i32 0}
!4 = !{!5, null, !8, !10}
!5 = !{!6}
!6 = !{i32 0, %"class.Texture2D<vector<float, 4> >"* undef, !"g_texture", i32 0, i32 0, i32 1, i32 2, i32 0, !7}
!7 = !{i32 0, i32 9}
!8 = !{!9}
!9 = !{i32 0, %cb* undef, !"cb", i32 0, i32 0, i32 1, i32 8, null}
!10 = !{!11}
!11 = !{i32 0, %struct.SamplerState* undef, !"g_sampler", i32 0, i32 0, i32 1, i32 0, null}
!12 = !{i32 0, %"class.Texture2D<vector<float, 4> >" undef, !13, %"class.Texture2D<vector<float, 4> >::mips_type" undef, !16, %struct.input undef, !18, %struct.output undef, !21, %cb undef, !23}
!13 = !{i32 20, !14, !15}
!14 = !{i32 6, !"h", i32 3, i32 0, i32 7, i32 9}
!15 = !{i32 6, !"mips", i32 3, i32 16}
!16 = !{i32 4, !17}
!17 = !{i32 6, !"handle", i32 3, i32 0, i32 7, i32 5}
!18 = !{i32 24, !19, !20}
!19 = !{i32 6, !"Position", i32 3, i32 0, i32 4, !"SV_POSITION", i32 7, i32 9}
!20 = !{i32 6, !"Texcoord", i32 3, i32 16, i32 4, !"TEXCOORD", i32 7, i32 9}
!21 = !{i32 16, !22}
!22 = !{i32 6, !"Color", i32 3, i32 0, i32 4, !"SV_TARGET", i32 7, i32 9}
!23 = !{i32 8, !24}
!24 = !{i32 6, !"texel_size", i32 3, i32 0, i32 7, i32 9}
!25 = !{i32 1, void ()* @main, !26}
!26 = !{!27}
!27 = !{i32 0, !28, !28}
!28 = !{}
!29 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!30 = !{void ()* @main, !"main", !31, !4, null}
!31 = !{!32, !36, null}
!32 = !{!33, !35}
!33 = !{i32 0, !"SV_Position", i8 9, i8 3, !34, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!34 = !{i32 0}
!35 = !{i32 1, !"TEXCOORD", i8 9, i8 0, !34, i8 2, i32 1, i8 2, i32 1, i8 0, null}
!36 = !{!37}
!37 = !{i32 0, !"SV_Target", i8 9, i8 16, !34, i8 0, i32 1, i8 4, i32 0, i8 0, null}

#endif

const unsigned char g_main[] = {
  0x44, 0x58, 0x42, 0x43, 0xfd, 0x70, 0x77, 0x53, 0xb6, 0x2d, 0x16, 0x9c,
  0x47, 0x9a, 0x29, 0xd1, 0xd5, 0x85, 0x32, 0xbb, 0x01, 0x00, 0x00, 0x00,
  0xdb, 0x0a, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
  0x44, 0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x00, 0xe3, 0x00, 0x00, 0x00,
  0xb7, 0x01, 0x00, 0x00, 0x53, 0x46, 0x49, 0x30, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x53, 0x47, 0x31,
  0x5d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74,
  0x69, 0x6f, 0x6e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44,
  0x00, 0x4f, 0x53, 0x47, 0x31, 0x32, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x53, 0x56, 0x5f, 0x54, 0x61, 0x72, 0x67, 0x65, 0x74, 0x00, 0x50,
  0x53, 0x56, 0x30, 0xcc, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03,
  0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x44, 0x03, 0x03,
  0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x01, 0x42, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x44, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x58, 0x49, 0x4c, 0x1c,
  0x09, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x47, 0x02, 0x00, 0x00, 0x44,
  0x58, 0x49, 0x4c, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x04,
  0x09, 0x00, 0x00, 0x42, 0x43, 0xc0, 0xde, 0x21, 0x0c, 0x00, 0x00, 0x3e,
  0x02, 0x00, 0x00, 0x0b, 0x82, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00, 0x13,
  0x00, 0x00, 0x00, 0x07, 0x81, 0x23, 0x91, 0x41, 0xc8, 0x04, 0x49, 0x06,
  0x10, 0x32, 0x39, 0x92, 0x01, 0x84, 0x0c, 0x25, 0x05, 0x08, 0x19, 0x1e,
  0x04, 0x8b, 0x62, 0x80, 0x18, 0x45, 0x02, 0x42, 0x92, 0x0b, 0x42, 0xc4,
  0x10, 0x32, 0x14, 0x38, 0x08, 0x18, 0x4b, 0x0a, 0x32, 0x62, 0x88, 0x48,
  0x90, 0x14, 0x20, 0x43, 0x46, 0x88, 0xa5, 0x00, 0x19, 0x32, 0x42, 0xe4,
  0x48, 0x0e, 0x90, 0x11, 0x23, 0xc4, 0x50, 0x41, 0x51, 0x81, 0x8c, 0xe1,
  0x83, 0xe5, 0x8a, 0x04, 0x31, 0x46, 0x06, 0x51, 0x18, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x00, 0x1b, 0x8c, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07, 0x40,
  0x02, 0xa8, 0x0d, 0x84, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x03, 0x20, 0x6d,
  0x30, 0x86, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x09, 0xa8, 0x00, 0x49,
  0x18, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x13, 0x82, 0x60, 0x42, 0x20,
  0x4c, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00, 0x89, 0x20, 0x00, 0x00, 0x55,
  0x00, 0x00, 0x00, 0x32, 0x22, 0x88, 0x09, 0x20, 0x64, 0x85, 0x04, 0x13,
  0x23, 0xa4, 0x84, 0x04, 0x13, 0x23, 0xe3, 0x84, 0xa1, 0x90, 0x14, 0x12,
  0x4c, 0x8c, 0x8c, 0x0b, 0x84, 0xc4, 0x4c, 0x10, 0x88, 0xc1, 0x0c, 0xc0,
  0x30, 0x02, 0x01, 0xcc, 0x11, 0x80, 0xc1, 0x4c, 0x6d, 0x30, 0x0e, 0xec,
  0x10, 0x0e, 0xf3, 0x30, 0x0f, 0x6e, 0x40, 0x0b, 0xe5, 0x80, 0x0f, 0xf4,
  0x50, 0x0f, 0xf2, 0x50, 0x0e, 0x72, 0x40, 0x0a, 0x7c, 0x60, 0x0f, 0xe5,
  0x30, 0x0e, 0xf4, 0xf0, 0x0e, 0xf2, 0xc0, 0x07, 0xe6, 0xc0, 0x0e, 0xef,
  0x10, 0x0e, 0xf4, 0xc0, 0x06, 0x60, 0x40, 0x07, 0x7e, 0x00, 0x06, 0x7e,
  0xa0, 0x07, 0x7a, 0xd0, 0x0e, 0xe9, 0x00, 0x0f, 0xf3, 0xf0, 0x0b, 0xf4,
  0x90, 0x0f, 0xf0, 0x50, 0x0e, 0x28, 0x10, 0x66, 0x12, 0x83, 0x71, 0x60,
  0x87, 0x70, 0x98, 0x87, 0x79, 0x70, 0x03, 0x5a, 0x28, 0x07, 0x7c, 0xa0,
  0x87, 0x7a, 0x90, 0x87, 0x72, 0x90, 0x03, 0x52, 0xe0, 0x03, 0x7b, 0x28,
  0x87, 0x71, 0xa0, 0x87, 0x77, 0x90, 0x07, 0x3e, 0x30, 0x07, 0x76, 0x78,
  0x87, 0x70, 0xa0, 0x07, 0x36, 0x00, 0x03, 0x3a, 0xf0, 0x03, 0x30, 0xf0,
  0x03, 0x24, 0x04, 0x03, 0x11, 0x37, 0x49, 0x53, 0x44, 0x09, 0x93, 0xcf,
  0x02, 0xcc, 0xb3, 0x10, 0x11, 0x3b, 0x01, 0x13, 0x81, 0x02, 0x81, 0x8c,
  0x61, 0x04, 0x01, 0x38, 0x41, 0x08, 0x50, 0x80, 0x50, 0x32, 0x02, 0x50,
  0x82, 0x85, 0x98, 0x39, 0x02, 0xa4, 0x18, 0x80, 0x20, 0x08, 0x8e, 0x40,
  0x4f, 0x31, 0x16, 0x41, 0x10, 0x1c, 0x80, 0xa2, 0xa3, 0x86, 0xcb, 0x9f,
  0xb0, 0x87, 0x90, 0x7c, 0x6e, 0xa3, 0x8a, 0x95, 0x98, 0xfc, 0xe2, 0xb6,
  0x11, 0x01, 0x00, 0x00, 0x40, 0xce, 0x3d, 0xc3, 0xe5, 0x4f, 0xd8, 0x43,
  0x48, 0x7e, 0x08, 0x34, 0xc3, 0x42, 0xa0, 0x80, 0x2a, 0xc4, 0x24, 0x54,
  0x02, 0x59, 0x37, 0x0d, 0x97, 0x3f, 0x61, 0x0f, 0x21, 0xf9, 0x2b, 0x21,
  0xad, 0xc4, 0xe4, 0x17, 0xb7, 0x8d, 0x0a, 0x00, 0x00, 0x00, 0xa1, 0x30,
  0x98, 0x50, 0x55, 0x00, 0x00, 0x00, 0x82, 0x20, 0x00, 0x94, 0xcd, 0x11,
  0x04, 0xc5, 0xa8, 0x04, 0x47, 0x10, 0x36, 0xe2, 0x06, 0x02, 0x8e, 0x91,
  0xa6, 0x88, 0x12, 0x26, 0x1f, 0x69, 0x1e, 0x6a, 0x42, 0x42, 0x80, 0x5c,
  0x23, 0x4d, 0x11, 0x25, 0x4c, 0xbe, 0x43, 0x4d, 0x0f, 0x35, 0xa1, 0x20,
  0x48, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x72, 0xc0, 0x87,
  0x74, 0x60, 0x87, 0x36, 0x68, 0x87, 0x79, 0x68, 0x03, 0x72, 0xc0, 0x87,
  0x0d, 0xaf, 0x50, 0x0e, 0x6d, 0xd0, 0x0e, 0x7a, 0x50, 0x0e, 0x6d, 0x00,
  0x0f, 0x7a, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90,
  0x0e, 0x71, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x78, 0xa0,
  0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x71, 0x60, 0x07, 0x7a, 0x30,
  0x07, 0x72, 0xd0, 0x06, 0xe9, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x73, 0x20,
  0x07, 0x6d, 0x90, 0x0e, 0x76, 0x40, 0x07, 0x7a, 0x60, 0x07, 0x74, 0xd0,
  0x06, 0xe6, 0x10, 0x07, 0x76, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x60,
  0x0e, 0x73, 0x20, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xd0, 0x06, 0xe6, 0x60,
  0x07, 0x74, 0xa0, 0x07, 0x76, 0x40, 0x07, 0x6d, 0xe0, 0x0e, 0x78, 0xa0,
  0x07, 0x71, 0x60, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x76, 0x40,
  0x07, 0x3a, 0x0f, 0x84, 0x90, 0x21, 0x23, 0x45, 0x44, 0x00, 0xd2, 0x00,
  0x80, 0x69, 0x03, 0x00, 0x66, 0x0e, 0x00, 0x60, 0xc8, 0xc3, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x90, 0xe7, 0x01,
  0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x21, 0x4f,
  0x04, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43,
  0x1e, 0x0b, 0x08, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x86, 0x3c, 0x19, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0c, 0x79, 0x38, 0x20, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xc8, 0x02, 0x01, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x32,
  0x1e, 0x98, 0x18, 0x19, 0x11, 0x4c, 0x90, 0x8c, 0x09, 0x26, 0x47, 0xc6,
  0x04, 0x43, 0x12, 0x4a, 0x60, 0x04, 0xa0, 0x10, 0x8a, 0xa1, 0x08, 0x4a,
  0xa2, 0x20, 0x0a, 0xa5, 0x0c, 0xca, 0xa1, 0x40, 0x4a, 0xa1, 0x60, 0xc8,
  0x29, 0x90, 0x22, 0x18, 0x01, 0x28, 0x84, 0x32, 0x28, 0x09, 0x52, 0x66,
  0x00, 0x28, 0x99, 0x01, 0xa0, 0x63, 0x06, 0x80, 0x8a, 0x19, 0x00, 0x22,
  0x66, 0x00, 0x68, 0x98, 0x01, 0xa0, 0x6f, 0x06, 0x80, 0xc0, 0x19, 0x00,
  0x0a, 0xc7, 0x42, 0x0c, 0x02, 0x00, 0x00, 0xe0, 0x79, 0x00, 0x00, 0x79,
  0x18, 0x00, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x1a, 0x03, 0x4c, 0x90, 0x46,
  0x02, 0x13, 0xc4, 0x88, 0x0c, 0x6f, 0xec, 0xed, 0x4d, 0x0c, 0x44, 0x06,
  0x26, 0x26, 0xc7, 0x05, 0xa6, 0xc6, 0x05, 0x06, 0x66, 0x43, 0x10, 0x4c,
  0x10, 0x84, 0x64, 0x82, 0x20, 0x28, 0x1b, 0x84, 0x81, 0x98, 0x20, 0x08,
  0xcb, 0x06, 0x61, 0x30, 0x28, 0xc0, 0xcd, 0x4d, 0x10, 0x04, 0x66, 0xc3,
  0x80, 0x24, 0xc4, 0x04, 0xa1, 0xf8, 0x98, 0x9c, 0x7d, 0xd1, 0x95, 0xe1,
  0xd1, 0xd5, 0xc9, 0x95, 0x4d, 0x10, 0x84, 0x66, 0x82, 0x20, 0x38, 0x1b,
  0x04, 0xc2, 0xd9, 0x90, 0x10, 0x0b, 0x43, 0x10, 0x43, 0x43, 0x3c, 0x1b,
  0x02, 0x68, 0x82, 0xa0, 0x70, 0x14, 0xc6, 0xc4, 0x26, 0x08, 0xc2, 0xb3,
  0x01, 0x21, 0xa4, 0x89, 0x20, 0x06, 0x0a, 0xd8, 0x10, 0x54, 0x13, 0x84,
  0xc3, 0x63, 0x72, 0xf6, 0x35, 0x17, 0xd6, 0x06, 0xc7, 0x56, 0x26, 0xb7,
  0x01, 0x21, 0x2e, 0x8c, 0x20, 0x06, 0x02, 0xd8, 0x10, 0x64, 0x1b, 0x88,
  0x08, 0xb0, 0xb4, 0x09, 0x02, 0x01, 0x06, 0x13, 0x04, 0x01, 0x62, 0x80,
  0x36, 0x41, 0x10, 0xa2, 0x09, 0x82, 0x20, 0x6d, 0x30, 0x12, 0xef, 0x23,
  0xc0, 0xc0, 0x21, 0xd1, 0x96, 0x06, 0x37, 0x37, 0x41, 0x10, 0xa6, 0x0d,
  0x44, 0x22, 0x06, 0xdf, 0x18, 0x6c, 0x18, 0xba, 0x30, 0x20, 0x83, 0x09,
  0xc2, 0x10, 0x06, 0x34, 0xd0, 0xc2, 0xdc, 0xc8, 0xd8, 0xca, 0x26, 0x08,
  0x02, 0xb5, 0xc1, 0x48, 0xce, 0xe0, 0x23, 0xc0, 0x00, 0x0d, 0x36, 0x08,
  0x46, 0x1a, 0x4c, 0x10, 0x3e, 0x31, 0x98, 0x20, 0x08, 0x15, 0x11, 0xa8,
  0xb7, 0xb9, 0x34, 0xba, 0xb4, 0x37, 0x37, 0x2e, 0x53, 0x56, 0x5f, 0x50,
  0x4f, 0x53, 0x49, 0x54, 0x49, 0x4f, 0x4e, 0x1b, 0x90, 0xa4, 0x0d, 0x3e,
  0xc2, 0x70, 0x03, 0x30, 0x70, 0x88, 0x50, 0x95, 0xe1, 0x8d, 0xbd, 0xbd,
  0xc9, 0x91, 0x11, 0xa1, 0x2a, 0xc2, 0x1a, 0x7a, 0x7a, 0x92, 0x22, 0xda,
  0x80, 0x24, 0x70, 0xf0, 0x8d, 0x81, 0x11, 0x07, 0x60, 0xe0, 0x6c, 0x18,
  0xd8, 0xe0, 0x0d, 0xe4, 0x60, 0x82, 0x00, 0x06, 0x63, 0xc0, 0x62, 0xe8,
  0x8d, 0xed, 0x4d, 0x8e, 0xc9, 0x94, 0xd5, 0x17, 0x55, 0x90, 0xd4, 0x51,
  0x11, 0xd5, 0x06, 0x24, 0xa9, 0x83, 0x8f, 0x30, 0xec, 0x00, 0x0c, 0x9c,
  0x0d, 0xc2, 0x18, 0xdc, 0xc1, 0x04, 0x21, 0xe9, 0xa8, 0xd0, 0x95, 0xe1,
  0x95, 0xb1, 0x7d, 0xcd, 0xa5, 0xe9, 0x95, 0x6d, 0x30, 0x12, 0x3d, 0xf8,
  0x08, 0x30, 0x70, 0x36, 0x08, 0xd4, 0x1e, 0x6c, 0x58, 0x08, 0xae, 0x0c,
  0xcc, 0x40, 0x0d, 0xd6, 0x60, 0x0e, 0xe8, 0x00, 0x0f, 0xf2, 0x80, 0x0f,
  0x26, 0x08, 0xcd, 0xb0, 0x01, 0xd8, 0x30, 0x10, 0x7f, 0xf0, 0x07, 0x1b,
  0x02, 0x50, 0xd8, 0x30, 0x0c, 0x7e, 0x10, 0x0a, 0x13, 0x84, 0x30, 0x20,
  0x83, 0x0d, 0xc1, 0x28, 0x90, 0x68, 0x0b, 0x4b, 0x73, 0xe3, 0x32, 0x65,
  0xf5, 0x05, 0xf5, 0x36, 0x97, 0x46, 0x97, 0xf6, 0xe6, 0x36, 0x41, 0x70,
  0xb6, 0x09, 0x82, 0xa3, 0x6d, 0x08, 0x88, 0x09, 0x82, 0x93, 0x4d, 0x10,
  0x1c, 0x6c, 0xc3, 0x42, 0x98, 0xc2, 0x29, 0xa0, 0x42, 0x2a, 0xa8, 0xc2,
  0xa0, 0x0a, 0xc4, 0x2a, 0x00, 0x13, 0x04, 0xe7, 0xda, 0xb0, 0x0c, 0x71,
  0x70, 0x0a, 0xab, 0x90, 0x0a, 0xad, 0x30, 0xb4, 0xc2, 0xb0, 0x0a, 0xc0,
  0x06, 0x81, 0x15, 0x5c, 0x81, 0xc9, 0x94, 0xd5, 0x17, 0x55, 0x98, 0xdc,
  0x59, 0x19, 0xdd, 0x04, 0xc1, 0xb1, 0x36, 0x2c, 0x04, 0x2c, 0x9c, 0x42,
  0x2c, 0xa4, 0xc2, 0x2a, 0x0c, 0xaa, 0x40, 0xac, 0x02, 0xb0, 0x21, 0x90,
  0x85, 0x0d, 0xc3, 0x2b, 0xcc, 0x02, 0xb0, 0xa1, 0xf0, 0x83, 0x52, 0xa0,
  0x85, 0x0d, 0xa8, 0xc2, 0xc6, 0x66, 0xd7, 0xe6, 0x92, 0x46, 0x56, 0xe6,
  0x46, 0x37, 0x25, 0x08, 0xaa, 0x90, 0xe1, 0xb9, 0xd8, 0x95, 0xc9, 0xcd,
  0xa5, 0xbd, 0xb9, 0x4d, 0x09, 0x88, 0x26, 0x64, 0x78, 0x2e, 0x76, 0x61,
  0x6c, 0x76, 0x65, 0x72, 0x53, 0x02, 0xa3, 0x0e, 0x19, 0x9e, 0xcb, 0x1c,
  0x5a, 0x18, 0x59, 0x99, 0x5c, 0xd3, 0x1b, 0x59, 0x19, 0xdb, 0x94, 0x20,
  0x29, 0x43, 0x86, 0xe7, 0x22, 0x57, 0x36, 0xf7, 0x56, 0x27, 0x37, 0x56,
  0x36, 0x37, 0x25, 0xd0, 0x2a, 0x91, 0xe1, 0xb9, 0xd0, 0xe5, 0xc1, 0x95,
  0x05, 0xb9, 0xb9, 0xbd, 0xd1, 0x85, 0xd1, 0xa5, 0xbd, 0xb9, 0xcd, 0x4d,
  0x11, 0xf8, 0x20, 0x14, 0xea, 0x90, 0xe1, 0xb9, 0xd8, 0xa5, 0x95, 0xdd,
  0x25, 0x91, 0x4d, 0xd1, 0x85, 0xd1, 0x95, 0x4d, 0x09, 0x46, 0xa1, 0x0e,
  0x19, 0x9e, 0x4b, 0x99, 0x1b, 0x9d, 0x5c, 0x1e, 0xd4, 0x5b, 0x9a, 0x1b,
  0xdd, 0xdc, 0x94, 0x80, 0x16, 0x00, 0x00, 0x79, 0x18, 0x00, 0x00, 0x4c,
  0x00, 0x00, 0x00, 0x33, 0x08, 0x80, 0x1c, 0xc4, 0xe1, 0x1c, 0x66, 0x14,
  0x01, 0x3d, 0x88, 0x43, 0x38, 0x84, 0xc3, 0x8c, 0x42, 0x80, 0x07, 0x79,
  0x78, 0x07, 0x73, 0x98, 0x71, 0x0c, 0xe6, 0x00, 0x0f, 0xed, 0x10, 0x0e,
  0xf4, 0x80, 0x0e, 0x33, 0x0c, 0x42, 0x1e, 0xc2, 0xc1, 0x1d, 0xce, 0xa1,
  0x1c, 0x66, 0x30, 0x05, 0x3d, 0x88, 0x43, 0x38, 0x84, 0x83, 0x1b, 0xcc,
  0x03, 0x3d, 0xc8, 0x43, 0x3d, 0x8c, 0x03, 0x3d, 0xcc, 0x78, 0x8c, 0x74,
  0x70, 0x07, 0x7b, 0x08, 0x07, 0x79, 0x48, 0x87, 0x70, 0x70, 0x07, 0x7a,
  0x70, 0x03, 0x76, 0x78, 0x87, 0x70, 0x20, 0x87, 0x19, 0xcc, 0x11, 0x0e,
  0xec, 0x90, 0x0e, 0xe1, 0x30, 0x0f, 0x6e, 0x30, 0x0f, 0xe3, 0xf0, 0x0e,
  0xf0, 0x50, 0x0e, 0x33, 0x10, 0xc4, 0x1d, 0xde, 0x21, 0x1c, 0xd8, 0x21,
  0x1d, 0xc2, 0x61, 0x1e, 0x66, 0x30, 0x89, 0x3b, 0xbc, 0x83, 0x3b, 0xd0,
  0x43, 0x39, 0xb4, 0x03, 0x3c, 0xbc, 0x83, 0x3c, 0x84, 0x03, 0x3b, 0xcc,
  0xf0, 0x14, 0x76, 0x60, 0x07, 0x7b, 0x68, 0x07, 0x37, 0x68, 0x87, 0x72,
  0x68, 0x07, 0x37, 0x80, 0x87, 0x70, 0x90, 0x87, 0x70, 0x60, 0x07, 0x76,
  0x28, 0x07, 0x76, 0xf8, 0x05, 0x76, 0x78, 0x87, 0x77, 0x80, 0x87, 0x5f,
  0x08, 0x87, 0x71, 0x18, 0x87, 0x72, 0x98, 0x87, 0x79, 0x98, 0x81, 0x2c,
  0xee, 0xf0, 0x0e, 0xee, 0xe0, 0x0e, 0xf5, 0xc0, 0x0e, 0xec, 0x30, 0x03,
  0x62, 0xc8, 0xa1, 0x1c, 0xe4, 0xa1, 0x1c, 0xcc, 0xa1, 0x1c, 0xe4, 0xa1,
  0x1c, 0xdc, 0x61, 0x1c, 0xca, 0x21, 0x1c, 0xc4, 0x81, 0x1d, 0xca, 0x61,
  0x06, 0xd6, 0x90, 0x43, 0x39, 0xc8, 0x43, 0x39, 0x98, 0x43, 0x39, 0xc8,
  0x43, 0x39, 0xb8, 0xc3, 0x38, 0x94, 0x43, 0x38, 0x88, 0x03, 0x3b, 0x94,
  0xc3, 0x2f, 0xbc, 0x83, 0x3c, 0xfc, 0x82, 0x3b, 0xd4, 0x03, 0x3b, 0xb0,
  0xc3, 0x8c, 0xcc, 0x21, 0x07, 0x7c, 0x70, 0x03, 0x74, 0x60, 0x07, 0x37,
  0x90, 0x87, 0x72, 0x98, 0x87, 0x77, 0xa8, 0x07, 0x79, 0x18, 0x87, 0x72,
  0x70, 0x83, 0x70, 0xa0, 0x07, 0x7a, 0x90, 0x87, 0x74, 0x10, 0x87, 0x7a,
  0xa0, 0x87, 0x72, 0x00, 0x00, 0x00, 0x00, 0x71, 0x20, 0x00, 0x00, 0x2b,
  0x00, 0x00, 0x00, 0x15, 0xe0, 0x05, 0x7e, 0xe7, 0xef, 0x3c, 0xdc, 0x86,
  0xb3, 0xcb, 0x72, 0x20, 0x70, 0x56, 0x9d, 0x86, 0xdb, 0x70, 0x76, 0x59,
  0x3e, 0xa5, 0x87, 0xe9, 0x65, 0x20, 0x30, 0x58, 0x00, 0x6e, 0x10, 0xf8,
  0x9d, 0xbf, 0xf4, 0x32, 0x9e, 0x5e, 0x97, 0x97, 0x81, 0xc0, 0x99, 0xf5,
  0x47, 0xa2, 0x96, 0xf1, 0xf4, 0xba, 0xbc, 0x2c, 0x23, 0x02, 0xad, 0x3f,
  0x92, 0xbd, 0x3c, 0xa6, 0xbf, 0xe5, 0xc0, 0x26, 0x09, 0x36, 0x03, 0x02,
  0x81, 0xc0, 0xa0, 0x09, 0x08, 0x42, 0x60, 0x06, 0xdb, 0x70, 0xf9, 0xce,
  0xe3, 0x0b, 0x01, 0x55, 0x14, 0x44, 0x54, 0x3a, 0xc0, 0x50, 0x12, 0x06,
  0x20, 0x60, 0x7e, 0x71, 0xdb, 0x86, 0x20, 0x0d, 0x97, 0xef, 0x3c, 0xbe,
  0x10, 0x11, 0xc0, 0x44, 0x84, 0x40, 0x33, 0x2c, 0x84, 0x11, 0x4c, 0xc3,
  0xe5, 0x3b, 0x8f, 0xbf, 0x38, 0xc0, 0x20, 0x36, 0x0f, 0x35, 0xf9, 0xc5,
  0x6d, 0xdb, 0x41, 0x35, 0x5c, 0xbe, 0xf3, 0xf8, 0x12, 0xc0, 0x3c, 0x0b,
  0x51, 0x12, 0x15, 0xb1, 0xf8, 0xc5, 0x6d, 0x5b, 0x41, 0x35, 0x5c, 0xbe,
  0xf3, 0xf8, 0xd2, 0xe4, 0x44, 0x04, 0x4a, 0x4d, 0x0f, 0x35, 0xf9, 0xc5,
  0x6d, 0xdb, 0x00, 0xc1, 0x00, 0x48, 0x03, 0x61, 0x20, 0x00, 0x00, 0x4e,
  0x00, 0x00, 0x00, 0x13, 0x04, 0x41, 0x2c, 0x10, 0x00, 0x00, 0x00, 0x07,
  0x00, 0x00, 0x00, 0xb4, 0x8d, 0x00, 0x90, 0x53, 0x02, 0x24, 0x14, 0xdf,
  0x0c, 0x40, 0xc9, 0x95, 0x1d, 0x01, 0x23, 0x00, 0x33, 0x00, 0x63, 0x04,
  0x20, 0x08, 0x82, 0xf0, 0x07, 0x00, 0x00, 0x23, 0x06, 0x09, 0x00, 0x82,
  0x60, 0xc0, 0x99, 0x41, 0x61, 0x91, 0x01, 0x19, 0x24, 0x23, 0x06, 0x09,
  0x00, 0x82, 0x60, 0xc0, 0x9d, 0x81, 0x51, 0x95, 0x41, 0x19, 0x28, 0x23,
  0x06, 0x09, 0x00, 0x82, 0x60, 0xc0, 0xa1, 0xc1, 0x91, 0x99, 0x81, 0x19,
  0x2c, 0x23, 0x06, 0x09, 0x00, 0x82, 0x60, 0xf0, 0xb4, 0x81, 0x19, 0xa0,
  0xc1, 0x19, 0x64, 0xc9, 0x88, 0x41, 0x02, 0x80, 0x20, 0x18, 0x3c, 0x6e,
  0x70, 0x06, 0x69, 0x80, 0x06, 0x8c, 0x32, 0x62, 0x70, 0x00, 0x20, 0x08,
  0x06, 0x56, 0x1b, 0x24, 0x43, 0x1a, 0x8c, 0x26, 0x04, 0xc0, 0x68, 0x82,
  0x10, 0x98, 0x90, 0xc8, 0xc7, 0x04, 0x45, 0x3e, 0x26, 0x1c, 0xf0, 0x31,
  0xe1, 0x80, 0xcf, 0x88, 0xc1, 0x03, 0x80, 0x20, 0x18, 0x64, 0x73, 0x30,
  0x31, 0x8b, 0x10, 0x38, 0x8e, 0x24, 0x49, 0xcf, 0x68, 0x42, 0x00, 0x8c,
  0x26, 0x08, 0xc1, 0x68, 0xc2, 0x20, 0x8c, 0x26, 0x10, 0xc3, 0x88, 0x41,
  0x02, 0x80, 0x20, 0x18, 0x44, 0x7a, 0xb0, 0x06, 0x75, 0x50, 0x07, 0x67,
  0x40, 0x8c, 0x18, 0x24, 0x00, 0x08, 0x82, 0x41, 0xa4, 0x07, 0x6b, 0x50,
  0x07, 0x75, 0x90, 0x0d, 0x23, 0x06, 0x09, 0x00, 0x82, 0x60, 0x10, 0xe9,
  0xc1, 0x1a, 0xd4, 0x41, 0x1d, 0xa0, 0x81, 0x30, 0x62, 0x90, 0x00, 0x20,
  0x08, 0x06, 0x91, 0x1e, 0xac, 0x41, 0x1d, 0xd4, 0x41, 0x19, 0x04, 0x18,
  0x0e, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00, 0x66, 0x33, 0xf8, 0x08, 0x6d,
  0x38, 0x84, 0x8f, 0xd0, 0xb5, 0xdd, 0x0c, 0x3e, 0x52, 0x5b, 0x0e, 0xe1,
  0x23, 0xb5, 0x6d, 0x30, 0x8a, 0x10, 0xfc, 0x42, 0x40, 0x15, 0x05, 0x11,
  0xd9, 0x4b, 0x64, 0xfc, 0x12, 0xc0, 0x3c, 0x0b, 0x11, 0xfd, 0x12, 0xc0,
  0x3c, 0x0b, 0x11, 0x99, 0x0b, 0x65, 0xfc, 0x13, 0x71, 0x4d, 0x54, 0x44,
  0xfc, 0x13, 0x71, 0x4d, 0x54, 0x44, 0xfc, 0xf6, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
