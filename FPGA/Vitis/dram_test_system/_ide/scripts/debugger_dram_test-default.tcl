# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test_system/_ide/scripts/debugger_dram_test-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test_system/_ide/scripts/debugger_dram_test-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351B7BAF4A" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351B7BAF4A-23727093-0"}
fpga -file /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test/_ide/bitstream/wrapper_read_write_video_debug_port.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/image_pipeline_zybo_z7/export/image_pipeline_zybo_z7/hw/wrapper_read_write_video_debug_port.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test/Debug/dram_test.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
