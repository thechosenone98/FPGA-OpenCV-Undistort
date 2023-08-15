import tkinter as tk
import random
from tkinter import ttk
from view_memory import *
from PIL import Image, ImageTk

SHOW_FPS = False

class DistortionWindow:
    def __init__(self, parent, usb_device):
        # Create a new Toplevel widget in the same thread as the parent
        self.window = parent
        self.window.title("FPGA Test Bench")
        self.usb_device = usb_device
        self.update_thread = None

        if SHOW_FPS:
            # Create an FPS counter label at the top (make the text quite large and bold)
            self.fps_label = ttk.Label(self.window, text="FPS: 0", font=("TkDefaultFont", 16, "bold"))
            self.fps_label.pack(side="top", pady=5)
        
        # Create a frame to hold the all the image and label pairs
        view_frame = ttk.Frame(self.window)
        view_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Create a frame to hold the original image and label
        original_image_frame = ttk.Frame(view_frame)
        original_image_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Create a frame to hold the FPGA image and label
        fpga_image_frame = ttk.Frame(view_frame)
        fpga_image_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Create a frame to hold the OpenCV image and label
        opencv_image_frame = ttk.Frame(view_frame)
        opencv_image_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Create a frame to hold the difference image and label
        diff_image_frame = ttk.Frame(view_frame)
        diff_image_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        #Create all the image labels
        self.original_view_label = ttk.Label(original_image_frame, text="Original Image")
        self.original_view_label.pack(side="top", pady=5)
        self.fpga_view_label = ttk.Label(fpga_image_frame, text="FPGA Image")
        self.fpga_view_label.pack(side="top", pady=5)
        self.opencv_view_label = ttk.Label(opencv_image_frame, text="OpenCV Image")
        self.opencv_view_label.pack(side="top", pady=5)
        self.diff_view_label = ttk.Label(diff_image_frame, text="|FPGA - OpenCV|")
        self.diff_view_label.pack(side="top", pady=5)


        # Create a label to display the original image
        self.original_image_label = ttk.Label(original_image_frame)
        self.original_image_label.pack(side="top", padx=5)

        # Create a label to display the image undistorted by the FPGA
        self.fpga_image_label = ttk.Label(fpga_image_frame)
        self.fpga_image_label.pack(side="top", padx=5)

        # Create a label for correct OpenCV image undistortion
        self.opencv_image_label = ttk.Label(opencv_image_frame)
        self.opencv_image_label.pack(side="top", padx=5)

        # Create a label to show the difference between the FPGA and OpenCV undistorted images
        self.diff_image_label = ttk.Label(diff_image_frame)
        self.diff_image_label.pack(side="top", padx=5)
        
        # Create sliders for each parameter
        field_frame = ttk.Frame(self.window)
        self.fx_field = self.create_field(field_frame, "fx", 0.001, 1000.0, 320.0)
        self.fy_field = self.create_field(field_frame, "fy", 0.001, 1000.0, 240.0)
        self.cx_field = self.create_field(field_frame, "cx", 0.001, 1000.0, 160.0)
        self.cy_field = self.create_field(field_frame, "cy", 0.001, 1000.0, 120.0)
        field_frame.pack(side="top", pady=5)
        # Create a frame for all the parameter sliders
        slider_frame = ttk.Frame(self.window)
        slider_frame.pack(side="top", pady=5)
        # Create a frame for the K1-K6 sliders
        k_frame = ttk.Frame(slider_frame)
        k_frame.pack(side="left", pady=5, anchor="n")
        ttk.Label(k_frame, text="Radial Distortion Parameters").pack(side="top", pady=5)
        self.k1_slider, self.k1_field = self.create_slider("k1", -16.0, 16.0, 0.0, frame=k_frame)
        self.k2_slider, self.k2_field = self.create_slider("k2", -16.0, 16.0, 0.0, frame=k_frame)
        self.k3_slider, self.k3_field = self.create_slider("k3", -16.0, 16.0, 0.0, frame=k_frame)
        self.k4_slider, self.k4_field = self.create_slider("k4", -16.0, 16.0, 0.0, frame=k_frame)
        self.k5_slider, self.k5_field = self.create_slider("k5", -16.0, 16.0, 0.0, frame=k_frame)
        self.k6_slider, self.k6_field = self.create_slider("k6", -16.0, 16.0, 0.0, frame=k_frame)
        # Create a frame for the P1-P2 sliders
        p_frame = ttk.Frame(slider_frame)
        p_frame.pack(side="left", pady=5, anchor="n")
        ttk.Label(p_frame, text="Tangential Distortion Parameters").pack(side="top", pady=5)
        self.p1_slider, self.p1_field = self.create_slider("p1", -16.0, 16.0, 0.0, frame=p_frame)
        self.p2_slider, self.p2_field = self.create_slider("p2", -16.0, 16.0, 0.0, frame=p_frame)
        # Create a frame for the S1-S4 sliders
        s_frame = ttk.Frame(slider_frame)
        s_frame.pack(side="left", pady=5, anchor="n")
        ttk.Label(s_frame, text="Thin prism Distortion Parameters").pack(side="top", pady=5)
        self.s1_slider, self.s1_field = self.create_slider("s1", -16.0, 16.0, 0.0, frame=s_frame)
        self.s2_slider, self.s2_field = self.create_slider("s2", -16.0, 16.0, 0.0, frame=s_frame)
        self.s3_slider, self.s3_field = self.create_slider("s3", -16.0, 16.0, 0.0, frame=s_frame)
        self.s4_slider, self.s4_field = self.create_slider("s4", -16.0, 16.0, 0.0, frame=s_frame)


        # Create a frame to hold a few checkerboxes
        self.checkbox_frame = ttk.Frame(self.window)
        self.checkbox_frame.pack(side="top", pady=5)

        # Create a function to handle the checkbox clicks
        def on_checkerboard_checkbox_click(checkbox_var, input_source):
            for checkbox_state in self.checkbox_variables:
                if checkbox_state != checkbox_var:
                    checkbox_state.set(False)
            checkbox_state = checkbox_var.get()
            if checkbox_state:
                self.update_thread.set_input_source(input_source)
            else:
                self.update_thread.set_input_source("camera")

        self.rgb_state = tk.BooleanVar()
        self.rgb_checkbox = ttk.Checkbutton(self.checkbox_frame, text="RGB", variable=self.rgb_state, command=lambda : on_checkerboard_checkbox_click(self.rgb_state, "camera_rgb"))
        self.rgb_checkbox.pack(side="left", padx=5)

        self.checkerboard_state = tk.BooleanVar()
        self.checkerboard_checkbox = ttk.Checkbutton(self.checkbox_frame, text="Checkerboard", variable=self.checkerboard_state, command=lambda : on_checkerboard_checkbox_click(self.checkerboard_state, "checkerboard"))
        self.checkerboard_checkbox.pack(side="left", padx=5)

        self.blank_state = tk.BooleanVar()
        self.blank_checkbox = ttk.Checkbutton(self.checkbox_frame, text="Blank", variable=self.blank_state, command=lambda : on_checkerboard_checkbox_click(self.blank_state, "blank")) 
        self.blank_checkbox.pack(side="left", padx=5)

        self.random_state = tk.BooleanVar()
        self.random_checkbox = ttk.Checkbutton(self.checkbox_frame, text="Random", variable=self.random_state, command=lambda : on_checkerboard_checkbox_click(self.random_state, "random"))
        self.random_checkbox.pack(side="left", padx=5)

        # Create a list that contains all the checkboxes variables
        self.checkbox_variables = [self.rgb_state, self.checkerboard_state, self.blank_state, self.random_state]
        
        
        # def on_treshold_slider_change(thing):
        #     print(thing)
        #     print("Threshold slider changed")
        #     threshold_min = self.threshold_min_var.get()
        #     threshold_max = self.threshold_max_var.get()
        #     self.update_thread.update_thresholds(threshold_min, threshold_max)

        def on_threshold_input_box_change(name, index, mode):
            print("Threshold input box changed")
            # Check that it is a valid number
            try:
                threshold_min = int(self.threshold_min_var.get())
                threshold_max = int(self.threshold_max_var.get())
                self.update_thread.update_thresholds(threshold_min, threshold_max)
            except ValueError:
                print("Invalid threshold value")

        # Add two slider with integer increment varying between 0-255 to select the threshold
        self.threshold_frame = ttk.Frame(self.window)
        self.threshold_frame.pack(side="top", pady=5)
        self.threshold_min_label = ttk.Label(self.threshold_frame, text="Threshold Min.")
        self.threshold_min_label.pack(side="left")
        self.threshold_min_var = tk.IntVar()
        self.threshold_min_var.set(0)
        min_threshold_input_box = ttk.Entry(self.threshold_frame, textvariable=self.threshold_min_var, width=10)
        self.threshold_min_var.trace_add("write", on_threshold_input_box_change)
        min_threshold_input_box.pack(side="left")
        self.threshold_min_slider = ttk.Scale(self.threshold_frame, from_=0, to=255, variable=self.threshold_min_var)
        self.threshold_min_slider.pack(side="left")
        self.threshold_max_label = ttk.Label(self.threshold_frame, text="Threshold Max.")
        self.threshold_max_label.pack(side="left")
        self.threshold_max_var = tk.IntVar()
        self.threshold_max_var.set(255)
        max_threshold_input_box = ttk.Entry(self.threshold_frame, textvariable=self.threshold_max_var, width=10)
        self.threshold_max_var.trace_add("write", on_threshold_input_box_change)
        max_threshold_input_box.pack(side="left")
        self.threshold_max_slider = ttk.Scale(self.threshold_frame, from_=0, to=255, variable=self.threshold_max_var)
        self.threshold_max_slider.pack(side="left")

        # Create a frame to hold three buttons (Apply, Randomize, Randomize All, Reset)
        button_frame = ttk.Frame(self.window)
        button_frame.pack(side="top", pady=5)
        
        # Add a button to apply the changes
        apply_button = ttk.Button(button_frame, text="Apply", command=self.apply_changes)
        apply_button.pack(side="left", padx=10)

        # Add a button to randomize the parameters
        randomize_button = ttk.Button(button_frame, text="Randomize", command=lambda all=False: self.randomize_parameters(all))
        randomize_button.pack(side="left", padx=10)

        # Add a button to randomize all the parameters
        randomize_all_button = ttk.Button(button_frame, text="Randomize All", command=lambda all=True: self.randomize_parameters(all))
        randomize_all_button.pack(side="left", padx=10)

        # Add a button to reset the parameters
        reset_button = ttk.Button(button_frame, text="Reset", command=self.reset_parameters)
        reset_button.pack(side="left", padx=10)

    def create_field(self, frame, label_text, min_value, max_value, default_value):
        label = ttk.Label(frame, text=label_text)
        label.pack(side="left")

        input_var = tk.DoubleVar()
        input_var.set(default_value)
        entry = ttk.Entry(frame, textvariable=input_var)
        entry.pack(side="left")

        def on_input_box_change(name, index, mode):
            try:
                value = float(input_var.get())
                if value < min_value or value > max_value:
                    raise ValueError
            except ValueError:
                input_var.set(min_value)
            self.apply_changes()

        input_var.trace_add("write", on_input_box_change)
        return input_var

    def create_slider(self, label_text, min_value, max_value, default_value, frame=None):
        # Create a frame to hold the label, slider and input box
        if frame is None:
            slider_frame = ttk.Frame(self.window)
        else:
            slider_frame = ttk.Frame(frame)
        slider_frame.pack(side="top", pady=5)

        # Create a label for the parameter
        label = ttk.Label(slider_frame, text=label_text)
        label.pack(side="left", padx=5)

        # Create a slider for the parameter
        slider = ttk.Scale(slider_frame, from_=min_value, to=max_value, value=default_value, orient="horizontal", length=200)
        slider.pack(side="left")

        # Create an input box next to the slider
        input_var = tk.DoubleVar(value=default_value)
        input_box = ttk.Entry(slider_frame, textvariable=input_var, width=10)
        input_box.pack(side="left", padx=5)

        # Link the slider and input box
        def on_slider_move(event):
            input_var.set(slider.get())
            self.apply_changes()
            
        slider.bind("<ButtonRelease-1>", on_slider_move)
        slider.bind("<B1-Motion>", on_slider_move)

        def on_input_box_change(name, index, mode):
            try:
                try:
                    value = float(input_var.get())
                except tk.TclError:
                    # Empty text box
                    value = 0.0
                except ValueError:
                    # Non-numeric text
                    value = 0.0
                print(value)
                if min_value <= value <= max_value:
                    print("Setting slider value to", value)
                    slider.set(value)
            except ValueError:
                print("Invalid value")
            self.apply_changes()

        input_var.trace_add("write", on_input_box_change)

        return (slider, input_var)

    def send_parameters_over_usb(self, parameters):
        # Create a list of the parameters by getting all values from parameter dict
        parameter_list = [parameters["fx"], parameters["fx_inverse"], parameters["fy"], parameters["fy_inverse"], parameters["cx"], parameters["cy"], parameters["k1"], parameters["k2"], parameters["k3"], parameters["k4"], parameters["k5"], parameters["k6"], parameters["p1"], parameters["p2"], parameters["s1"], parameters["s2"], parameters["s3"], parameters["s4"]]
        print(parameter_list)
        # Convert this to a numpy list of doubles
        parameter_list = np.array(parameter_list, dtype=np.double)
        # Send them as doubles over USB
        send_parameters(self.usb_device, parameter_list)
    
    def set_update_thread(self, update_thread):
        self.update_thread = update_thread

    def create_random_parameters(self, all=False):
        # Create a random set of parameters
        parameters = {}
        parameters["k1"] = random.uniform(-1.0, 1.0)
        parameters["k2"] = random.uniform(-1.0, 1.0)
        parameters["k3"] = random.uniform(-1.0, 1.0)
        parameters["k4"] = random.uniform(-1.0, 1.0)
        parameters["k5"] = random.uniform(-1.0, 1.0)
        parameters["k6"] = random.uniform(-1.0, 1.0)
        parameters["p1"] = random.uniform(-1.0, 1.0)
        parameters["p2"] = random.uniform(-1.0, 1.0)
        parameters["s1"] = random.uniform(-1.0, 1.0)
        parameters["s2"] = random.uniform(-1.0, 1.0)
        parameters["s3"] = random.uniform(-1.0, 1.0)
        parameters["s4"] = random.uniform(-1.0, 1.0)
        if all:
            parameters["fx"] = random.uniform(0.001, 500.0)
            parameters["fx_inverse"] = 1.0 / parameters["fx"]
            parameters["fy"] = random.uniform(0.001, 500.0)
            parameters["fy_inverse"] = 1.0 / parameters["fy"]
            parameters["cx"] = random.uniform(0.001, 250.0)
            parameters["cy"] = random.uniform(0.001, 250.0)
        else:
            parameters["fx"] = self.fx_field.get()
            parameters["fx_inverse"] = 1.0 / parameters["fx"]
            parameters["fy"] = self.fy_field.get()
            parameters["fy_inverse"] = 1.0 / parameters["fy"]
            parameters["cx"] = self.cx_field.get()
            parameters["cy"] = self.cy_field.get()
        return parameters

    def reset_parameters(self):
        # Create a random set of parameters
        parameters = {"fx": 320.0, "fx_inverse": 1/320.0, "fy": 240.0, "fy_inverse": 1/240.0, "cx": 160.0, "cy": 120.0, "k1": 0.0, "k2": 0.0, "k3": 0.0, "k4": 0.0, "k5": 0.0, "k6": 0.0, "p1": 0.0, "p2": 0.0, "s1": 0.0, "s2": 0.0, "s3": 0.0, "s4": 0.0}
        # Update the sliders and input boxes
        self.update_sliders(parameters)
        # Send the parameters over USB
        self.apply_changes()

    def randomize_parameters(self, all=False):
        # Create a random set of parameters
        if all:
            parameters = self.create_random_parameters(all=True)
        else:
            parameters = self.create_random_parameters(all=False)
        # Update the sliders and input boxes
        self.update_sliders(parameters)
        # Send the parameters over USB
        self.apply_changes()

    def update_sliders(self, parameters):
        self.fx_field.set(parameters["fx"])
        self.fy_field.set(parameters["fy"])
        self.cx_field.set(parameters["cx"])
        self.cy_field.set(parameters["cy"])
        self.k1_field.set(parameters["k1"])
        self.k2_field.set(parameters["k2"])
        self.k3_field.set(parameters["k3"])
        self.k4_field.set(parameters["k4"])
        self.k5_field.set(parameters["k5"])
        self.k6_field.set(parameters["k6"])
        self.p1_field.set(parameters["p1"])
        self.p2_field.set(parameters["p2"])
        self.s1_field.set(parameters["s1"])
        self.s2_field.set(parameters["s2"])
        self.s3_field.set(parameters["s3"])
        self.s4_field.set(parameters["s4"])

    def apply_changes(self):
        # Get the value of each slider
        fx = self.fx_field.get()
        fy = self.fy_field.get()
        cx = self.cx_field.get()
        cy = self.cy_field.get()
        k1 = self.k1_slider.get()
        k2 = self.k2_slider.get()
        k3 = self.k3_slider.get()
        k4 = self.k4_slider.get()
        k5 = self.k5_slider.get()
        k6 = self.k6_slider.get()
        p1 = self.p1_slider.get()
        p2 = self.p2_slider.get()
        s1 = self.s1_slider.get()
        s2 = self.s2_slider.get()
        s3 = self.s3_slider.get()
        s4 = self.s4_slider.get()
        
        # Call the update thread to send the parameters over USB
        if self.update_thread is None:
            raise Exception("Update thread not set")
        self.update_thread.update({"fx":fx, "fx_inverse":1/fx, "fy":fy, "fy_inverse":1/fy, "cx":cx, "cy":cy, "k1":k1, "k2":k2, "k3":k3, "k4":k4, "k5":k5, "k6":k6, "p1":p1, "p2":p2, "s1":s1, "s2":s2, "s3":s3, "s4":s4})
        
    def update_image(self, original_image, fpga_undistorted_image, opencv_undistorted_image, difference_image):
        # Convert the OpenCV image to a PhotoImage object
        orig_image = Image.fromarray(original_image)
        fpga_image = Image.fromarray(fpga_undistorted_image)
        cv_image = Image.fromarray(opencv_undistorted_image)
        diff_image = Image.fromarray(difference_image)
        orig_image = ImageTk.PhotoImage(orig_image)
        fpga_image = ImageTk.PhotoImage(fpga_image)
        cv_image = ImageTk.PhotoImage(cv_image)
        diff_image = ImageTk.PhotoImage(diff_image)

        # Update the image on the GUI
        self.original_image_label.configure(image=orig_image)
        self.fpga_image_label.configure(image=fpga_image)
        self.opencv_image_label.configure(image=cv_image)
        self.diff_image_label.configure(image=diff_image)
        self.original_image_label.image = orig_image
        self.fpga_image_label.image = fpga_image
        self.opencv_image_label.image = cv_image
        self.diff_image_label.image = diff_image

    def update_fps(self, fps):
        if SHOW_FPS:
            self.fps_label.configure(text=f"FPS: {fps:.2f}")

