import ctypes
import time
import threading

dll = ctypes.CDLL("../bin/Release/screen.dll")

#Setup dll methods
dll.drawPixel.restype = None
dll.drawPixel.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_bool]
dll.drawLine.restype = None
dll.drawLine.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_bool]
dll.getCursorPos.restype = None
dll.getCursorPos.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
dll.cls.restype = None


while True:
    x = ctypes.c_int(0)
    y = ctypes.c_int(0)
    dll.getCursorPos(x, y)
    dll.drawLine(10, 10, x, y, 1, 0, 255, 0, False)
    time.sleep(1)
    dll.cls()