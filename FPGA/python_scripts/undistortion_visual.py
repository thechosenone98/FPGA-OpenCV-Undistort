import matplotlib.pyplot as plt
import numpy as np
from matplotlib import patches

from linear_interpolation_visual import bilinear_interpolation, get_neighbours
import cv2
import numpy as np

IMAGE_SIZE = 5

# Create a synthetic 16x16 gradient image
image = np.zeros((IMAGE_SIZE, IMAGE_SIZE), dtype=np.uint8)
for i in range(IMAGE_SIZE):
    for j in range(IMAGE_SIZE):
        image[i, j] = (i+j)%256

# Create mapping using OpenCV
camera_matrix = np.array([[IMAGE_SIZE, 0, IMAGE_SIZE/2],[0, IMAGE_SIZE, IMAGE_SIZE/2],[0, 0, 1]])
mappings_x, mappings_y = cv2.initUndistortRectifyMap(camera_matrix, np.array([0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), np.eye(3), camera_matrix, (IMAGE_SIZE, IMAGE_SIZE), cv2.CV_32FC1)
print(mappings_x)
print(mappings_y)

# Create output image
output_image = np.zeros_like(image)

# Create subplots
fig, axes = plt.subplots(1, 2, figsize=(12, 6))

# Plot original image on the left subplot
axes[0].imshow(image, cmap='gray')
axes[0].set_title("Original Image")
axes[0].set_facecolor('black')
axes[0].set_xlim(-1.5, IMAGE_SIZE-0.5)
axes[0].set_ylim(IMAGE_SIZE-0.5, -1.5)

# Draw a rectangle around the image
rect = patches.Rectangle((-0.5, -0.5), IMAGE_SIZE, IMAGE_SIZE, linewidth=2, edgecolor='g', facecolor='none')
axes[0].add_patch(rect)

# Plot output image on the right subplot
axes[1].imshow(output_image, cmap='gray')
axes[1].set_title("Output Image after Undistortion")

# Plot mappings with arrows
for i in range(IMAGE_SIZE):
    for j in range(IMAGE_SIZE):
        # Check if coordinate is part of top left triangle of image
        if i < j:
            continue
        # Get the destination coordinates for the pixel at (i, j)
        orig_x, orig_y = mappings_x[i][j], mappings_y[i][j]
        print(orig_x, orig_y)
        neighbours = get_neighbours(orig_x, orig_y)
        print(neighbours)
        values = []
        for x1, y1 in neighbours:
            if 0 <= x1 < IMAGE_SIZE and 0 <= y1 < IMAGE_SIZE:
                values.append(image[y1][x1])
            else:
                values.append(0)
        print(values)

        # Perform interpolation
        interp_value, coefficients = bilinear_interpolation(orig_x, orig_y, values)
        print(interp_value, coefficients)
        # Update the output image
        output_image[i][j] = interp_value
        
        # Define arrow properties
        arrowprops = dict(arrowstyle='->', lw=1.5, color='red', connectionstyle='arc3, rad=-0.3')
        
        # Plot arrow
        axes[1].annotate("", xy=(j, i), xycoords=axes[1].transData, xytext=(orig_x, orig_y),
                         textcoords=axes[0].transData, arrowprops=arrowprops)

        # Draw 4 line connecting the point to the sampled neighbours
        # Use their opacity to represent their weight
        for k, (x1, y1) in enumerate(neighbours):
            axes[0].plot([orig_x, x1], [orig_y, y1], color='orange', alpha=coefficients[k], zorder=0)
            # Add a dot at the sampled neighbour
            axes[0].scatter(x1, y1, s=20, c='orange', zorder=10)

        # Add point at the start of the arrow
        axes[0].scatter(orig_x, orig_y, s=20, c='red', zorder=10)

# Update the output image
axes[1].imshow(output_image, cmap='gray')

# Save the figure
plt.savefig("undistortion_visual.pdf", bbox_inches='tight')
plt.savefig("undistortion_visual.png", bbox_inches='tight')

# Show the plots
plt.show()
