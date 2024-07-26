import time
import ctypes

dll = ctypes.CDLL("../bin/Release/listener.dll")

dll.keyListener.restype = ctypes.c_int
dll.keyListener.argtypes = [ctypes.c_char_p]

stat = ctypes.create_string_buffer(256)#The value that gets used as sttatus for executing the command.

resultVKCode = dll.keyListener(stat)

print("VK-code:")
print(resultVKCode)