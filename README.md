# FPGA Image Processing

This repository is dedicated to accelerating the speed and efficiency of image processing on the Triton platform using FPGAs.

## FPGA

The FPGA folder contains the Vivado and Vitis projects for the FPGA. The Vivado project is used to generate the bitstream for the FPGA. The Vitis project is used to code the software that will run on the on the Zynq CPU core of the FPGA.

### Vitis

In order to open the Vitis project follow the steps described below:
1. Open Vitis and then select File->Import...
2. Select "Import project from Git".
3. Select Existing local repository and click Next.
4. Navigate to the `FPGA/Vitis` folder and select it.
5. Follow the rest of the steps in the wizard until you're done.

### Vivado

To open the Vivado project, simply open the `FPGA/Vivado/CameraPipeline.xpr` file in Vivado.

## How to use

1. Plug both micro-USB port of the Zybo-Z7 to your computer.

2. Open Vitis by following the guide above and then right-click on the `dram_test` project (located inside `dram_test_system` in the Project Explorer on the left)
and select `Run As->Launch Hardware (Single Application Debug)`. This will run the application on the FPGA.

3. To interact with the implementation on the FPGA you can use the python application `camera_view.py` located under `FPGA/python_scripts/`.
You will need to install the required python packages by running `pip install -r requirements.txt` in the `FPGA/python_scripts/` folder.
This script will use your webcam to capture images and then send them to the FPGA for processing. You can also change the distortion parameters through the interface.
