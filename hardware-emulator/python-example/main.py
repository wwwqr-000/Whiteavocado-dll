import ctypes
import time

dll = ctypes.CDLL("../bin/Release/hardware-emulator.dll")

dll.beep.restype = None
dll.beep.argtypes = [ctypes.c_char_p]
dll.key.restype = None
dll.key.argtypes = [ctypes.c_char_p]
dll.mouse.restype = None
dll.mouse.argtypes = [ctypes.c_char_p]

time.sleep(1)
dll.mouse(b"left_down")
dll.mouse(b"left_up")
dll.beep(b"click")