import cv2
import numpy as np

def convert_number_to_binary(number, nb_bits):
    return bin(number & (2**nb_bits - 1))[2:].zfill(nb_bits)

def convert_binary_to_hex(binary_str):
    formatted_str = ""
    for i in range(0, len(binary_str), 4):
        formatted_str += hex(int(binary_str[i:i+4], 2))[2:]
    return formatted_str

def generate_undistort_map(image_size, camera_matrix, dist_coeffs, fixed_point=True):
    h, w = image_size
    map_xy = cv2.initUndistortRectifyMap(
        cameraMatrix=camera_matrix,
        distCoeffs=dist_coeffs,
        R=None,
        newCameraMatrix=camera_matrix,
        size=(w, h),
        m1type=cv2.CV_16SC2 if fixed_point else cv2.CV_32FC2
    )
    return map_xy

# Example usage:
image_size = (240, 320)
camera_matrix = np.array([
    [320, 0, 160],
    [0, 240, 120],
    [0, 0, 1]
], dtype=np.float32)
dist_coeffs = np.array([0.2, -0.1, 0, 0], dtype=np.float32)

map_xy = generate_undistort_map(image_size, camera_matrix, dist_coeffs, fixed_point=True)

print("map_xy:", map_xy)

# Take each coordinate pair and append it's fractionnal part to each coordinate
coordinates = []
for i in range(len(map_xy[0])):
    for j in range(len(map_xy[0][i])):
        pair = map_xy[0][i][j]
        x = pair[0] << (15 - 10) | (map_xy[1][i][j] & 31)
        y = pair[1] << (15 - 10) | (map_xy[1][i][j] >> 5)
        coordinates.append((x, y))

# Write coordinates to a file as a hex. Each line is a coordinate pair.
# Also make sure that they are in their two's complement form.
with open("remap_map.txt", "w") as f:
    for pair in coordinates:
        bin_form = convert_number_to_binary(pair[0] << 15 | pair[1], 32)
        hex_form = convert_binary_to_hex(bin_form)
        f.write(f"{hex_form}\n")

# Write another file with the coordinates in their decimal form.
with open("remap_map_dec.txt", "w") as f:
    for pair in coordinates:
        f.write(f"{float(pair[0]) / float(1 << 5)}, {float(pair[1]) / float(1 << 5)}\n")
