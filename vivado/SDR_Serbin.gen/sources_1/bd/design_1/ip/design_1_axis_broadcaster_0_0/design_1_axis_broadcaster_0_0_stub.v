// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2022.1 (win64) Build 3526262 Mon Apr 18 15:48:16 MDT 2022
// Date        : Fri Nov 11 17:09:22 2022
// Host        : DESKTOP-789PPEF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/SoC_Fall2022/lab_7/Lab7_Serbin/vivado/SDR_Serbin.gen/sources_1/bd/design_1/ip/design_1_axis_broadcaster_0_0/design_1_axis_broadcaster_0_0_stub.v
// Design      : design_1_axis_broadcaster_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg400-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "top_design_1_axis_broadcaster_0_0,Vivado 2022.1" *)
module design_1_axis_broadcaster_0_0(aclk, aresetn, s_axis_tvalid, s_axis_tdata, 
  m_axis_tvalid, m_axis_tdata)
/* synthesis syn_black_box black_box_pad_pin="aclk,aresetn,s_axis_tvalid,s_axis_tdata[31:0],m_axis_tvalid[1:0],m_axis_tdata[63:0]" */;
  input aclk;
  input aresetn;
  input s_axis_tvalid;
  input [31:0]s_axis_tdata;
  output [1:0]m_axis_tvalid;
  output [63:0]m_axis_tdata;
endmodule