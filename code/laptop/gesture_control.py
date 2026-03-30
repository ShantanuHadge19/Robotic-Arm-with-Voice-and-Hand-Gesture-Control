import cv2
import mediapipe as mp
import numpy as np
import serial
import time
import math

# Initialize serial connection
arduinoData = serial.Serial('COM3', 115200)
time.sleep(2)  # Allow time for the serial connection to initialize

def send_signal_to_arduino(M, X, B, A, x, y):
    """Send formatted signal to Arduino."""
    print(f'Xf={x},Yf={y}')
    signal = f"{M},{X},{B},{A},{x},{y}\r"
    arduinoData.write(signal.encode())



# Initialize MediaPipe Hand Tracking
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.7)
mp_drawing = mp.solutions.drawing_utils

# Start Video Capture
cap = cv2.VideoCapture(0)

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

a = 1
M = 0
X = 0
B = 90
A = 120

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break


    frame = cv2.flip(frame, 1)  # Flip for natural mirroring

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.05, 8, minSize=(120,120))
    for (x, y, w, h) in faces:
         cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 5)
         xf=x
         yf=y
         send_signal_to_arduino(M, X, B, A, int(xf),int(yf))
        
    

    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb_frame)

    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            # Extract landmarks
            wrist = hand_landmarks.landmark[0]  # Wrist (Point 0)
            thumbtip = hand_landmarks.landmark[4]
            indextip = hand_landmarks.landmark[8]

            # Convert normalized coordinates to pixel values
            x0, y0 = int(wrist.x * frame.shape[1]), int(wrist.y * frame.shape[0])
            x4, y4 = int(thumbtip.x * frame.shape[1]), int(thumbtip.y * frame.shape[0])
            x8, y8 = int(indextip.x * frame.shape[1]), int(indextip.y * frame.shape[0])

            # Midpoint between thumbtip and indextip
            cx, cy = (x4 + x8) // 2, (y4 + y8) // 2
            cv2.circle(frame, (cx, cy), 5, (255, 0, 255), cv2.FILLED)

            # Compute distance between thumb and index tip
            length = math.hypot(x8 - x4, y8 - y4)

            # Get Z-coordinates for depth comparison
            wrist_z = wrist.z
            thumbtip_z = thumbtip.z

            # Determine hand position
            if thumbtip_z > wrist_z:
                hand_position = "Bent Forward"
                direction = "UP"
                color = (0, 0, 255)  # Red
            else:
                hand_position = "Bent Backward"
                direction = "DOWN"
                color = (0, 255, 0)  # Green

                #for (x, y, w, h) in faces:
                   #cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 5)
                  # xf=x
                  # yf=y
                   

                if length < 35:
                    M = 10  # Close grip
                else:
                    M = 0   # Open grip

                # Compute angle using vector from wrist to thumbtip
                vector_0_12 = np.array([x4 - x0, y4 - y0])
                reference_vector = np.array([0, -1])  # Upward direction

                dot_product = np.dot(vector_0_12, reference_vector)
                magnitude_0_12 = np.linalg.norm(vector_0_12)
                magnitude_ref = np.linalg.norm(reference_vector)

                # Avoid division by zero
                if magnitude_0_12 > 0 and magnitude_ref > 0:
                    cosine_angle = np.clip(dot_product / (magnitude_0_12 * magnitude_ref), -1.0, 1.0)
                    angle = np.arccos(cosine_angle) * (180.0 / np.pi)
                else:
                    angle = 0

                # Map angle to servo range
                servo_angle = np.interp(angle, [0, 180], [0, 180])
                y = a * servo_angle

                # Send to Arduino
                X = int(y)
                B = int(x0)
                A = int(y0)
                send_signal_to_arduino(M, X, B, A,xf,yf)
                print(f'M={M}, Angle={y:.2f}, X0={x0}')

            # Display hand status
            cv2.putText(frame, f"{hand_position} ({direction})", (50, 50), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
            #cv2.putText(frame, f"Angle: {servo_angle:.2f}", (50, 80), 
                        #cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 0), 2)

            # Draw circles
            cv2.circle(frame, (x0, y0), 7, (255, 0, 0), -1)  
            cv2.circle(frame, (x4, y4), 7, (0, 255, 0), -1)  

            # Draw hand landmarks
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
    
    
    cv2.imshow("Hand Bend Detection", frame)

    # Press 'Q' to exit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
 
