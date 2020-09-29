#if 0
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; TEXCOORD                 0   xy          1     NONE   float   xy  
;
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; TEXCOORD                 0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 1, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @main() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = icmp eq i32 %1, 2
  %3 = select i1 %2, float 2.000000e+00, float 0.000000e+00
  %4 = icmp eq i32 %1, 1
  %5 = select i1 %4, float 2.000000e+00, float 0.000000e+00
  %.i0 = fmul fast float %3, 2.000000e+00
  %.i1 = fmul fast float %5, 2.000000e+00
  %.i01 = fadd fast float %.i0, -1.000000e+00
  %.i123 = fsub fast float 1.000000e+00, %.i1
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %.i01)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %.i123)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %5)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.typeAnnotations = !{!4}
!dx.viewIdState = !{!8}
!dx.entryPoints = !{!9}

!0 = !{!"dxcoob 2019.05.00"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 4}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{i32 1, void ()* @main, !5}
!5 = !{!6}
!6 = !{i32 0, !7, !7}
!7 = !{}
!8 = !{[3 x i32] [i32 1, i32 6, i32 51]}
!9 = !{void ()* @main, !"main", !10, null, null}
!10 = !{!11, !14, null}
!11 = !{!12}
!12 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !13, i8 0, i32 1, i8 1, i32 0, i8 0, null}
!13 = !{i32 0}
!14 = !{!15, !16}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !13, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!16 = !{i32 1, !"TEXCOORD", i8 9, i8 0, !13, i8 2, i32 1, i8 2, i32 1, i8 0, null}

#endif

