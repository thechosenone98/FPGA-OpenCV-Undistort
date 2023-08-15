from view_memory import *
import cv2
from skimage.metrics import structural_similarity as ssim
from tkinter_parameter_window import *
import threading
import time

class ImageUpdateThread(threading.Thread):
    def __init__(self, cap, usb_device, distortion_window, default_parameters):
        super().__init__()
        self.cap = cap
        self.usb_device = usb_device
        self.distortion_window = distortion_window
        self._stop_event = threading.Event()
        self._update_event = threading.Event()
        self.input_source = "camera"
        self.threshold_min = 0
        self.threshold_max = 255
        self.fx = 0.0
        self.fx_inverse = 0.0
        self.fy = 0.0
        self.fy_inverse = 0.0
        self.cx = 0.0
        self.cy = 0.0
        self.k1 = 0.0
        self.k2 = 0.0
        self.k3 = 0.0
        self.k4 = 0.0
        self.k5 = 0.0
        self.k6 = 0.0
        self.p1 = 0.0
        self.p2 = 0.0
        self.s1 = 0.0
        self.s2 = 0.0
        self.s3 = 0.0
        self.s4 = 0.0

        # Set all parameters to their default values (if None is passed, use the default value)
        if default_parameters is None:
            default_parameters = {"fx": 320.0, "fx_inverse":1/320.0, "fy": 240.0, "fy_inverse":1/240.0, "cx": 160.0, "cy": 120.0, "k1": 0.0, "k2": 0.0, "k3": 0.0, "k4": 0.0, "k5": 0.0, "k6": 0.0, "p1": 0.0, "p2": 0.0, "s1": 0.0, "s2": 0.0, "s3": 0.0, "s4": 0.0}
        for parameter in default_parameters:
            setattr(self, parameter, default_parameters[parameter])

    def stop(self):
        self._stop_event.set()

    def update(self, parameters):
        for parameter in parameters:
            setattr(self, parameter, parameters[parameter])
        self._update_event.set()

    def set_input_source(self, source="camera"):
        self.input_source = source

    def update_thresholds(self, min_thresh, max_thresh):
        self.threshold_min = min_thresh
        self.threshold_max = max_thresh

    def update_parameters(self):
        self.distortion_window.send_parameters_over_usb({"fx": self.fx, "fx_inverse": self.fx_inverse, "fy": self.fy, "fy_inverse": self.fy_inverse, "cx": self.cx, "cy": self.cy, "k1": self.k1, "k2": self.k2, "k3": self.k3, "k4": self.k4, "k5": self.k5, "k6": self.k6, "p1": self.p1, "p2": self.p2, "s1": self.s1, "s2": self.s2, "s3": self.s3, "s4": self.s4})

    def run(self):
        image_to_send = np.zeros((240, 320, 1), dtype=np.uint8)
        result = np.zeros((240, 320, 3), dtype=np.uint8)
        while not self._stop_event.is_set():
            current_input_source = self.input_source
            full_start = time.time()
            # Update undisortion parameters if need be
            if self._update_event.is_set():
                self.update_parameters()
                self._update_event.clear()
            # Check if the thread should stop before getting a new image
            if self._stop_event.is_set():
                break
            # Get the image to process
            if "camera" in current_input_source:
                # Capture a frame from the camera
                before = time.time()
                ret, frame = self.cap.read()
                if not ret:
                    raise IOError("Cannot read a frame from the camera")
                after = time.time()
                print(f"Time to capture image from camera: {after - before}")
                # Convert the frame to grayscale
                if "rgb" not in current_input_source:
                    before = time.time()
                    frame_to_process = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                    after = time.time()
                    print(f"Time to convert image to grayscale: {after - before}")
                else:
                    # Convert the frame to RGB
                    frame_to_process = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            elif current_input_source == "checkerboard":
                # Create a checkerboard image
                frame_to_process = checkerboard(320, 240, 10, 0)
            elif current_input_source == "blank":
                # Create a white image
                frame_to_process = np.ones((240, 320), dtype=np.uint8) * 255
            elif current_input_source == "random":
                # Create a random image
                frame_to_process = np.random.randint(0, 255, (240, 320), dtype=np.uint8)
            else:
                raise ValueError("Invalid input source")

            # Check if the thread should stop before sending the image to the FPGA
            if self._stop_event.is_set():
                break

            # Send the image to the FPGA
            # Loop for each channel of the image
            if frame_to_process.ndim == 2:
                frame_to_process = np.expand_dims(frame_to_process, axis=-1)
            for channel in range(frame_to_process.shape[-1]):
                # Get the image channel
                before = time.time()
                image_to_send = frame_to_process[:,:,channel]
                image_to_send = np.expand_dims(image_to_send, axis=-1)
                set_images_in_fpga(self.usb_device, image_to_send)
                after = time.time()
                print(f"Time to SEND image to FPGA: {after - before}")
                # Get the DONE message from FPGA
                before = time.time()
                reply = self.usb_device.read(0x81, 4, timeout=100000)
                reply_str = reply.tobytes().decode('ascii')
                if reply_str != "DONE":
                    raise ValueError("DONE not received")
                after = time.time()
                print(f"Time to GET DONE message from FPGA: {after - before}")
                # Get the image from the FPGA
                before = time.time()
                # Be aware that this returns a numpy array of shape (320, 240, 1) and not (320, 240)
                imgs = get_images_from_fpga(self.usb_device, 320, 240, 1)
                result[:,:,channel] = imgs[:,:,0]
                after = time.time()
                print(f"Time to GET image from FPGA: {after - before}")

            # Break before trying to set an image in a closing window
            if self._stop_event.is_set():
                break

            # merge original image in side by side with the processed image
            # merged_img = np.concatenate((gray_with_dim[:, :, 0], np.ones((240, 10), dtype=np.uint8) * 128, img[:, :, 0]), axis=1)
            # Create OpenCV undistorted image using the parameters
            before = time.time()
            opencv_undistorted = cv2.undistort(frame_to_process, np.array([[self.fx, 0, self.cx], [0, self.fy, self.cy], [0, 0, 1]]), np.array([self.k1, self.k2, self.p1, self.p2, self.k3, self.k4, self.k5, self.k6, self.s1, self.s2, self.s3, self.s4]))
            after = time.time()
            print(f"Time to undistort image with OpenCV: {after - before}")
            # Get the difference between the FPGA and OpenCV images
            before = time.time()
            if "rgb" not in current_input_source:
                # Convert diff and img to int16 to avoid overflow
                diff = np.abs(np.int16(result[:,:,0]) - np.int16(opencv_undistorted))
            else:
                # Convert diff and img to int16 to avoid overflow
                diff = np.abs(np.int16(result) - np.int16(opencv_undistorted))
            # Apply a threshold to the difference image
            diff[diff < self.threshold_min] = 0
            diff[diff > self.threshold_max] = 0
            # Set the any non-zero values to 255
            diff[diff != 0] = 255
            after = time.time()
            print(f"Time to calculate difference between FPGA and OpenCV images: {after - before}")
            # Update all image labels
            if "rgb" not in current_input_source:
                self.distortion_window.update_image(frame_to_process[:,:,0], result[:,:,0], opencv_undistorted, diff)
            else:
                self.distortion_window.update_image(frame_to_process, result, opencv_undistorted, np.uint8(diff))
            # Print the structural similarity index (SSIM) between the FPGA and OpenCV images
            if "rgb" not in current_input_source:
                print(f"SSIM: {ssim(result[:,:,0], opencv_undistorted)}")
            else:
                print(f"SSIM: {ssim(result, opencv_undistorted, channel_axis=2)}")

            full_end = time.time()
            elapsed = full_end - full_start
            print(f"Time for full loop: {elapsed}")
            # Update fps label
            fps = 1 / elapsed
            self.distortion_window.update_fps(fps)

        print("Image update thread stopped")

