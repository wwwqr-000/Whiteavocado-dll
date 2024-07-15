import time
import ctypes

listener = ctypes.CDLL("../bin/Release/listener.dll")

listener.key.restype = None
listener.key.argtypes = [ctypes.c_char_p, ctypes.c_bool]

listener.key(b"a", True)