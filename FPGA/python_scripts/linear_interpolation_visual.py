import matplotlib.pyplot as plt
from matplotlib.patches import ConnectionPatch
import numpy as np


def bilinear_interpolation(x, y, values):
    f11, f12, f21, f22 = values

    x_frac = abs(x) - abs(int(x))
    y_frac = abs(y) - abs(int(y))
    one_minus_x_frac = 1 - x_frac
    one_minus_y_frac = 1 - y_frac
    if x < 0:
        x_frac, one_minus_x_frac = one_minus_x_frac, x_frac
    if y < 0:
        y_frac, one_minus_y_frac = one_minus_y_frac, y_frac
    coeff_tl = one_minus_x_frac * one_minus_y_frac
    coeff_tr = x_frac * one_minus_y_frac
    coeff_bl = one_minus_x_frac * y_frac
    coeff_br = x_frac * y_frac

    return (f11 * coeff_tl +
            f12 * coeff_tr +
            f21 * coeff_bl +
            f22 * coeff_br), [coeff_tl, coeff_tr, coeff_bl, coeff_br]

def get_neighbours(x, y):
    if -1 < x < 0:
        x = -1
    if -1 < y < 0:
        y = -1
    return [(int(x), int(y)),
            (int(x) + 1, int(y)),
            (int(x), int(y) + 1),
            (int(x) + 1, int(y) + 1)]

if __name__ == '__main__':
    # Create a synthetic image
    image = np.array([[1, 2, 3],
                    [4, 5, 6],
                    [7, 8, 9]])

    # Select interpolation point
    x = 1.33
    y = 1.25

    # Nearest neighbors
    points = get_neighbours(x, y)
    values = [image[int(y1), int(x1)] for x1, y1 in points]

    # Perform interpolation
    interp_value, coefficients = bilinear_interpolation(x, y, values)

    # Create subplots
    fig, axes = plt.subplots(1, 2, figsize=(16, 8))

    # Plot original image on the left subplot
    axes[0].imshow(image, vmin=1, vmax=9, cmap='gray')

    # Mark the points used for interpolation
    for px, py in points:
        axes[0].scatter(px, py, c='r', marker='o', s=200)
        axes[0].text(px + 0.1, py - 0.05, f"({px},{py})", fontsize=14, color='blue', weight='bold')

    # Mark the interpolation point
    axes[0].scatter(x, y, c='b', marker='x', s=200)
    axes[0].text(x + 0.1, y - 0.05, f"({x},{y}) = {interp_value:.2f}", fontsize=14, color='blue', weight='bold')

    # Add coefficients
    for (px, py), coef in zip(points, coefficients):
        axes[0].text(px - 0.45, py - 0.2, f"coeff={coef:.2f}", fontsize=16, color='darkred')

    # Add values
    for (px, py), val in zip(points, values):
        axes[0].text(px - 0.45, py - 0.3, val, fontsize=24, color='darkred')

    axes[0].set_title("Original Image")

    # Create output image
    output_image = np.zeros_like(image)
    output_image[2, 2] = interp_value

    # Plot output image on the right subplot
    axes[1].imshow(output_image, vmin=1, vmax=9, cmap='gray')
    axes[1].set_title("Output Image with Interpolated Pixel")

    # Add the final interpolated value on the right subplot
    axes[1].text(2 - 0.45, 2 - 0.3, f"{int(interp_value)}", fontsize=24, color='darkred', zorder=0)

    # Add curved arrow pointing to new position
    ab = [axes[0], axes[1]]
    print(axes[0].get_xlim())
    print(axes[0].get_ylim())
    xyA = (x, y)
    xyB = (2.0, 2.0)
    con = ConnectionPatch(xyA=xyA, xyB=xyB, coordsA="data", coordsB="data", axesA=ab[0], axesB=ab[1],
                        arrowstyle="-|>", color="red", lw=2, connectionstyle="arc3,rad=-0.3")
    ab[1].add_artist(con)
    plt.savefig('/Users/zach-mcc/KTH/Accerion-Thesis/Thesis Writing/Figures/My Figures/Bilinear Interpolation/bilinear_interpolation.pdf')
    plt.savefig('/Users/zach-mcc/KTH/Accerion-Thesis/Thesis Writing/Figures/My Figures/Bilinear Interpolation/bilinear_interpolation.png', transparent=True)
    plt.show()
