# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/image_pipeline_zybo_z7/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis/image_pipeline_zybo_z7/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {image_pipeline_zybo_z7}\
-hw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vitis}

platform write
platform generate -domains 
platform active {image_pipeline_zybo_z7}
domain active {zynq_fsbl}
bsp reload
bsp config stdin "ps7_uart_1"
bsp config stdout "ps7_uart_1"
bsp write
bsp reload
catch {bsp regenerate}
platform generate
platform active {image_pipeline_zybo_z7}
domain active {standalone_domain}
bsp reload
bsp reload
domain active {zynq_fsbl}
bsp reload
domain active {standalone_domain}
bsp config stdin "ps7_uart_1"
bsp config stdout "ps7_uart_1"
bsp write
bsp reload
catch {bsp regenerate}
domain active {zynq_fsbl}
bsp write
platform generate -domains standalone_domain 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
domain active {standalone_domain}
bsp reload
domain active {zynq_fsbl}
bsp reload
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains standalone_domain 
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
domain active {standalone_domain}
bsp reload
bsp reload
domain active {zynq_fsbl}
catch {bsp regenerate}
platform generate -domains zynq_fsbl 
platform active {image_pipeline_zybo_z7}
domain active {standalone_domain}
catch {bsp regenerate}
platform generate -domains standalone_domain 
bsp reload
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains standalone_domain,zynq_fsbl 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform generate -domains standalone_domain,zynq_fsbl 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
domain active {zynq_fsbl}
domain active {standalone_domain}
bsp reload
bsp reload
bsp reload
catch {bsp regenerate}
domain active {zynq_fsbl}
catch {bsp regenerate}
platform generate -domains standalone_domain,zynq_fsbl 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform active {image_pipeline_zybo_z7}
bsp reload
catch {bsp regenerate}
domain active {standalone_domain}
bsp reload
catch {bsp regenerate}
platform generate
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
domain active {zynq_fsbl}
bsp reload
catch {bsp regenerate}
domain active {standalone_domain}
bsp reload
catch {bsp regenerate}
platform clean
platform generate
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
domain active {zynq_fsbl}
bsp reload
catch {bsp regenerate}
domain active {standalone_domain}
bsp reload
catch {bsp regenerate}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port (copy).xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
domain active {zynq_fsbl}
domain active {zynq_fsbl}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
domain active {zynq_fsbl}
bsp reload
catch {bsp regenerate}
domain active {standalone_domain}
bsp reload
catch {bsp regenerate}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate
platform generate
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains standalone_domain,zynq_fsbl 
platform active {image_pipeline_zybo_z7}
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains standalone_domain,zynq_fsbl 
platform config -updatehw {/home/zacharie/FPGA_Projects/fpga-image-processing/FPGA/Vivado/wrapper_read_write_video_debug_port.xsa}
platform generate -domains 
