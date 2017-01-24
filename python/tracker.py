import numpy as np
from time import sleep, clock
import cv2
import win32api
import socket
from bacterium import bacterium
import pyHook, pythoncom
from subprocess import call
from PIL import ImageGrab


def onMouse(event, x, y, flag, param):
    if isinstance(param, tracker) == False:
        return
    if event == cv2.EVENT_LBUTTONDOWN:
        param.changePos(2 * x, 2 * y)


def getContrast(inframe):
    a = len(np.shape(inframe))
    if len(np.shape(inframe)) != 2:
        return

    width, height = np.shape(inframe)
    width -= 2
    height -= 2

    center = inframe[1 : height, 1 : width]
    left = inframe[1 : height, 0 : width - 1]
    right = inframe[1 : height, 2 : width + 1]
    up = inframe[0 : height - 1, 1 : width]
    down = inframe[2 : height + 1, 1 : width]

    res = abs(center - left)  + abs(center - right) ** 2 + abs(center - up) ** 2\
          + abs(center - down) ** 2

    return res.mean()


class Region:
    def __init__(self):
        self.start = []
        self.end = []


class tracker():
    def __init__(self, bact, max_x, max_y):
        self.bact = bact
        if isinstance(bact, bacterium) == False:
            print("Error")
            return
        self.isinited = True
        self.outofFocus = False
        self.isLost = True
        self.xrange = [0, 0]
        self.yrange = [0, 0]
        self.max_x = max_x
        self.max_y = max_y
        self.ischangeing = False
        self.contrast_prev = 0
        self.dire_up = False
        self.isgreat = True

    def changePos(self, x, y):
        self.bact.x = x
        self.bact.y = y
        self.isLost = False
        self.outofFocus = False
        self.ischangeing = True
        self.setRange()
        self.ischangeing = False

    def update(self, frame):
        if self.isinited == False or self.isLost == True or self.ischangeing == True:
            return 0

        ylow = self.yrange[0]
        yhigh = self.yrange[1]
        xlow = self.xrange[0]
        xhigh = self.xrange[1]

        region = frame[ylow : yhigh, xlow: xhigh]

        # region = region.astype(np.double) ** 2
        # region = region - region.mean()
        # region = 255 * (region / region.max())
        # region = region.astype(np.uint8)

        res = np.where(region >= region.max() - 3)
        resy = res[0]
        resx = res[1]
        posx = resx.mean()
        posy = resy.mean()

        self.bact.x = self.xrange[0] + posx
        self.bact.y = self.yrange[0] + posy
        self.setRange()

        # print(self.xrange, self.yrange, self.bact.x, self.bact.y)

        contra = getContrast(region)
        if  contra< 30:
            self.outofFocus = True
        else:
            self.outofFocus = False
        return contra

    def auto_focus(self, contrast):
        if self.outofFocus == False:
            return

        if contrast > self.contrast_prev:
            self.isgreat = True
        else:
            self.isgreat = False

        if self.isgreat == True:
            if self.dire_up == True:
                self.movez(10)
            else:
                self.movez(-10)
        else:
            self.dire_up = not self.dire_up
            if self.dire_up == True:
                self.movez(10)
            else:
                self.movez(-10)


    def movez(self, step):
        call(["Stagez.exe", str(step)])

    def setRange(self):
        x = int(self.bact.x)
        y = int(self.bact.y)

        if x - self.bact.width > 0:
            self.xrange[0] = x - self.bact.width;
        else:
            self.isLost = True
            return

        if x + self.bact.width < self.max_x:
            self.xrange[1] = x + self.bact.width
        else:
            self.isLost = True
            return

        if y - self.bact.width > 0:
            self.yrange[0] = y - self.bact.width
        else:
            self.isLost = True
            return

        if y + self.bact.width < self.max_y:
            self.yrange[1] = y + self.bact.width
        else:
            self.isLost = True
            return


def LeftDown(event):
    print("here")
    global cnt
    global region
    if event.MessageName == 'mouse left down':
        if cnt == 0:
            region.start = event.Position
        elif cnt == 1:
            region.end = event.Position
        cnt += 1
    return True



if __name__ == '__main__':
    # font = cv2.FONT_HERSHEY_SIMPLEX
    # hm = pyHook.HookManager()
    # hm.MouseLeftDown = LeftDown
    # cnt = 0
    # region = Region()
    # hm.HookMouse()
    # while(cnt < 2):
    #     pythoncom.PumpWaitingMessages()

    # hm.UnhookMouse()
    # grab_r = (region.start[0], region.start[1], region.end[0], region.end[1])
    cap = cv2.VideoCapture(r"D:\Users\Neil Zhao\Documents\Tencent Files\1055306580\FileRecv\1021\great.avi")
    ret, frame = cap.read()
    y, x, channel = np.shape(frame)
    track = tracker(bacterium(30), grab_r[3]-grab_r[1], grab_r[2] - grab_r[0])
    # track = tracker(bacterium(30), x, y)
    cv2.namedWindow('test')
    cv2.setMouseCallback('test', onMouse, track)
    contrast = 0

    while(1):
        # ret, frame = cap.read()
        # if frame is None:
        #     break
        frame = ImageGrab.grab(bbox=(grab_r))
        frame = np.array(frame)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # gray = gray.astype(np.double) ** 2
        # gray = gray - gray.mean()
        # gray = 255 * (gray / gray.max())
        # gray = gray.astype(np.uint8)
        #
        # frame = frame.astype(np.double) ** 2
        # frame = frame - frame.mean()
        # frame = 255 * (frame / frame.max())
        # frame = frame.astype(np.uint8)
        # kernel = np.ones((track.max_y, track.max_y), np.float32) / (track.max_y * track.max_y)
        # gray = cv2.GaussianBlur(gray,(5, 5),0)
        # frame = cv2.GaussianBlur(frame, (5, 5), 0)

        contrast = track.update(gray)

        if track.outofFocus == True:
            cv2.putText(frame, "Out of focus", (10, 250), font, 1.2, (0, 255, 255), 2, cv2.LINE_AA)
            cv2.rectangle(frame, (track.xrange[0], track.yrange[0]), (track.xrange[1], track.yrange[1]), (255, 0, 0), 3)
            # track.auto_focus(contrast)
        elif track.isLost == True:
            cv2.putText(frame, "Bacterium lost", (10, 200), font, 1.2, (0, 0, 255), 2, cv2.LINE_AA)
        else:
            cv2.putText(frame, "2D fixed", (10, 200), font, 1.2, (0, 255, 0), 2, cv2.LINE_AA)
            cv2.rectangle(frame, (track.xrange[0], track.yrange[0]), (track.xrange[1], track.yrange[1]), (255, 0, 0), 3)

        cv2.putText(frame, "x=%.2f, y=%.2f, contrast=%.2f" % (track.bact.x, track.bact.y, contrast), (10, 50), font, 1.2,
                        (255, 255, 255), 2, cv2.LINE_AA)
        frame = cv2.resize(frame, None, fx=0.5, fy=0.5, interpolation = cv2.INTER_CUBIC)
        cv2.imshow('test', frame)
        if cv2.waitKey(30) & 0xFF == 27:
            break

    cv2.destroyAllWindows()