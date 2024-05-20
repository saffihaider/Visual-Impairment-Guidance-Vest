import cv2
import numpy as np
import os
import matplotlib.pyplot as plt
from picamera2 import Picamera2, Preview
import time
import pygame
import math


# parameters

# define oriange
#lower_yellow = np.array([80, 200, 0])
#upper_yellow = np.array([100, 255, 255])
# define pink
lower_yellow = np.array([20, 50, 130])
upper_yellow = np.array([120, 120, 220])
lower_red1 = np.array([0, 120, 90])
upper_red1 = np.array([10, 255, 255])
lower_red2 = np.array([170, 120, 90])
upper_red2 = np.array([180, 255, 255])
lower_pink = np.array([160, 90, 90])
upper_pink = np.array([170, 255, 255])
# define dilate kernel
kernel = np.ones((9, 9), np.uint8)

def gray_world_white_balance(img):
    # Calculate the mean of each channel and then average them
    mean_val = np.mean(img, axis=(0, 1))
    gray_mean = np.mean(mean_val)
    
    # Calculate scale factors for each channel
    scale_factors = gray_mean / mean_val
    
    # Scale the channels
    balanced = img.copy()
    for i in range(3):
        balanced[:, :, i] = np.clip(balanced[:, :, i] * scale_factors[i], 0, 255)
    
    return balanced.astype(np.uint8)
def playSound(video):
    pygame.init()

    # 加载音频文件
    pygame.mixer.init()
    pygame.mixer.music.load(video)

    # 播放音频
    pygame.mixer.music.play()

    # 等待音频播放完成
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)


def cornerDet(x, y, matrix, thre):
    # 要查找最近的坐标点的目标点
    rows, cols = matrix.shape
    start_y = y + thre
    if start_y >= rows - 1:
        start_y = rows - 1

    if start_y < 0:
        start_y = 0

    start_x = x
    # 一组坐标点
    target_x = 0
    target_y = 0

    for col in range(int(start_x), int(cols - 1)):
        col = col + 1
        
        if matrix[int(start_y), int(col)] == 1:
            target_y = col
            break
        if (col == cols - 1) and (matrix[int(y), int(x + 20)] == 1):
            target_y = cols - 1

    for col in range(int(start_x), 0, -1):
        col = col - 1
        if matrix[int(start_y), int(col)] == 1:
            if abs(target_y - start_x) > abs(col - start_x):
                target_y = col
                break

    return target_y


def DetectRoad(image):
    width, height = 800, 600
    new_size = (width, height)

    # 调整图像到新的尺寸
    image = cv2.resize(image, new_size, interpolation=cv2.INTER_AREA)

    # 显示调整后的图像

    blurred_image = cv2.GaussianBlur(image, (9, 9), 0)  # do gaussian blur
    hsv = cv2.cvtColor(blurred_image, cv2.COLOR_BGR2HSV)  # image to hsv
    
# Equalize the V channel
    hsv[:, :, 2] = cv2.equalizeHist(hsv[:, :, 2])

# Convert back to BGR
    blurred_image = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)

    #mask = cv2.inRange(hsv, lower_yellow, upper_yellow)

    mask_red1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(hsv, lower_red2, upper_red2)

    mask_pink = cv2.inRange(hsv, lower_pink, upper_pink)

# Combine the masks
    mask = cv2.bitwise_or(mask_red1, mask_red2)
    mask = cv2.bitwise_or(mask, mask_pink)
