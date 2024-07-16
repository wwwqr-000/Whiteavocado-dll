import ctypes
import time
import sys
import os

#Check for pyinstaller enabled for included dll
if getattr(sys, 'frozen', False):
    dll_path = f"{sys._MEIPASS}/file-manager.dll"
else:
    dll_path = "../bin/Release/file-manager.dll"

#Build command: pyinstaller --onefile --add-data "../bin/Release/file-manager.dll;." main.py

file_manager = ctypes.CDLL(dll_path)

file_manager.moveSelfStartup.restype = ctypes.c_int
file_manager.moveSelfStartup.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

print(file_manager.moveSelfStartup(b"C:/Users/za2qa/Desktop/background-apps/Whiteavocado-dll/file-manager/python-example/marf/", b"File-Test"))
os.system("start installer.cmd")
time.sleep(5)
os.remove("installer.cmd")
time.sleep(10)

#file_manager.access.restype = ctypes.c_bool
#file_manager.access.argtypes = [ctypes.c_char_p]
#
#if file_manager.access(b"./test.txt") == False:
#    print("No access to file.\n")
#else:
#    print("You have access to the file.\n")
#
