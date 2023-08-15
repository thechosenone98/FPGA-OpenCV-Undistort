import cv2

# Open the default camera
cap = cv2.VideoCapture(0)

# Check if camera opened successfully
if not cap.isOpened():
    print("Error opening video capture")
    exit()

# Loop to read and display frames
while True:
    # Read frame from camera
    ret, frame = cap.read()

    # Check if frame was successfully read
    if not ret:
        print("Error reading frame")
        break

    # Display the frame
    cv2.imshow("Frame", frame)

    # Exit if 'q' key is pressed
    if cv2.waitKey(1) == ord('q'):
        break

# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()
