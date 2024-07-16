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
dll.msgBox.restype = None
dll.msgBox.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]

result = ctypes.pointer(ctypes.c_char_p())

def grs():
    return result.contents.value.decode('utf-8')

def dcp():
    x = ctypes.c_int(0)
    y = ctypes.c_int(0)
    dll.getCursorPos(x, y)
    dll.drawLine(10, 10, x, y, 1, 0, 255, 0, False)
    time.sleep(1)
    dll.cls()

for x in range(2):
    dcp();

dll.msgBox(b"Feedback", b"Did you like those lines?", b"yn", b"q", result)

if (grs() == "no"):
    dll.msgBox(b":(", b"Then I will draw another one anyway!", b"o", b"s", result)
    dcp()
else:
    dll.msgBox(b":)", b"Thank you very much!", b"o", b"a", result)

