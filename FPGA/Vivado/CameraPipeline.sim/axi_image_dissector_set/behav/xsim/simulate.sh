#!/bin/bash -f
# ****************************************************************************
# Vivado (TM) v2022.2 (64-bit)
#
# Filename    : simulate.sh
# Simulator   : Xilinx Vivado Simulator
# Description : Script for simulating the design by launching the simulator
#
# Generated by Vivado on Tue Mar 28 16:28:21 CEST 2023
# SW Build 3671981 on Fri Oct 14 04:59:54 MDT 2022
#
# IP Build 3669848 on Fri Oct 14 08:30:02 MDT 2022
#
# usage: simulate.sh
#
# ****************************************************************************
set -Eeuo pipefail
# simulate design
echo "xsim axi_image_dissector_tb_behav -key {Behavioral:axi_image_dissector_set:Functional:axi_image_dissector_tb} -tclbatch axi_image_dissector_tb.tcl -protoinst "protoinst_files/bd_0696.protoinst" -protoinst "protoinst_files/top.protoinst" -log simulate.log"
xsim axi_image_dissector_tb_behav -key {Behavioral:axi_image_dissector_set:Functional:axi_image_dissector_tb} -tclbatch axi_image_dissector_tb.tcl -protoinst "protoinst_files/bd_0696.protoinst" -protoinst "protoinst_files/top.protoinst" -log simulate.log

