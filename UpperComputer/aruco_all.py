import math
import time

import serial
import cv2
import time
import threading
import numpy as np

# def find_maker(image):
KNOWN_DISTANCE = 40.0  # 在实际距离为40cm时测量
KNOWN_WIDTH = 4.8  # Aruco码的实际边长（正方形）
focalLength = 85 * KNOWN_DISTANCE / KNOWN_WIDTH  # 焦距——85为实拍图片中Aruco码的像素值
ser_port0 = "COM15"  # 根据实际情况而定

ser = serial.Serial(ser_port0, 115200, timeout=1, write_timeout=1)


def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


def points_distance(point0, point1):
    return math.sqrt(pow(point1[0] - point0[0], 2) + pow(point1[1] - point0[1], 2))


def str_adjust(input):
    str_input = str(int(input))
    if len(str_input) >= 3:
        return str_input[0:3]
    else:
        # zfill(a)函数是前补a位
        return str_input.zfill(3)


# 通过串口发送信息
# 可以完全自己更改，以下模式是根据接收端的协议而定
def send(cX, cY, distance):
    if distance >= 0:
        for i in range(1, 6):
            if i == 1:
                # time.sleep(1)
                ser.write(str(1).encode())
                print("receive:", ser.read())
            elif i == 2:
                # time.sleep(1)
                ser.write(str(2).encode())
                print("receive:", ser.read())
            elif i == 3:
                # time.sleep(1)
                for j in range(0, 3):
                    ser.write(str_adjust(cX)[j].encode())
                    print("receive:", ser.read())
            elif i == 4:
                # time.sleep(1)
                for j in range(0, 3):
                    ser.write(str_adjust(cY)[j].encode())
                    print("receive:", ser.read())
            else:
                # distance必须小于1000
                # time.sleep(1)
                for j in range(0, 3):
                    ser.write(str_adjust(int(distance))[j].encode())
                    print("receive:", ser.read())


def work():
    # 无人机IP和端口
    url = "http://192.168.1.1:80/av.asf?user=admin&pwd=&id=1&video=1&audio=0"
    # Aruco码初始化
    arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_50)  # 设置预定义的字典
    arucoParams = cv2.aruco.DetectorParameters_create()  # 使用默认值初始化检测器参数
    # 开启摄像头
    videoStream = cv2.VideoCapture(0)
    # 开启串口
    ser.flushInput()  # 清空缓冲区
    # (ret, frame) = videoStream.read()
    print(ser.isOpen())
    t = []
    t0 = threading.Thread(target=send, args=(0, 0, -1))
    t1 = threading.Thread(target=send, args=(0, 0, -1))
    t.append(t0)
    t.append(t1)
    t[0].start()
    while True:
        ret, frame = videoStream.read()  # 开始读取视频流
        cv2.imwrite("SaveImage.jpg", frame)  # 保存图片
        # detect aruco markers in frame
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # 先转为灰度图
        gray = cv2.GaussianBlur(gray, (5, 5), 0)  # 进行高斯降噪
        (corners, ids, rejected) = cv2.aruco.detectMarkers(gray, arucoDict, parameters=arucoParams)  # 获得aruco码的相关信息
        # find aruco
        if len(corners) > 0:  # 如果检测到aruco码
            print("waq,yeah")
            ids = ids.flatten()
            for (markerCorner, markerID) in zip(corners, ids):
                corners = markerCorner.reshape((4, 2))  # 得到四个顶点
                (topLeft, topRight, bottomRight, bottomLeft) = corners  # 将四个顶点分配
                topRight = (int(topRight[0]), int(topRight[1]))
                bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
                bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
                topLeft = (int(topLeft[0]), int(topLeft[1]))
                # every_edge_
                # 计算四条边的像素值
                lin1_dis = points_distance(topRight, bottomRight)
                lin2_dis = points_distance(bottomRight, bottomLeft)
                lin3_dis = points_distance(bottomLeft, topLeft)
                lin4_dis = points_distance(topLeft, topRight)
                # aruco码是正方形，求平均像素值mean_lin_dis来计算距离
                mean_lin_dis = (lin1_dis + lin2_dis + lin3_dis + lin4_dis) / 4
                # focalLength = mean_lin_dis * KNOWN_DISTANCE / KNOWN_WIDTH
                # 得到aruco码与相机的距离
                distance = distance_to_camera(KNOWN_WIDTH, focalLength, mean_lin_dis)
                # box aruco
                # 在视频界面上显示出aruco码的外接四边形
                cv2.line(frame, topLeft, topRight, (0, 255, 0), 2)
                cv2.line(frame, topRight, bottomRight, (0, 255, 0), 2)
                cv2.line(frame, bottomRight, bottomLeft, (0, 255, 0), 2)
                cv2.line(frame, bottomLeft, topLeft, (0, 255, 0), 2)

                # show aruco center
                # 中心点像素信息
                cX = int((topLeft[0] + bottomRight[0]) / 2.0)
                cY = int((topLeft[1] + bottomRight[1]) / 2.0)
                cv2.circle(frame, (cX, cY), 4, (0, 0, 255), -1)

                # aruco id
                cv2.putText(frame, str(markerID), (topLeft[0], topLeft[1] - 15),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (0, 0, 255), 2)
                cv2.putText(frame, 'distance:' + str(distance), (topLeft[0], topLeft[1] - 35),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 0, 0), 2)
                print("count done")
                # send
                t[0].join()
                t[0] = t[1]
                t[1] = threading.Thread(target=send, args=(cX, cY, distance))
                t[0].start()
                print("end")
        # cv2.imshow("gray", gray)
        cv2.imshow("frame", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()
    videoStream.release()
    ser.close()


if __name__ == '__main__':
    work()
