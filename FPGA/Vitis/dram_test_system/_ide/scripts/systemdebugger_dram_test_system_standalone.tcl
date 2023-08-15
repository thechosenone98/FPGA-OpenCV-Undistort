# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test_system/_ide/scripts/systemdebugger_dram_test_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test_system/_ide/scripts/systemdebugger_dram_test_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Avnet MiniZed V1 1234-oj1A" && level==0 && jtag_device_ctx=="jsn-MiniZed V1-1234-oj1A-13723093-0"}
fpga -file /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/dram_test/_ide/bitstream/wrapper_read_write_video.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/read_write_video/export/read_write_video/hw/wrapper_read_write_video.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
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
