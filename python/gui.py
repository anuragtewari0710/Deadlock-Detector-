import tkinter as tk
from tkinter import messagebox
from ctypes import *

class DeadlockGUI:
    def __init__(self, root, lib):
        self.root = root
        self.lib = lib
        self.root.title("Deadlock Detection")
        self.root.geometry("700x500")
        self.n_proc = 0
        self.n_res = 0
        self.build()
        
    def build(self):
        tk.Label(self.root, text="Deadlock System", font=("Arial", 14, "bold")).pack(pady=10)
        
        # Resource
        f1 = tk.Frame(self.root)
        f1.pack(pady=5)
        tk.Label(f1, text="Resource:").pack(side=tk.LEFT)
        self.r_name = tk.Entry(f1, width=10)
        self.r_name.pack(side=tk.LEFT)
        self.r_inst = tk.Entry(f1, width=5)
        self.r_inst.pack(side=tk.LEFT)
        tk.Button(f1, text="Add", command=self.add_res, bg="green", fg="white").pack(side=tk.LEFT)
        
        # Process
        f2 = tk.Frame(self.root)
        f2.pack(pady=5)
        tk.Label(f2, text="Process:").pack(side=tk.LEFT)
        self.p_name = tk.Entry(f2, width=10)
        self.p_name.pack(side=tk.LEFT)
        tk.Label(f2, text="Max:").pack(side=tk.LEFT)
        self.p_max = tk.Entry(f2, width=10)
        self.p_max.pack(side=tk.LEFT)
        tk.Button(f2, text="Add", command=self.add_proc, bg="blue", fg="white").pack(side=tk.LEFT)
        
        # Allocate
        f3 = tk.Frame(self.root)
        f3.pack(pady=5)
        tk.Label(f3, text="PID:").pack(side=tk.LEFT)
        self.pid = tk.Entry(f3, width=5)
        self.pid.pack(side=tk.LEFT)
        tk.Label(f3, text="RID:").pack(side=tk.LEFT)
        self.rid = tk.Entry(f3, width=5)
        self.rid.pack(side=tk.LEFT)
        tk.Label(f3, text="Amt:").pack(side=tk.LEFT)
        self.amt = tk.Entry(f3, width=5)
        self.amt.pack(side=tk.LEFT)
        tk.Button(f3, text="Allocate", command=self.alloc, bg="orange", fg="white").pack(side=tk.LEFT, padx=2)
        tk.Button(f3, text="Release", command=self.rel, bg="purple", fg="white").pack(side=tk.LEFT)
        
        # Buttons
        f4 = tk.Frame(self.root)
        f4.pack(pady=10)
        tk.Button(f4, text="Detect Deadlock", command=self.detect, bg="red", fg="white").pack(side=tk.LEFT, padx=3)
        tk.Button(f4, text="Check Safe", command=self.safe, bg="teal", fg="white").pack(side=tk.LEFT, padx=3)
        tk.Button(f4, text="Status", command=self.status, bg="gray", fg="white").pack(side=tk.LEFT)
        
        # Log
        self.log = tk.Text(self.root, height=20, bg="black", fg="green")
        self.log.pack(padx=20, pady=10, fill=tk.BOTH, expand=True)
        self.log.insert(tk.END, "Ready!\n")
        
    def add_res(self):
        n = self.r_name.get()
        i = int(self.r_inst.get())
        self.lib.add_resource(self.n_res, n.encode(), i)
        self.log.insert(tk.END, f"Added R{self.n_res}: {n}\n")
        self.n_res += 1
        
    def add_proc(self):
        n = self.p_name.get()
        m = [int(x) for x in self.p_max.get().split(",")]
        arr = (len(m) * c_int)(*m)
        self.lib.add_process(self.n_proc, n.encode(), arr, len(m))
        self.log.insert(tk.END, f"Added P{self.n_proc}: {n}\n")
        self.n_proc += 1
        
    def alloc(self):
        p, r, a = int(self.pid.get()), int(self.rid.get()), int(self.amt.get())
        self.lib.allocate_resource(p, r, a)
        self.log.insert(tk.END, f"P{p} allocated {a} of R{r}\n")
        
    def rel(self):
        p, r, a = int(self.pid.get()), int(self.rid.get()), int(self.amt.get())
        self.lib.release_resource(p, r, a)
        self.log.insert(tk.END, f"P{p} released {a} of R{r}\n")
        
    def detect(self):
        arr = (100 * c_int)()
        c = self.lib.detect_deadlock(arr, 100)
        if c > 0:
            pids = [arr[i] for i in range(c)]
            self.log.insert(tk.END, f"DEADLOCK: {pids}\n")
            messagebox.showwarning("Deadlock!", f"Processes: {pids}")
        else:
            self.log.insert(tk.END, "No deadlock\n")
            messagebox.showinfo("Safe", "No deadlock!")
            
    def safe(self):
        seq = (100 * c_int)()
        req = (self.n_res * c_int)(*[0] * self.n_res)
        r = self.lib.check_safe_request(0, req, self.n_res, seq, 100)
        if r > 0:
            s = [seq[i] for i in range(r)]
            self.log.insert(tk.END, f"Safe: {s}\n")
            messagebox.showinfo("Safe", f"Seq: {s}")
        else:
            self.log.insert(tk.END, "UNSAFE\n")
            
    def status(self):
        self.log.insert(tk.END, f"\nProcesses: {self.n_proc}, Resources: {self.n_res}\n")
