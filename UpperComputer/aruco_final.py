import binascii
import math
import time

import serial
import cv2
import time
import threading
import os
import numpy as np

camera_matrix = np.array(
    [[4.6429354704e+02, 0., 3.1333423645e+02], [0., 4.6885993446e+02, 1.4059063087e+02], [0., 0., 1.]])
dist_matrix = np.array([0.05826439, 0.136195, -0.00314291, -0.00313936, -0.23242677])

save_dir = "C:\\Users\\86183\\Desktop\\test\\"  # 图片保存路径
KNOWN_DISTANCE = 40.0  # 在实际距离为40cm时测量
KNOWN_WIDTH = 4.8  # Aruco码的实际边长（正方形）
font = cv2.FONT_HERSHEY_SIMPLEX
focalLength = 85 * KNOWN_DISTANCE / KNOWN_WIDTH  # 焦距——85为实拍图片中Aruco码的像素值
ser_port0 = "COM16"  # 根据实际情况而定
ser = serial.Serial(ser_port0, 115200, timeout=1)  # 初始化串口


# 根据像素值计算aruco码到相机的距离
def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


#
def covert(point, z=1):
    point = np.array(point, dtype=np.float)
    pts_uv = cv2.undistortPoints(point, camera_matrix, dist_matrix) * z
    return pts_uv[0][0]


# 计算像素点的距离
def points_distance(point0, point1):
    return math.sqrt(pow(point1[0] - point0[0], 2) + pow(point1[1] - point0[1], 2))


# 将输入的数字转为二进制16位形式（字符串）
def str_adjust_16(input):
    str_input = bin(input)[2:]
    if len(str_input) < 16:
        return str_input.zfill(16)


# 勾股定理计算垂直距离
def height(cDistance, camX, camY):
    return 100 * math.sqrt(pow(cDistance / 100, 2) - pow(camX, 2) - pow(camY, 2))


# 将输入的二进制字符串转为数字
def str_bin_to_int(str):
    num = 0
    for i in range(len(str)):
        num = num + int(str[i]) * pow(2, len(str) - 1 - i)
    return num


def signal(X):
    if X >= 0:
        return 0
    else:
        return 1


# 通过串口发送信息
# 可以完全自己更改，以下模式是根据接收端的协议而定
def send(camX, camY, distance, id):
    pack_mes0 = bytearray()
    pack_mes0.append(0xAA)
    pack_mes0.append(0xAF)
    pack_mes0.append(0x52)
    pack_mes0.append(10)
    pack_mes0.append(0x01)
    signX = signal(camX)
    cX_0 = str_bin_to_int(str_adjust_16(int(abs(camX)))[0:8])
    cX_1 = str_bin_to_int(str_adjust_16(int(abs(camX)))[8:])
    signY = signal(camY)
    cY_0 = str_bin_to_int(str_adjust_16(int(abs(camY)))[0:8])
    cY_1 = str_bin_to_int(str_adjust_16(int(abs(camY)))[8:])
    dis_0 = str_bin_to_int(str_adjust_16(int(distance))[0:8])
    dis_1 = str_bin_to_int(str_adjust_16(int(distance))[8:])
    pack_mes0.append(signX)
    pack_mes0.append(cX_0)
    pack_mes0.append(cX_1)
    pack_mes0.append(signY)
    pack_mes0.append(cY_0)
    pack_mes0.append(cY_1)
    pack_mes0.append(dis_0)
    pack_mes0.append(dis_1)
    pack_mes0.append(id)
    pack_mes0.append(str_bin_to_int(
        str_adjust_16(0xAA + 0xAF + 0x52 + 10 + 1 + signX + cX_0 + cX_1 + signY + cY_0 + cY_1 + dis_0 + dis_1 + id)[
        8:]))
    ser.write(pack_mes0)
    # print("receive:", ser.readlines())


def work():
    # 无人机IP和端口
    url = "http://192.168.1.1:80/av.asf?user=admin&pwd=&id=1&video=1&audio=0"
    # Aruco码初始化
    arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_250)  # 设置预定义的字典
    arucoParams = cv2.aruco.DetectorParameters_create()  # 使用默认值初始化检测器参数
    # 开启摄像头
    videoStream = cv2.VideoCapture(0)
    # 开启串口
    ser.flushInput()  # 清空缓冲区
    t = []
    t0 = threading.Thread(target=send, args=(0, 0, 0, 0))
    t1 = threading.Thread(target=send, args=(0, 0, 0, 0))
    t.append(t0)
    t.append(t1)
    num = 0
    t[0].start()
    while True:
        ret, frame = videoStream.read()  # 开始读取视频流
        frame = cv2.undistort(frame, camera_matrix, dist_matrix)
        cv2.imwrite("SaveImage.jpg", frame)  # 保存图片
        # detect aruco markers in frame
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # 先转为灰度图
        gray = cv2.GaussianBlur(gray, (5, 5), 0)  # 进行高斯降噪
        (corners, ids, rejected) = cv2.aruco.detectMarkers(gray, arucoDict, parameters=arucoParams)  # 获得aruco码的相关信息
        # find aruco
        if len(corners) > 0:  # 如果检测到aruco码
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

                # aruco degree
                rvec, tvec, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorner, 0.048, camera_matrix, dist_matrix)
                (rvec - tvec).any()  # get rid of that nasty numpy value array error
                # 旋转矩阵到欧拉角
                R = np.zeros((3, 3), dtype=np.float64)
                cv2.Rodrigues(rvec, R)
                # aruco id
                cv2.putText(frame, str(markerID), (topLeft[0], topLeft[1] - 15),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (0, 0, 255), 2)
                cv2.putText(frame, 'distance:' + str(distance), (topLeft[0], topLeft[1] - 30),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 0, 0), 2)
                cv2.putText(frame, 'pixel:' + str(mean_lin_dis), (topLeft[0], topLeft[1] - 70),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 0, 0), 2)
                cam_points_x = tvec[0][0][0]
                cam_points_y = tvec[0][0][1]
                cv2.putText(frame, 'height:' + str(height(distance, cam_points_x, cam_points_y)),
                            (topLeft[0], topLeft[1] - 50),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (0, 0, 255), 2)
                # send
                t[0].join()
                t[0] = t[1]
                t[1] = threading.Thread(target=send,
                                        args=(int(cam_points_x * 100), int(cam_points_y * 100), distance, markerID))
                print("camX:", cam_points_x * 100)
                print("camY:", cam_points_y * 100)
                print("dis:", distance)
                print("id:", markerID)
                t[0].start()
        else:
            t[0].join()
            t[0] = t[1]
            t[1] = threading.Thread(target=send, args=(0, 0, 0, 0))
            t[0].start()
        cv2.imshow("frame", frame)
        if cv2.waitKey(1) & 0xFF == ord('c'):
            cv2.imwrite(os.path.join(save_dir + str(num) + '.jpg'), frame)
            num += 1
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cv2.destroyAllWindows()
    videoStream.release()


if __name__ == '__main__':
    work()