const unsigned char g_main[] = {
  0x44, 0x58, 0x42, 0x43, 0x54, 0x28, 0x61, 0xf9, 0x96, 0xaf, 0x61, 0x2b,
  0x8c, 0xcc, 0xfe, 0xf7, 0x07, 0xf2, 0xf0, 0x0f, 0x01, 0x00, 0x00, 0x00,
  0xa9, 0x06, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
  0x44, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xe5, 0x00, 0x00, 0x00,
  0x75, 0x01, 0x00, 0x00, 0x53, 0x46, 0x49, 0x30, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x53, 0x47, 0x31,
  0x34, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x56,
  0x65, 0x72, 0x74, 0x65, 0x78, 0x49, 0x44, 0x00, 0x4f, 0x53, 0x47, 0x31,
  0x5d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74,
  0x69, 0x6f, 0x6e, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44,
  0x00, 0x50, 0x53, 0x56, 0x30, 0x88, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
  0xff, 0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x01, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x54, 0x45,
  0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x41, 0x01, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x44,
  0x03, 0x03, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x01, 0x42, 0x00, 0x03, 0x02, 0x00, 0x00, 0x33, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x44, 0x58, 0x49, 0x4c, 0x2c, 0x05, 0x00, 0x00, 0x60, 0x00, 0x01,
  0x00, 0x4b, 0x01, 0x00, 0x00, 0x44, 0x58, 0x49, 0x4c, 0x00, 0x01, 0x00,
  0x00, 0x10, 0x00, 0x00, 0x00, 0x14, 0x05, 0x00, 0x00, 0x42, 0x43, 0xc0,
  0xde, 0x21, 0x0c, 0x00, 0x00, 0x42, 0x01, 0x00, 0x00, 0x0b, 0x82, 0x20,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x07, 0x81, 0x23,
  0x91, 0x41, 0xc8, 0x04, 0x49, 0x06, 0x10, 0x32, 0x39, 0x92, 0x01, 0x84,
  0x0c, 0x25, 0x05, 0x08, 0x19, 0x1e, 0x04, 0x8b, 0x62, 0x80, 0x10, 0x45,
  0x02, 0x42, 0x92, 0x0b, 0x42, 0x84, 0x10, 0x32, 0x14, 0x38, 0x08, 0x18,
  0x4b, 0x0a, 0x32, 0x42, 0x88, 0x48, 0x90, 0x14, 0x20, 0x43, 0x46, 0x88,
  0xa5, 0x00, 0x19, 0x32, 0x42, 0xe4, 0x48, 0x0e, 0x90, 0x11, 0x22, 0xc4,
  0x50, 0x41, 0x51, 0x81, 0x8c, 0xe1, 0x83, 0xe5, 0x8a, 0x04, 0x21, 0x46,
  0x06, 0x51, 0x18, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1b, 0x8c, 0xe0,
  0xff, 0xff, 0xff, 0xff, 0x07, 0x40, 0x02, 0xa8, 0x0d, 0x84, 0xf0, 0xff,
  0xff, 0xff, 0xff, 0x03, 0x20, 0x01, 0x00, 0x00, 0x00, 0x49, 0x18, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x82, 0x60, 0x42, 0x20, 0x00, 0x00,
  0x00, 0x89, 0x20, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x32, 0x22, 0x08,
  0x09, 0x20, 0x64, 0x85, 0x04, 0x13, 0x22, 0xa4, 0x84, 0x04, 0x13, 0x22,
  0xe3, 0x84, 0xa1, 0x90, 0x14, 0x12, 0x4c, 0x88, 0x8c, 0x0b, 0x84, 0x84,
  0x4c, 0x10, 0x34, 0x23, 0x00, 0x25, 0x00, 0x8a, 0x39, 0x02, 0x30, 0x98,
  0x23, 0x40, 0x8a, 0x31, 0x33, 0x43, 0x43, 0x35, 0x03, 0x50, 0x0c, 0x98,
  0x19, 0x3a, 0xc2, 0x81, 0x80, 0x1c, 0x98, 0x39, 0x82, 0x00, 0x00, 0x00,
  0x00, 0x13, 0x14, 0x72, 0xc0, 0x87, 0x74, 0x60, 0x87, 0x36, 0x68, 0x87,
  0x79, 0x68, 0x03, 0x72, 0xc0, 0x87, 0x0d, 0xaf, 0x50, 0x0e, 0x6d, 0xd0,
  0x0e, 0x7a, 0x50, 0x0e, 0x6d, 0x00, 0x0f, 0x7a, 0x30, 0x07, 0x72, 0xa0,
  0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x71, 0xa0, 0x07, 0x73, 0x20,
  0x07, 0x6d, 0x90, 0x0e, 0x78, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90,
  0x0e, 0x71, 0x60, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xd0, 0x06, 0xe9, 0x30,
  0x07, 0x72, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x76, 0x40,
  0x07, 0x7a, 0x60, 0x07, 0x74, 0xd0, 0x06, 0xe6, 0x10, 0x07, 0x76, 0xa0,
  0x07, 0x73, 0x20, 0x07, 0x6d, 0x60, 0x0e, 0x73, 0x20, 0x07, 0x7a, 0x30,
  0x07, 0x72, 0xd0, 0x06, 0xe6, 0x60, 0x07, 0x74, 0xa0, 0x07, 0x76, 0x40,
  0x07, 0x6d, 0xe0, 0x0e, 0x78, 0xa0, 0x07, 0x71, 0x60, 0x07, 0x7a, 0x30,
  0x07, 0x72, 0xa0, 0x07, 0x76, 0x40, 0x07, 0x43, 0x9e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x3c, 0x05, 0x10,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x79, 0x10,
  0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x02,
  0x01, 0x0b, 0x00, 0x00, 0x00, 0x32, 0x1e, 0x98, 0x10, 0x19, 0x11, 0x4c,
  0x90, 0x8c, 0x09, 0x26, 0x47, 0xc6, 0x04, 0x43, 0x9a, 0x12, 0x18, 0x01,
  0x28, 0x84, 0x62, 0x20, 0x2a, 0x85, 0x12, 0x18, 0x01, 0x28, 0x89, 0x32,
  0x28, 0x84, 0x22, 0xa0, 0x1d, 0x6b, 0x08, 0x8c, 0x39, 0x00, 0x00, 0x00,
  0x00, 0x79, 0x18, 0x00, 0x00, 0x4e, 0x00, 0x00, 0x00, 0x1a, 0x03, 0x4c,
  0x90, 0x46, 0x02, 0x13, 0xc4, 0x88, 0x0c, 0x6f, 0xec, 0xed, 0x4d, 0x0c,
  0x44, 0x06, 0x26, 0x26, 0xc7, 0x05, 0xa6, 0xc6, 0x05, 0x06, 0x66, 0x43,
  0x10, 0x4c, 0x10, 0x86, 0x61, 0x82, 0x30, 0x10, 0x1b, 0x84, 0x81, 0x98,
  0x20, 0x0c, 0xc5, 0x06, 0x61, 0x30, 0x28, 0xd8, 0xcd, 0x4d, 0x10, 0x06,
  0x63, 0xc3, 0x80, 0x24, 0xc4, 0x04, 0x41, 0x00, 0x36, 0x00, 0x1b, 0x06,
  0x82, 0x61, 0x36, 0x04, 0xcd, 0x86, 0x61, 0x58, 0x9c, 0x09, 0xc2, 0xe2,
  0x6c, 0x08, 0x20, 0x12, 0x6d, 0x61, 0x69, 0x6e, 0x5c, 0xa6, 0xac, 0xbe,
  0xac, 0xca, 0xe4, 0xe8, 0xca, 0xf0, 0x92, 0x88, 0x26, 0x08, 0xc4, 0x31,
  0x41, 0x20, 0x90, 0x0d, 0x01, 0x31, 0x41, 0x20, 0x92, 0x0d, 0x0b, 0x31,
  0x51, 0x95, 0x75, 0x0d, 0x15, 0x71, 0x01, 0x1b, 0x02, 0x8c, 0xcb, 0x94,
  0xd5, 0x17, 0xd4, 0xdb, 0x5c, 0x1a, 0x5d, 0xda, 0x9b, 0xdb, 0x04, 0x81,
  0x50, 0x26, 0x08, 0xc4, 0x32, 0x41, 0x20, 0x98, 0x0d, 0x0b, 0xa1, 0x6d,
  0x9c, 0xd5, 0x0d, 0x1d, 0x71, 0x01, 0x44, 0xa8, 0x8a, 0xb0, 0x86, 0x9e,
  0x9e, 0xa4, 0x88, 0x26, 0x08, 0x44, 0xb3, 0x61, 0x19, 0xbe, 0xed, 0xb2,
  0xc0, 0x60, 0x00, 0x83, 0xe1, 0x02, 0x36, 0x08, 0x5e, 0x18, 0x6c, 0x18,
  0x32, 0x31, 0x00, 0x36, 0x14, 0x8b, 0x34, 0x06, 0x00, 0x50, 0x85, 0x8d,
  0xcd, 0xae, 0xcd, 0x25, 0x8d, 0xac, 0xcc, 0x8d, 0x6e, 0x4a, 0x10, 0x54,
  0x21, 0xc3, 0x73, 0xb1, 0x2b, 0x93, 0x9b, 0x4b, 0x7b, 0x73, 0x9b, 0x12,
  0x10, 0x4d, 0xc8, 0xf0, 0x5c, 0xec, 0xc2, 0xd8, 0xec, 0xca, 0xe4, 0xa6,
  0x04, 0x46, 0x1d, 0x32, 0x3c, 0x97, 0x39, 0xb4, 0x30, 0xb2, 0x32, 0xb9,
  0xa6, 0x37, 0xb2, 0x32, 0xb6, 0x29, 0x41, 0x52, 0x89, 0x0c, 0xcf, 0x85,
  0x2e, 0x0f, 0xae, 0x2c, 0xc8, 0xcd, 0xed, 0x8d, 0x2e, 0x8c, 0x2e, 0xed,
  0xcd, 0x6d, 0x6e, 0x4a, 0xe0, 0xd4, 0x21, 0xc3, 0x73, 0xb1, 0x4b, 0x2b,
  0xbb, 0x4b, 0x22, 0x9b, 0xa2, 0x0b, 0xa3, 0x2b, 0x9b, 0x12, 0x40, 0x75,
  0xc8, 0xf0, 0x5c, 0xca, 0xdc, 0xe8, 0xe4, 0xf2, 0xa0, 0xde, 0xd2, 0xdc,
  0xe8, 0xe6, 0xa6, 0x04, 0x63, 0x00, 0x00, 0x00, 0x00, 0x79, 0x18, 0x00,
  0x00, 0x42, 0x00, 0x00, 0x00, 0x33, 0x08, 0x80, 0x1c, 0xc4, 0xe1, 0x1c,
  0x66, 0x14, 0x01, 0x3d, 0x88, 0x43, 0x38, 0x84, 0xc3, 0x8c, 0x42, 0x80,
  0x07, 0x79, 0x78, 0x07, 0x73, 0x98, 0x71, 0x0c, 0xe6, 0x00, 0x0f, 0xed,
  0x10, 0x0e, 0xf4, 0x80, 0x0e, 0x33, 0x0c, 0x42, 0x1e, 0xc2, 0xc1, 0x1d,
  0xce, 0xa1, 0x1c, 0x66, 0x30, 0x05, 0x3d, 0x88, 0x43, 0x38, 0x84, 0x83,
  0x1b, 0xcc, 0x03, 0x3d, 0xc8, 0x43, 0x3d, 0x8c, 0x03, 0x3d, 0xcc, 0x78,
  0x8c, 0x74, 0x70, 0x07, 0x7b, 0x08, 0x07, 0x79, 0x48, 0x87, 0x70, 0x70,
  0x07, 0x7a, 0x70, 0x03, 0x76, 0x78, 0x87, 0x70, 0x20, 0x87, 0x19, 0xcc,
  0x11, 0x0e, 0xec, 0x90, 0x0e, 0xe1, 0x30, 0x0f, 0x6e, 0x30, 0x0f, 0xe3,
  0xf0, 0x0e, 0xf0, 0x50, 0x0e, 0x33, 0x10, 0xc4, 0x1d, 0xde, 0x21, 0x1c,
  0xd8, 0x21, 0x1d, 0xc2, 0x61, 0x1e, 0x66, 0x30, 0x89, 0x3b, 0xbc, 0x83,
  0x3b, 0xd0, 0x43, 0x39, 0xb4, 0x03, 0x3c, 0xbc, 0x83, 0x3c, 0x84, 0x03,
  0x3b, 0xcc, 0xf0, 0x14, 0x76, 0x60, 0x07, 0x7b, 0x68, 0x07, 0x37, 0x68,
  0x87, 0x72, 0x68, 0x07, 0x37, 0x80, 0x87, 0x70, 0x90, 0x87, 0x70, 0x60,
  0x07, 0x76, 0x28, 0x07, 0x76, 0xf8, 0x05, 0x76, 0x78, 0x87, 0x77, 0x80,
  0x87, 0x5f, 0x08, 0x87, 0x71, 0x18, 0x87, 0x72, 0x98, 0x87, 0x79, 0x98,
  0x81, 0x2c, 0xee, 0xf0, 0x0e, 0xee, 0xe0, 0x0e, 0xf5, 0xc0, 0x0e, 0xec,
  0x30, 0x03, 0x62, 0xc8, 0xa1, 0x1c, 0xe4, 0xa1, 0x1c, 0xcc, 0xa1, 0x1c,
  0xe4, 0xa1, 0x1c, 0xdc, 0x61, 0x1c, 0xca, 0x21, 0x1c, 0xc4, 0x81, 0x1d,
  0xca, 0x61, 0x06, 0xd6, 0x90, 0x43, 0x39, 0xc8, 0x43, 0x39, 0x98, 0x43,
  0x39, 0xc8, 0x43, 0x39, 0xb8, 0xc3, 0x38, 0x94, 0x43, 0x38, 0x88, 0x03,
  0x3b, 0x94, 0xc3, 0x2f, 0xbc, 0x83, 0x3c, 0xfc, 0x82, 0x3b, 0xd4, 0x03,
  0x3b, 0xb0, 0x03, 0x00, 0x00, 0x71, 0x20, 0x00, 0x00, 0x0b, 0x00, 0x00,
  0x00, 0x16, 0x30, 0x0d, 0x97, 0xef, 0x3c, 0xfe, 0xe2, 0x00, 0x83, 0xd8,
  0x3c, 0xd4, 0xe4, 0x23, 0xb7, 0x6d, 0x02, 0xd5, 0x70, 0xf9, 0xce, 0xe3,
  0x4b, 0x93, 0x13, 0x11, 0x28, 0x35, 0x3d, 0xd4, 0xe4, 0x17, 0xb7, 0x6d,
  0x00, 0x04, 0x03, 0x20, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x61, 0x20, 0x00,
  0x00, 0x31, 0x00, 0x00, 0x00, 0x13, 0x04, 0x41, 0x2c, 0x10, 0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x34, 0xa5, 0x30, 0x03, 0x50, 0x04, 0x74,
  0x63, 0x04, 0x20, 0x08, 0x82, 0x20, 0x18, 0x8c, 0x00, 0x8c, 0x11, 0x80,
  0x20, 0x08, 0xe2, 0xdf, 0x18, 0x01, 0x08, 0x82, 0x20, 0xfe, 0x0b, 0x00,
  0x00, 0x23, 0x06, 0x09, 0x00, 0x82, 0x60, 0x50, 0x54, 0x91, 0x24, 0x35,
  0xc6, 0x70, 0x43, 0x60, 0x80, 0xc1, 0x74, 0x83, 0x51, 0x04, 0xc3, 0x0d,
  0x83, 0x05, 0x06, 0xd3, 0x0d, 0xc8, 0x11, 0xd8, 0x90, 0xc8, 0xc7, 0x04,
  0x45, 0x3e, 0x26, 0x20, 0xf0, 0x31, 0x45, 0x88, 0xcf, 0x88, 0x41, 0x02,
  0x80, 0x20, 0x18, 0x20, 0x1d, 0xb4, 0x6d, 0x96, 0x30, 0x62, 0x90, 0x00,
  0x20, 0x08, 0x06, 0x48, 0x07, 0x6d, 0xdb, 0x15, 0x8c, 0x18, 0x24, 0x00,
  0x08, 0x82, 0x01, 0xd2, 0x41, 0xdb, 0x26, 0x31, 0x23, 0x06, 0x09, 0x00,
  0x82, 0x60, 0x80, 0x74, 0xd0, 0xb6, 0x51, 0xcb, 0x88, 0x41, 0x02, 0x80,
  0x20, 0x18, 0x20, 0x1d, 0xc4, 0x6d, 0xd6, 0x31, 0x62, 0x90, 0x00, 0x20,
  0x08, 0x06, 0x48, 0x07, 0x71, 0xdb, 0x55, 0x60, 0x38, 0x10, 0x00, 0x00,
  0x00, 0x06, 0x00, 0x00, 0x00, 0xb6, 0x31, 0xf8, 0x08, 0x6d, 0x1d, 0x84,
  0x8f, 0xd0, 0xb5, 0x71, 0x0c, 0x3e, 0x52, 0x9b, 0x47, 0xe1, 0x23, 0xb5,
  0x7d, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00
};
