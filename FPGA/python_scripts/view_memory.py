import cv2
import usb.core
import numpy as np
import time

class USBDevice():
    def __init__(self, idVendor=0x0d7d, idProduct=0x0100):
        self.idVendor = idVendor
        self.idProduct = idProduct
     
    def __enter__(self):
        self.device = usb.core.find(idVendor=self.idVendor, idProduct=self.idProduct)
        if self.device is None:
            raise ValueError("Device not found")
        if self.device.is_kernel_driver_active(0):
            self.device.detach_kernel_driver(0)
        self.device.set_configuration()
        usb.util.claim_interface(self.device, 0)
        return self.device
 
    def __exit__(self, *args):
        # Release the interface and reattach the kernel driver if necessary
        usb.util.release_interface(self.device, 0)
        if self.device.is_kernel_driver_active(0):
            self.device.attach_kernel_driver(0)

def get_images_from_fpga(device, width=320, height=240, num_images=10):
    # Define the start of transmission message
    REQUEST_GET_IMAGE = "GET"
    # Create an empty image
    img = np.zeros((height, width, num_images), dtype=np.uint8)

    # Send a Start of Request along with the number of images we want to endpoint 1
    bytes_array_num_image = num_images.to_bytes(4, byteorder='big')
    print(bytes_array_num_image)
    device.write(0x01, bytes(REQUEST_GET_IMAGE, encoding="ascii") + bytes_array_num_image)
    print("Sent GET request")
    # Start a timer
    start_time = time.time()
    # Start receiving the images
    for i in range(num_images):
        # Receive the rest of the data
        data = device.read(0x81, width * height, timeout=100000)

        # Reshape the data into a 2D array of pixels and save it in the images
        img[:, :, i] = np.frombuffer(data, dtype=np.uint8).reshape((height, width))

        # Send ACK
        device.write(0x01, bytes("OKAY", encoding="ascii"))
    # Stop the timer
    end_time = time.time()

    # Print the time it took to receive the images along with the Mbps
    print("Time: ", end_time - start_time)
    print("Mbps: ", (width * height * num_images * 8) / (end_time - start_time) / 1e6)

    return img

def set_images_in_fpga(device, images):
    # Define the start of transmission message
    REQUEST_SET_IMAGE = "SET"
    # Send a Start of Request along with the number of images we want to endpoint 1
    num = images.shape[2]
    bytes_array_num_image = num.to_bytes(4, byteorder='big')
    print(bytes_array_num_image)
    device.write(0x01, bytes(REQUEST_SET_IMAGE, encoding="ascii") + bytes_array_num_image)

    # Start a timer
    start_time = time.time()
    # Start receiving the images
    for i in range(images.shape[2]):
        # Send the image
        device.write(0x01, images[:, :, i].tobytes())

        # # Receive and check the ACK
        reply = device.read(0x81, 3, timeout=10000)
        reply_str = reply.tobytes().decode('ascii')
        if reply_str != "ACK":
            raise ValueError("ACK not received")
    # Stop the timer
    end_time = time.time()

    # Print the time it took to receive the images along with the Mbps
    print("Time: ", end_time - start_time)
    print("Mbps: ", (images.shape[0] * images.shape[1] * images.shape[2] * 8) / (end_time - start_time) / 1e6)

def send_parameters(device, parameters):
    # Define the start of transmission message
    REQUEST_SET_PARAMETERS = "PAR"
    # Send a Start of Request
    device.write(0x01, bytes(REQUEST_SET_PARAMETERS, encoding="ascii") + bytes(4))
    # Send the parameters
    device.write(0x01, parameters.tobytes())
    # Receive and check the ACK
    reply = device.read(0x81, 3, timeout=10000)
    reply_str = reply.tobytes().decode('ascii')
    if reply_str != "ACK":
        raise ValueError("ACK not received")