# Combine the masks
    #mask = cv2.bitwise_or(mask1, mask2)
    mask = cv2.dilate(mask, kernel, iterations=3)
    cv2.imshow("ohh",mask)
    #cv2.imshow('Line Detected', mask)
    # find contours
    contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    filtered_mask = np.zeros_like(mask)
    for contour in contours:
        cv2.drawContours(filtered_mask, [contour], -1, 255, thickness=cv2.FILLED)

    # canny edge detection
    edges = cv2.Canny(filtered_mask, threshold1=30, threshold2=255)
    white_edges = cv2.bitwise_and(edges, edges, mask=filtered_mask)
    mask = cv2.dilate(white_edges, kernel, iterations=2)

    corners = cv2.goodFeaturesToTrack(white_edges, maxCorners=1, qualityLevel=0.5, minDistance=100)

    corner_x = 0
    corner_y = 0

    # 在原始图像上绘制角点
    if corners is not None:
        for corner in corners:
            corner_x, corner_y = corner.ravel()
            cv2.circle(image, (int(corner_x), int(corner_y)), 3, (0, 0, 255), -1)

    corner_x_ratio = corner_x / image.shape[1]
    corner_y_ratio = corner_y / image.shape[0]

    lines = cv2.HoughLinesP(mask, 1, np.pi / 180, 40, minLineLength=10, maxLineGap=10)

    # 创建用于绘制线条的图像
    line_image = np.copy(image) * 0  # 创建一个与原始图像大小相同的黑色图像

    # 绘制检测到的线
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            cv2.line(line_image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    combo_image = cv2.addWeighted(image, 0.8, line_image, 1, 0)
    oldmask = mask
    if corner_y >= 800:
        corner_y = 799
    if corner_x >= 600:
        corner_x = 599

    mask[mask != 0] = 1
    print(corner_x, corner_y, corner_x_ratio, corner_y_ratio)

    def angle_between_points(B, A, C):
        vector1 = np.array(A) - np.array(B)
        vector2 = np.array(C) - np.array(B)

        magnitude1 = np.linalg.norm(vector1)
        magnitude2 = np.linalg.norm(vector2)

        # Check if either magnitude is zero to avoid division by zero
        if magnitude1 == 0 or magnitude2 == 0:
            return 0

            # Calculate cosine
        cosine_theta = np.dot(vector1, vector2) / (magnitude1 * magnitude2)
        cosine_theta = np.clip(cosine_theta, -1.0, 1.0)

        # Calculate angle in radians
        angle_radians = np.arccos(cosine_theta)

        # Convert to degrees
        angle_degrees = np.degrees(angle_radians)

        return angle_degrees

    columns = mask.shape[1]
    rows = mask.shape[0]
    
    y_pos2 = 2*rows//9-1
    

    #if np.sum(mask[y_pos2])==0:
   #     playSound("helloworld.mp3")
    #    time.sleep(2)
    
        
        
    

    y_pos = 2 * rows / 3 - 1
    # print(y_pos,corner_y,'i')
    # newcxU = corner_y + 20
    # newcxD = corner_y - 20

    # newcyU = 0
    # counter1 = 0
    # for i < 600:
    #     if mask[newcxU,corner_x+i] == 1:
    #         break
    #     new

    # if corner_y >= y_pos:
    #     count = 0
    #     newcx = int(corner_x)
    #     newcy = int(corner_y)
    #     print(corner_x,corner_y)
    #     while count < 25:
    #         for i in range(0, 3):
    #             #if newcx == rows-1:
    #             #    break
    #             if mask[newcx + i - 1, newcy - count] == 1:
    #                 newcx = newcx + i - 1
    #                 newcy = newcy - count
    #                 break
    #         count += 1

    #     count2 = 0
    #     newcx2 = int(corner_x)
    #     newcy2 = int(corner_y)
    #     while count2 < 25:
    #         for i in range(0, 3):
    #             #if newcx2 == rows-1:
    #             #    break
    #             if mask[newcx2 + i - 1, newcy2 + count2] == 1:
    #                 newcx2 = newcx2 + i - 1
    #                 newcy2 = newcy2 + count2
    #                 break
    #         count2 += 1

    #     A1 = (int(corner_x), int(corner_y))
    #     A2 = (newcx, newcy)
    #     A3 = (newcx2, newcy2)
    #     print(A1,A2,A3)
    #     ang = angle_between_points(A1, A2, A3)
    #     print(ang)
    #     if ang > 60:
    #         if newcx < corner_x:
    #             turn = 'Turn left'
    #         if newcx > corner_x:
    #             turn = 'Turn right'
    #     else:
    #         turn="No turn"

    angle = 0
    moving = 0

    top_turn_check = int(3 * rows / 5)

    # checking for slope to compare line segments
    def calculate_slope(x1, y1, x2, y2):
        if x2 - x1 == 0:
            return float('inf')
        return (y2 - y1) / (x2 - x1)
    
    ###################################################################
    ###################################################################
    counter1 = [0, 0, 0]
    y_vals = [top_turn_check, int(5 * rows / 6), int(rows - (rows / 10))]
    for j in range(3):
        ##commented is checking left to middle
        # for i in range(0,columns):
        for i in range(columns // 2, -1, -1):
            if mask[y_vals[j]][i] == 1:
                break
        counter1[j] = i
        counter = 0

    # to check for right sidewalk edge, checking is starting in the middle to the right
    counter2 = [0, 0, 0]
    y_vals = [top_turn_check, int(5 * rows / 6), int(rows - (rows / 10))]
    for j in range(3):
        ##commented is checking rigght to middle
        # for i in range(columns-1,-1,-1):
        for i in range(columns // 2, columns):
            if mask[y_vals[j]][i] == 1:
                break

        counter2[j] = i
        counter = 0

    # averaging
    middle_array = [0, 0, 0]

    # checking for slope to compare line segments
    # def calculate_slope(x1, y1, x2, y2):
    #     if x2 - x1 == 0:
    #         return float('inf')
    #     return (y2 - y1) / (x2 - x1)

    # to see if it makes sense that the line segments belong to the same line
    def are_points_on_same_line(x1, y1, x2, y2, x3, y3):
        slope1 = calculate_slope(x1, y1, x2, y2)
        slope2 = calculate_slope(x2, y2, x3, y3)

        # threshold for the slope change
        if abs(slope1 - slope2) < 15:
            slope2 = slope1

        return slope1 == slope2

    can_move = False
    # if both side of sidewalk edges makes sense
    if are_points_on_same_line(counter1[0], y_vals[0], counter1[1], y_vals[1], counter1[2],
                                y_vals[2]) & are_points_on_same_line(counter2[0], y_vals[0], counter2[1], y_vals[1],
                                                                    counter2[2], y_vals[2]) == True:
        for i in range(len(counter1)):
            # take the average to be the middle
            middle_array[i] = int((counter1[i] + counter2[i]) / 2)
            can_move = True
    # take the left points for angle calculation if right measurements dont make sense
    if are_points_on_same_line(counter1[0], y_vals[0], counter1[1], y_vals[1], counter1[2], y_vals[2]) == False:
        middle_array = counter2
    # take the right points for angle calculation if right measurements dont make sense
    if are_points_on_same_line(counter2[0], y_vals[0], counter2[1], y_vals[1], counter2[2], y_vals[2]) == False:
        middle_array = counter1
        # creating the middle array
    
    A = (middle_array[0], rows - 1 - y_vals[0])
    B = (middle_array[2], rows - 1 - y_vals[2])
    C = (middle_array[2], rows - 1 - y_vals[2] + 50)

    
    placehold = int((counter1[0] + counter2[0])/2)

    trigger = 0
    new_index = -10
    for i in range(1,rows-250):
        if mask[top_turn_check-i][placehold] == 1:
            new_index = top_turn_check-i
            trigger = 1
            break
    
    
    checker = 1
    if trigger == 1:
        # for i in range(70,columns-21-placehold):
        #     print(new_index,placehold+i,mask[new_index][placehold+i],'yyyy')
        #     if mask[new_index][placehold+i] == 1:
        #         print(new_index,placehold+i,'fd')
        #         checker = 1
        #         break
        #     final = placehold +i

        # for i in range(0,new_index-1):
        #     if mask[new_index-i][columns-1] == 1:
        #         checker = 1

        for i in range(1,columns-1-placehold):
            if mask[new_index+100][placehold+i] == 1:
                checker = -1
                break


        if checker == 1:
            turn = 'Turn right'
            cv2.imshow('Lines Detected', combo_image)
            playSound("turnright.mp3")
            time.sleep(2)
        
        #elif np.sum(mask[y_pos2])==0:
         #   playSound("helloworld.mp3")
          #  turn = 'no corner'
           # time.sleep(2)
    
    
        elif checker == -1 and new_index != -10:
            if np.sum(mask[y_pos2])==0:
                #playSound("helloworld.mp3")
                turn = 'no corner'
                time.sleep(0.5)
            else:
                turn = 'Turn left'
                cv2.imshow('Lines Detected', combo_image)
                playSound("turnleft.mp3")
                time.sleep(2)
            







    ###################################################################
    ###################################################################
    # if corner_x + 10 - cornerDet(corner_x, corner_y, mask, -200) < -50:
    #     turn = 'Turn right'
    #     #cv2.imshow('Lines Detected', combo_image)
    #     playSound("turnright.mp3")
    #     time.sleep(0.2)
    # elif corner_x - 10 - cornerDet(corner_x, corner_y, mask, -200) > 50:
    #     turn = 'Turn left'
    #     #cv2.imshow('Lines Detected', combo_image)
    #     playSound("turnleft.mp3")
    #     time.sleep(0.2)
    else:

        turn = 'No corner'
        cv2.imshow('Lines Detected', combo_image)
        # to check for the left sidewalk edge, checking is starting in the middle to the left
        counter1 = [0, 0, 0]
        y_vals = [int(2 * rows / 3), int(5 * rows / 6), int(rows - (rows / 10))]
        for j in range(3):
            ##commented is checking left to middle
            # for i in range(0,columns):
            for i in range(columns // 2, -1, -1):
                if mask[y_vals[j]][i] == 1:
                    break
            counter1[j] = i
            counter = 0

        # to check for right sidewalk edge, checking is starting in the middle to the right
        counter2 = [0, 0, 0]
        y_vals = [int(2 * rows / 3), int(5 * rows / 6), int(rows - (rows / 10))]
        for j in range(3):
            ##commented is checking rigght to middle
            # for i in range(columns-1,-1,-1):
            for i in range(columns // 2, columns):
                if mask[y_vals[j]][i] == 1:
                    break

            counter2[j] = i
            counter = 0

        # averaging
        middle_array = [0, 0, 0]

        # checking for slope to compare line segments
        def calculate_slope(x1, y1, x2, y2):
            if x2 - x1 == 0:
                return float('inf')
            return (y2 - y1) / (x2 - x1)

        # to see if it makes sense that the line segments belong to the same line
        def are_points_on_same_line(x1, y1, x2, y2, x3, y3):
            slope1 = calculate_slope(x1, y1, x2, y2)
            slope2 = calculate_slope(x2, y2, x3, y3)

            # threshold for the slope change
            if abs(slope1 - slope2) < 15:
                slope2 = slope1

            return slope1 == slope2

        can_move = False
        # if both side of sidewalk edges makes sense
        if are_points_on_same_line(counter1[0], y_vals[0], counter1[1], y_vals[1], counter1[2],
                                   y_vals[2]) & are_points_on_same_line(counter2[0], y_vals[0], counter2[1], y_vals[1],
                                                                        counter2[2], y_vals[2]) == True:
            for i in range(len(counter1)):
                # take the average to be the middle
                middle_array[i] = int((counter1[i] + counter2[i]) / 2)
                can_move = True
        # take the left points for angle calculation if right measurements dont make sense
        if are_points_on_same_line(counter1[0], y_vals[0], counter1[1], y_vals[1], counter1[2], y_vals[2]) == False:
            middle_array = counter2
        # take the right points for angle calculation if right measurements dont make sense
        if are_points_on_same_line(counter2[0], y_vals[0], counter2[1], y_vals[1], counter2[2], y_vals[2]) == False:
            middle_array = counter1
            # creating the middle array
        A = (middle_array[0], rows - 1 - y_vals[0])
        B = (middle_array[2], rows - 1 - y_vals[2])
        C = (middle_array[2], rows - 1 - y_vals[2] + 50)

        # variable to see which direction to move
        move = int(columns / 2) - middle_array[2]

        # threshold variable to check if they should move left or right
        threshold = 50

        # if either edge check is bad, ignore moving left or right this time
        if can_move == False:
            # used very large number instead of Boolean for easy manipulation
            # moving = 100000000000
            moving = 'Take next measurement'
        if move < -threshold:
            # if need to right number is 1
            # moving = 1
            moving = 'Move to the right'
            playSound("moveright.mp3")
        if move > threshold:
            # if need to move left number is -1
            # moving = -1
            moving = 'Move to the left'
            playSound("moveleft.mp3")
        else:
            # stay the course is 0
            # moving = 0
            moving = 'You are in the middle'
            time.sleep(0.2)
            playSound("staymid.mp3")

        # print(angle_between_points(B,A, C), moving)
        angle = angle_between_points(B, A, C)
        if middle_array[0] < middle_array[2]:
            angle = -angle

        cv2.imshow('Lines Detected', combo_image)

        # return angle_between_points(A, B, C), moving
    return corner_x_ratio, corner_y_ratio, oldmask, turn, angle, moving

    # to visualize angle checking
    # plt.imshow(mask, cmap='gray', interpolation='none')
    # plt.show()


if __name__ == "__main__":


# Set volume to 70%
    
    
    picam2 = Picamera2()
    camera_config = picam2.create_preview_configuration()
    picam2.configure(camera_config)

    picam2.start()

    try:
        while True:
            
            # 创建一个无限循环来持续处理图像
            image = picam2.capture_array()  # 从摄像头捕获图像
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            
            # 将图像传递给 DetectRoad 函数并处理
            corner_x_ratio, corner_y_ratio, oldmask, turn, angle, moving = DetectRoad(image)

            print(turn,'\n...\n...\n...\n' '\n', 'Walk at a', angle, 'degree angle', '\n', moving)

            # 这里可以添加一个按键检测，以便在按下特定键时退出循环
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except KeyboardInterrupt:
        # 如果在终端中按下了 Ctrl+C，那么捕获这个异常以优雅地退出循环
        print("Exiting loop")

    # 循环结束后，清理工作
    picam2.stop_preview()
    picam2.close()



