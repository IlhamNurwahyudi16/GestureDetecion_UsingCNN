import numpy as np
from keras.models import model_from_json
import operator
import cv2
import time
import socket

target_ip = "192.168.1.10" # IP Address of the ESP8266 Server.
target_port = 4210
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Loading the model
json_file = open("Latih.json", "r")
model_json = json_file.read()
json_file.close()
loaded_model = model_from_json(model_json)
# load weights into new model
loaded_model.load_weights("Latih.h5")
print("Loaded model from disk")

#cap = cv2.VideoCapture(0)
cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)

def empty(a):
    pass
cv2.namedWindow("HSV")
cv2.resizeWindow("HSV",640,240)
cv2.createTrackbar("HUE Min","HSV",0,179,empty)
cv2.createTrackbar("SAT Min","HSV",0,255,empty)
cv2.createTrackbar("VALUE Min","HSV",0,255,empty)
cv2.createTrackbar("HUE Max","HSV",179,179,empty)
cv2.createTrackbar("SAT Max","HSV",255,255,empty)
cv2.createTrackbar("VALUE Max","HSV",255,255,empty)

fps_start_time = 0
fps = 0

while True:
    _, frame = cap.read()  
    frame2 = frame
    fps_end_time = time.time()
    time_diff = fps_end_time - fps_start_time
    fps = 1/(time_diff)
    fps_start_time = fps_end_time
    fps_text = int(fps)
    # Simulating mirror image
    frame = cv2.flip(frame, 1)
    frame2 = cv2.flip(frame2, 1)
    cv2.putText(frame2, str(fps_text), (5, 50), cv2.FONT_HERSHEY_COMPLEX, 1, (0,255,255),1)
    #Ger HSV Value from trackbar
    h_min = cv2.getTrackbarPos("HUE Min", "HSV")
    s_min = cv2.getTrackbarPos("SAT Min", "HSV")
    v_min = cv2.getTrackbarPos("VALUE Min", "HSV")
    h_max = cv2.getTrackbarPos("HUE Max", "HSV")
    s_max = cv2.getTrackbarPos("SAT Max", "HSV")
    v_max = cv2.getTrackbarPos("VALUE Max", "HSV")
    lower = np.array([h_min, s_min, v_min])
    upper = np.array([h_max, s_max, v_max])

    #Konversi RGB ke HSV
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #Menggunakan range untuk kapture frame diantara lower dan upper
    mask = cv2.inRange(hsv_frame, lower, upper)
    #Segmentasi Warna
    res = cv2.bitwise_and(frame, frame, mask=mask)
    #Invert
    invert_image = cv2.bitwise_not(mask)
    data_gesture = cv2.resize(invert_image,(100,100))
    #Melakukan Prediksi
    result = loaded_model.predict(data_gesture.reshape(1, 100, 100, 1))
    prediction = {'KOSONG': result[0][0], 
                  'STOP': result[0][1], 
                  'MAJU': result[0][2],
                  'MUNDUR': result[0][3],
                  'KANAN': result[0][4],
                  'KIRI': result[0][5]}
    # Sorting based on top prediction
    prediction = sorted(prediction.items(), key=operator.itemgetter(1), reverse=True)
    if prediction[0][0]=='KOSONG':
        client.sendto(b"STOP", (target_ip, target_port))
    elif prediction[0][0]=='MAJU':
        client.sendto(b"MAJU", (target_ip, target_port))
    elif prediction[0][0]=='KANAN':
        client.sendto(b"KANAN", (target_ip, target_port))
    elif prediction[0][0]=='KIRI':
        client.sendto(b"KIRI", (target_ip, target_port))
    elif prediction[0][0]=='STOP':
        client.sendto(b"STOP", (target_ip, target_port))
    elif prediction[0][0]=='MUNDUR':
        client.sendto(b"MUNDUR", (target_ip, target_port))

    # Displaying the predictions
    cv2.putText(frame2, prediction[0][0], (500, 50), cv2.FONT_HERSHEY_COMPLEX, 1, (0,255,255), 1)
    cv2.imshow("Frame", frame2)
    cv2.imshow("Thresholding", invert_image)
    interrupt = cv2.waitKey(10)
    if interrupt & 0xFF == 27: # esc key
        break
        
 
cap.release()
cv2.destroyAllWindows()
