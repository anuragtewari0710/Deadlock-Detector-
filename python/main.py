import tkinter as tk
from ctypes import *
from gui import DeadlockGUI

# Load C++ library
lib = CDLL("./libdeadlock.so")

# Define function signatures
lib.init_system.argtypes = []
lib.init_system.restype = None

lib.add_resource.argtypes = [c_int, c_char_p, c_int]
lib.add_resource.restype = c_int

lib.add_process.argtypes = [c_int, c_char_p, POINTER(c_int), c_int]
lib.add_process.restype = c_int

lib.allocate_resource.argtypes = [c_int, c_int, c_int]
lib.allocate_resource.restype = c_int

lib.release_resource.argtypes = [c_int, c_int, c_int]
lib.release_resource.restype = c_int

lib.detect_deadlock.argtypes = [POINTER(c_int), c_int]
lib.detect_deadlock.restype = c_int

lib.check_safe_request.argtypes = [c_int, POINTER(c_int), c_int, POINTER(c_int), c_int]
lib.check_safe_request.restype = c_int

lib.get_process_count.argtypes = []
lib.get_process_count.restype = c_int

lib.get_resource_count.argtypes = []
lib.get_resource_count.restype = c_int

lib.get_process_info.argtypes = [c_int, POINTER(c_int), c_char_p, POINTER(c_int), POINTER(c_int), POINTER(c_int), c_int]
lib.get_process_info.restype = c_int

lib.get_resource_info.argtypes = [c_int, POINTER(c_int), c_char_p, POINTER(c_int), POINTER(c_int)]
lib.get_resource_info.restype = c_int

lib.clear_system.argtypes = []
lib.clear_system.restype = None

lib.cleanup_system.argtypes = []
lib.cleanup_system.restype = None

# Initialize
lib.init_system()

# Run GUI
root = tk.Tk()
app = DeadlockGUI(root, lib)
root.mainloop()

# Cleanup
lib.cleanup_system()
