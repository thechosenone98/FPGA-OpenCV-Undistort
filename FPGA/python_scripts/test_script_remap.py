import cv2
import numpy as np

# Define the camera matrix and distortion coefficients
camera_matrix = np.array( [[323.96217478,   0.0,         144.11903806],
                           [  0.0,         319.42950947, 121.04027067],
                           [  0.0,          0.0,          1.0        ]])
dist_coeffs = np.array([[-2.69313814e+01,  2.27495786e+02, -2.36143247e-02,  1.57136313e-03, -5.72697558e+02, -2.69084110e+01,  2.26956406e+02, -5.69533597e+02, -7.98374080e-03,  4.24070388e-02,  2.18769347e-02,  3.75369209e-02]])
# Load the input image
img = cv2.imread('input_resized.jpg')

# Get the image size and generate the remap array
h, w = img.shape[:2]
new_camera_matrix, roi = cv2.getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, (w,h), 1, (w,h))
map1, map2 = cv2.initUndistortRectifyMap(camera_matrix, dist_coeffs, None, new_camera_matrix, (w,h), cv2.CV_32FC1)

print(map1)
print(map1.shape)
print(map1[0].shape)
print(map1[0][0].shape)

# Convert the remap arrays to fixed-point format
map1_fixed, map2_fixed = cv2.convertMaps(map1, map2, cv2.CV_16SC2, nninterpolation=False)

# Print fix point maps
print(map2_fixed)
print(map2_fixed[0].shape)
print(map2_fixed.shape)
print(np.amax(map2_fixed, axis=0))

# Apply the remap to the input image
undistorted_img = cv2.remap(img, map1_fixed, map2_fixed, cv2.INTER_LINEAR)

# Save the undistorted image
cv2.imwrite('undistorted.jpg', undistorted_img)