def checkerboard(width, height, square_size, angle=0):
    # Create a larger checkerboard pattern
    img = np.zeros((height*2, width*2), dtype=np.uint8)
    for i in range(0, height*2, square_size):
        for j in range(0, width*2, square_size):
            if (i//square_size + j//square_size) % 2 == 0:
                img[i:i+square_size, j:j+square_size] = 255
    
    # Rotate the image
    center = (width, height)
    M = cv2.getRotationMatrix2D(center, angle, 1.0)
    rotated = cv2.warpAffine(img, M, (img.shape[1], img.shape[0]))
    
    # Crop the image to the desired size
    x = int(rotated.shape[1] / 2 - width / 2)
    y = int(rotated.shape[0] / 2 - height / 2)
    cropped = rotated[y:y+height, x:x+width]
    
    return cropped

def create_gradient_pattern(width, height, rotation_angle):
    # Create a grayscale gradient from 0 to 255
    img = np.tile(np.arange(256, dtype=np.uint8), (width, 1)).T

    # Rotate the image by the specified angle
    M = cv2.getRotationMatrix2D((width/2, height/2), rotation_angle, 1)
    img = cv2.warpAffine(img, M, (width, height))

    return img

def create_gradient_pattern_horizontal(width, height):
    # Create an array of zeros with the given dimensions
    img = np.zeros((height, width), dtype=np.uint8)
    
    # Loop through each row of the image
    for y in range(height):
        # Set the pixel values in the row to a repeating gradient from 0-255
        img[y, :] = np.tile(np.arange(256), (width // 256 + 1))[:width] % 256
        
    return img

def map_range(value, in_min, in_max, out_min, out_max):
    # Map a value from the input range to the output range
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

if __name__ == "__main__":
    # Get the USB device
    with USBDevice() as device:
        # Send images to the FPGA
        # Start by creating multiple images
        img = np.zeros((240, 320, 1000), dtype=np.uint8)
        for i in range(img.shape[2]):
            # Iterate through line and make half of them white and the other half black
            ## Draw a white square around the border of the image
            #img[:, :, i] = 255
            #img[10:-10, 10:-10, i] = 255
            #img[:, :, i] = create_gradient_pattern(320, 240, map_range(i, 0, img.shape[2], 0, 360))
            ## Draw a white square around the border of the image
            #img[:, :, i] = 0
            #img[10:-10, 10:-10, i] = 255
            #img[:, :, i] = create_gradient_pattern(320, 240, map_range(i, 0, img.shape[2], 0, 360))
            # Put random noise in the image
            # img[:, :, i] = np.random.randint(0, 256, (240, 320), dtype=np.uint8)
            # Put checkerboard pattern in the image
            #img[:, :, i] = np.tile(np.tile(np.array([[0, 255], [255, 0]], dtype=np.uint8), (20, 20)), (6, 8))
            img[:, :, i] = checkerboard(320, 240, 20, map_range(i, 0, img.shape[2], 0, 360))
            #img[:, :, i] = create_gradient_pattern_horizontal(320, 240)
# Send the images to the FPGA
        set_images_in_fpga(device, img)
        # Start a timer here
        start_time = time.time()
        # Wait for the FPGA to finish processing the images (it will send a message to endpoint 1)
        # Check that the message is "DONE"
        reply = device.read(0x81, 4, timeout=100000)
        # End time
        end_time = time.time()
        print("Time: ", end_time - start_time)
        print(f"FPS: {img.shape[2] / (end_time - start_time)}")
        reply_str = reply.tobytes().decode('ascii')
        if reply_str != "DONE":
            raise ValueError("DONE not received")
        # Get the images from the FPGA
        returned_img = get_images_from_fpga(device, num_images=img.shape[2])
        # Remap pixel from 0-7 to 0-255 but make sure they remain uint8
        #for i in range(returned_img.shape[2]):
        #    for y in range(returned_img.shape[1]):
        #        for x in range(returned_img.shape[0]):
        #            returned_img[x, y, i] = np.uint8(map_range(returned_img[x, y, i], 0, 7, 0, 255))

        # Compare the images
        print("Images are the same: ", np.all(img == returned_img))

        # Make all non white pixels black
        #returned_img[returned_img != 255] = 0

        # For each row in the first image, print the cummulative sum of pixel values
        #total = 0
        #for i in range(img.shape[0]):
        #    for j in range(40):
        #        # Get chunk of 8 pixels at a time
        #        total += np.sum(img[i, j*8:(j+1)*8, 0])
        #        print(f"Coordinate (Row:{i},Chunk:{j}): {total}")
        

        # Display all the images as a looping video (do a side by side comparison)
        exiting = False
        while not exiting:
            for i in range(returned_img.shape[2]):
                # Merge image and add some gray padding in the middle
                merged_img = np.concatenate((img[:, :, i], np.ones((240, 10), dtype=np.uint8) * 128, returned_img[:, :, i]), axis=1)
                # Display the image
                cv2.imshow("Side by Side", merged_img)
                key = cv2.waitKey()
                print(i)
                # If the user presses ESC, exit the program
                if key == 27:
                    exiting = True
                    break
        cv2.destroyAllWindows()



