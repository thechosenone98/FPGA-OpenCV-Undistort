
import tkinter as tk
import cv2
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image, ImageTk
from tkinter import filedialog

class DistortionVisualizer(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Distortion Visualizer")
        self.geometry("1200x800")
        self.original_image = None
        self.distorted_image = None
        self.distortion_params = {
            "k1": 0.0,
            "k2": 0.0,
            "p1": 0.0,
            "p2": 0.0,
            "k3": 0.0
        }
        self.create_widgets()

    def create_widgets(self):
        # Create a frame to hold the load image and checkboard buttons
        self.load_frame = tk.Frame(self)
        self.load_frame.pack()

        # create button to load image
        self.load_button = tk.Button(self.load_frame, text="Load Image", command=self.load_image)
        self.load_button.pack(side=tk.LEFT)

        # create button to load checkboard image
        self.load_checkboard_button = tk.Button(self.load_frame, text="Load Checkboard", command=self.load_checkerboard)
        self.load_checkboard_button.pack(side=tk.LEFT)

        # Create a frame in which to place the two image labels
        self.image_frame = tk.Frame(self)
        self.image_frame.pack()

        # create label to display original image
        self.original_label = tk.Label(self.image_frame)
        self.original_label.pack(side=tk.LEFT)

        # create label to display distorted image
        self.distorted_label = tk.Label(self.image_frame)
        self.distorted_label.pack(side=tk.LEFT)

        # Create a frame to hold the distortion parameters frames
        self.distortion_frame = tk.Frame(self)
        self.distortion_frame.pack()

        # Create a Frame to hold the K parameters
        self.k_frame = tk.Frame(self.distortion_frame)
        self.k_frame.pack(side=tk.LEFT, anchor=tk.N)

        # Create a Frame to hold the P parameters
        self.p_frame = tk.Frame(self.distortion_frame)
        self.p_frame.pack(side=tk.LEFT, anchor=tk.N)

        # Create a Frame to hold the S parameters
        self.s_frame = tk.Frame(self.distortion_frame)
        self.s_frame.pack(side=tk.LEFT, anchor=tk.N)

        # create sliders to change distortion parameters
        self.k1_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k1", command=self.update_distorted_image)
        self.k1_slider.pack()
        self.k2_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k2", command=self.update_distorted_image)
        self.k2_slider.pack()
        self.k3_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k3", command=self.update_distorted_image)
        self.k3_slider.pack()
        self.k4_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k4", command=self.update_distorted_image)
        self.k4_slider.pack()
        self.k5_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k5", command=self.update_distorted_image)
        self.k5_slider.pack()
        self.k6_slider = tk.Scale(self.k_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="k6", command=self.update_distorted_image)
        self.k6_slider.pack()

        self.p1_slider = tk.Scale(self.p_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="p1", command=self.update_distorted_image)
        self.p1_slider.pack()
        self.p2_slider = tk.Scale(self.p_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="p2", command=self.update_distorted_image)
        self.p2_slider.pack()

        self.s1_slider = tk.Scale(self.s_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="s1", command=self.update_distorted_image)
        self.s1_slider.pack()
        self.s2_slider = tk.Scale(self.s_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="s2", command=self.update_distorted_image)
        self.s2_slider.pack()
        self.s3_slider = tk.Scale(self.s_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="s3", command=self.update_distorted_image)
        self.s3_slider.pack()
        self.s4_slider = tk.Scale(self.s_frame, from_=-1.0, to=1.0, resolution=0.01, orient=tk.HORIZONTAL, label="s4", command=self.update_distorted_image)
        self.s4_slider.pack()

        # Create a frame to hold the various save buttons
        self.save_frame = tk.Frame(self)
        self.save_frame.pack()

        # create button to save both image in a single image
        self.save_button = tk.Button(self.save_frame, text="Save Image", command=self.save_image_side_by_side)
        self.save_button.pack(side=tk.LEFT)

        # create button to save distorted image only
        self.save_distorted_button = tk.Button(self.save_frame, text="Save Distorted Image", command=self.save_distorted_image)
        self.save_distorted_button.pack(side=tk.LEFT)

    def load_image(self):
        # open file dialog to select image file
        file_path = filedialog.askopenfilename()
        if file_path:
            # read image using opencv
            self.original_image = cv2.imread(file_path)
            # convert image to RGB format
            self.original_image = cv2.cvtColor(self.original_image, cv2.COLOR_BGR2RGB)
            # display original image
            self.display_original_image()

    def display_original_image(self):
        # convert image to PIL format
        pil_image = Image.fromarray(self.original_image)
        # create photo image from PIL image
        photo_image = ImageTk.PhotoImage(pil_image)
        # display photo image in label
        self.original_label.config(image=photo_image)
        self.original_label.image = photo_image

    def load_checkerboard(self):
        # create checkerboard pattern
        checkerboard = np.zeros((640, 640), dtype=np.uint8)
        for i in range(0, 640, 80):
            for j in range(0, 640, 80):
                if (i // 80 + j // 80) % 2 == 0:
                    checkerboard[i:i+80, j:j+80] = 255
        # convert image to RGB format
        self.original_image = cv2.cvtColor(checkerboard, cv2.COLOR_GRAY2RGB)
        # display original image
        self.display_original_image()

    def update_distorted_image(self, event=None):
        # update distortion parameters
        self.distortion_params["k1"] = self.k1_slider.get()
        self.distortion_params["k2"] = self.k2_slider.get()
        self.distortion_params["k3"] = self.k3_slider.get()
        self.distortion_params["k4"] = self.k4_slider.get()
        self.distortion_params["k5"] = self.k5_slider.get()
        self.distortion_params["k6"] = self.k6_slider.get()
        self.distortion_params["p1"] = self.p1_slider.get()
        self.distortion_params["p2"] = self.p2_slider.get()
        self.distortion_params["s1"] = self.s1_slider.get()
        self.distortion_params["s2"] = self.s2_slider.get()
        self.distortion_params["s3"] = self.s3_slider.get()
        self.distortion_params["s4"] = self.s4_slider.get()

        if self.original_image is not None:
            # apply distortion to original image using opencv
            self.distorted_image = cv2.undistort(self.original_image, np.array([[self.original_image.shape[1], 0, self.original_image.shape[1]/2], [0, self.original_image.shape[0], self.original_image.shape[0]/2], [0, 0, 1]]), np.array([self.distortion_params["k1"], self.distortion_params["k2"], self.distortion_params["p1"], self.distortion_params["p2"], self.distortion_params["k3"], self.distortion_params["k4"], self.distortion_params["k5"], self.distortion_params["k6"], self.distortion_params["s1"], self.distortion_params["s2"], self.distortion_params["s3"], self.distortion_params["s4"]]))
            # convert image to RGB format
            self.distorted_image = cv2.cvtColor(self.distorted_image, cv2.COLOR_BGR2RGB)
            # display distorted image
            self.display_distorted_image()

    def display_distorted_image(self):
        # convert image to PIL format
        pil_image = Image.fromarray(self.distorted_image)
        # create photo image from PIL image
        photo_image = ImageTk.PhotoImage(pil_image)
        # display photo image in label
        self.distorted_label.config(image=photo_image)
        self.distorted_label.image = photo_image

    def save_image_side_by_side(self):
        if self.original_image is not None and self.distorted_image is not None:
            # create a new image with twice the width of the original images and the same height
            output_image = Image.new('RGBA', (self.original_image.shape[1] * 2 + 100, self.original_image.shape[0]), (0, 0, 0, 0))
            # paste the original image on the left side of the output image
            output_image.paste(Image.fromarray(self.original_image), (0, 0))
            # paste the distorted image on the right side of the output image
            output_image.paste(Image.fromarray(self.distorted_image), (self.original_image.shape[1] + 100, 0))
            # save the output image
            file_path = filedialog.asksaveasfilename(defaultextension=".png")
            if file_path:
                output_image.save(file_path)

    def save_distorted_image(self):
        if self.distorted_image is not None:
            # save the distorted image
            file_path = filedialog.asksaveasfilename(defaultextension=".png")
            if file_path:
                cv2.imwrite(file_path, cv2.cvtColor(self.distorted_image, cv2.COLOR_RGB2BGR))

if __name__ == "__main__":
    app = DistortionVisualizer()
    app.mainloop()




