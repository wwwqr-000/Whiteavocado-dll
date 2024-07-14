import ctypes
import time
import threading

dll = ctypes.CDLL("../bin/Release/hardware-emulator.dll")

dll.beep.restype = None
dll.beep.argtypes = [ctypes.c_char_p]
dll.key.restype = None
dll.key.argtypes = [ctypes.c_char_p, ctypes.c_int]
dll.mouseKey.restype = None
dll.mouseKey.argtypes = [ctypes.c_char_p]
dll.mouseTP.restype = None
dll.mouseTP.argtypes = [ctypes.c_int, ctypes.c_int]
dll.mouseMove.restype = None
dll.mouseMove.argtypes = [ctypes.c_int, ctypes.c_int]

time.sleep(5)

def sprint(duration):
    dll.key(b"w", 50)
    time.sleep(0.05)
    dll.key(b"w", duration)

def one_block_forward():
    dll.key(b"w", 230)

def elytra_startup():
    t1 = threading.Thread(target=sprint, args=(100,))
    t1.start();
    dll.key(b"space", 100)
    time.sleep(0.02)
    dll.key(b"space", 100)
    dll.mouseKey(b"right_down")
    dll.mouseKey(b"right_up")
    t1.join()

#elytra_startup()

dll.mouseKey(b"left_down")
time.sleep(1)
dll.mouseKey(b"left_up")
elytra_startup()

#for x in range(100):
#    dll.mouseKey(b"left_down")
#    dll.mouseKey(b"left_up")
#    time.sleep(0.005)

#dll.mouseKey(b"left_down")
#time.sleep(1)
#dll.mouseKey(b"left_up")

#for x in range(20):
#    dll.key(b"w", 230)
#    time.sleep(0.2)