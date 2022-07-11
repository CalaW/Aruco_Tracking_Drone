import cv2
import numpy as np
import math

url = "http://192.168.1.1:80/av.asf?user=admin&pwd=&id=1&video=1&audio=0"

save_dir = "C:\\Users\\dyh20200207\\Desktop\\test"

arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_50)
arucoParams = cv2.aruco.DetectorParameters_create()

videoStream = cv2.VideoCapture(url)

font = cv2.FONT_HERSHEY_SIMPLEX

camera_matrix = np.array([[4.6429354704e+02, 0., 3.1333423645e+02],[0., 4.6885993446e+02, 1.4059063087e+02],[0., 0., 1.]])
dist_matrix = np.array([0.05826439, 0.136195, -0.00314291, -0.00313936, -0.23242677])


(ret, frame) = videoStream.read()
num = 0
while ret:
    ret, frame = videoStream.read()
    # num += 1
    # print(num)
    # if num%100 ==0:
    #     cv2.imwrite(save_dir+'\\'+'%d.jpg'%num, frame)

    # detect aruco markers in frame
    (corners, ids, rejected) = cv2.aruco.detectMarkers(frame, arucoDict, parameters=arucoParams)

    # find aruco
    if len(corners) > 0:
        ids = ids.flatten()
        for (markerCorner, markerID) in zip(corners, ids):
            corners = markerCorner.reshape((4, 2))
            (topLeft, topRight, bottomRight, bottomLeft) = corners
            topRight = (int(topRight[0]), int(topRight[1]))
            bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
            bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
            topLeft = (int(topLeft[0]), int(topLeft[1]))

            # box aruco
            cv2.line(frame, topLeft, topRight, (0, 255, 0), 2)
            cv2.line(frame, topRight, bottomRight, (0, 255, 0), 2)
            cv2.line(frame, bottomRight, bottomLeft, (0, 255, 0), 2)
            cv2.line(frame, bottomLeft, topLeft, (0, 255, 0), 2)

            # show aruco center
            cX = int((topLeft[0] + bottomRight[0]) / 2.0)
            cY = int((topLeft[1] + bottomRight[1]) / 2.0)
            cv2.circle(frame, (cX, cY), 4, (0, 0, 255), -1)

            # calculate size
            # aruco_X = math.sqrt((topRight[0]-topLeft[0])**2 + (topRight[1]-topLeft[1])**2)
            # aruco_Y = math.sqrt((topRight[0]-bottomRight[0])**2 + (topRight[1]-bottomRight[1])**2)
            # aruco_size = aruco_X * aruco_Y
            # depth = math.sqrt(float(aruco_size) / float(benchmark_size)) * benchmark_depth
            #
            # estimate depth
            rvec, tvec, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorner, 0.048, camera_matrix, dist_matrix)
            (rvec - tvec).any()  # get rid of that nasty numpy value array error

            deg = rvec[0][0][2] / math.pi * 180
            # deg=rvec[0][0][2]/math.pi*180*90/104
            # 旋转矩阵到欧拉角
            R = np.zeros((3, 3), dtype=np.float64)
            cv2.Rodrigues(rvec, R)
            sy = math.sqrt(R[0, 0] * R[0, 0] + R[1, 0] * R[1, 0])
            singular = sy < 1e-6
            if not singular:  # 偏航，俯仰，滚动
                x = math.atan2(R[2, 1], R[2, 2])
                y = math.atan2(-R[2, 0], sy)
                z = math.atan2(R[1, 0], R[0, 0])
            else:
                x = math.atan2(-R[1, 2], R[1, 1])
                y = math.atan2(-R[2, 0], sy)
                z = 0
                # 偏航，俯仰，滚动换成角度
            rx = x * 180.0 / 3.141592653589793
            ry = y * 180.0 / 3.141592653589793
            rz = z * 180.0 / 3.141592653589793

            cv2.putText(frame, 'deg_z:' + str(ry) + str('deg'), (0, 140), font, 1, (0, 255, 0), 2,
                        cv2.LINE_AA)

            # estimate distance
            # distance = ((tvec[0][0][2] + 0.02) * 0.0254) * 100  # 单位是米
            print(tvec)
            distance = (tvec[0][0][2]) * 1.5

            cv2.putText(frame, 'distance:' + str(round(distance, 4)) + str('m'), (0, 110), font, 1, (0, 255, 0), 2,
                        cv2.LINE_AA)

            # aruco id
            cv2.putText(frame, str(markerID), (topLeft[0], topLeft[1] - 15),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.5, (0, 255, 0), 2)

    cv2.imshow("frame", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cv2.destroyAllWindows()
videoStream.release()