def on_close(threads):
    for thread in threads:
        print(f"Stopping thread: {thread}")
        thread.stop()
        thread.join()
    root.destroy()

def init_videocapture(width=320, height=240):
    camera = cv2.VideoCapture(0, cv2.CAP_V4L2)
    camera.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    camera.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    camera.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
    camera.set(cv2.CAP_PROP_FPS, 30)
    return camera

if __name__ == "__main__":
    # Create a VideoCapture object to capture frames from the default camera (index 0)
    cap = init_videocapture(320, 240)

    # Check if the camera is opened correctly
    if not cap.isOpened():
        raise IOError("Cannot open webcam")

    # Get the USB device
    with USBDevice() as device:
        # Create a Tkinter root window
        root = tk.Tk()
        
        # Create the distortion parameter window
        distortion_window = DistortionWindow(root, device)
        image_update_thread = ImageUpdateThread(cap, device, distortion_window, default_parameters={"fx": 320.0, "fx_inverse":1/320.0, "fy": 240.0, "fy_inverse":1/240.0, "cx": 160.0, "cy": 120.0, "k1": 0.0, "k2": 0.0, "k3": 0.0, "k4": 0.0, "k5": 0.0, "k6": 0.0, "p1": 0.0, "p2": 0.0, "s1": 0.0, "s2": 0.0, "s3": 0.0, "s4": 0.0})
        distortion_window.set_update_thread(image_update_thread)
        
        
        # Schedule the start of the update thread after the main loop has started
        root.after(0, image_update_thread.start)
        root.protocol("WM_DELETE_WINDOW", lambda : on_close([image_update_thread]))
        
        root.mainloop()
        print("Main loop stopped")
        # Release the VideoCapture object and close all windows
        cap.release()
        cv2.destroyAllWindows()
