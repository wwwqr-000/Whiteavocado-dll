import ctypes
import time

dll = ctypes.CDLL("../bin/Release/hardware-emulator.dll")

dll.beep.restype = None
dll.beep.argtypes = [ctypes.c_char_p]
dll.key.restype = None
dll.key.argtypes = [ctypes.c_char_p]
dll.mouseKey.restype = None
dll.mouseKey.argtypes = [ctypes.c_char_p]
dll.mouseTP.restype = None
dll.mouseTP.argtypes = [ctypes.c_int, ctypes.c_int]
dll.mouseMove.restype = None
dll.mouseMove.argtypes = [ctypes.c_int, ctypes.c_int]

time.sleep(2)
dll.mouseKey(b"left_down")
dll.mouseKey(b"left_up")
dll.beep(b"click")
time.sleep(0.3)
dll.mouseTP(10, 10)
time.sleep(0.3)
dll.mouseTP(10, 120)
dll.mouseMove(10, 0)

dll.mouseMove(360, 0)