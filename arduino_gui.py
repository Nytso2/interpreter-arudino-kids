#!/usr/bin/env python3

import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext, ttk
import os
import subprocess
import threading
from pathlib import Path

class ArduinoKidsCompilerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🤖 Arduino Kids Programming Language Compiler")
        self.root.geometry("900x650")  # Fixed size that fits everything
        self.root.minsize(800, 600)    # Minimum size
        self.root.configure(bg="#f0f0f0")
        
        # Variables
        self.current_file = None
        self.compiler_path = "./inter"  # Path to your compiled interpreter
        
        self.setup_ui()
        self.check_compiler()
    
    def setup_ui(self):
        # Main title - smaller height
        title_frame = tk.Frame(self.root, bg="#2c3e50", height=60)
        title_frame.pack(fill="x", padx=0, pady=0)
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(title_frame, 
                              text="🤖 Arduino Kids Programming Language",
                              font=("Arial", 14, "bold"),
                              fg="white", bg="#2c3e50")
        title_label.pack(expand=True)
        
        subtitle_label = tk.Label(title_frame,
                                 text="Easy Arduino Programming for Kids",
                                 font=("Arial", 9),
                                 fg="#ecf0f1", bg="#2c3e50")
        subtitle_label.pack()
        
        # Main content with scrollable area
        main_canvas = tk.Canvas(self.root, bg="#f0f0f0")
        scrollbar = ttk.Scrollbar(self.root, orient="vertical", command=main_canvas.yview)
        scrollable_frame = tk.Frame(main_canvas, bg="#f0f0f0")
        
        scrollable_frame.bind(
            "<Configure>",
            lambda e: main_canvas.configure(scrollregion=main_canvas.bbox("all"))
        )
        
        main_canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        main_canvas.configure(yscrollcommand=scrollbar.set)
        
        main_canvas.pack(side="left", fill="both", expand=True, padx=10, pady=5)
        scrollbar.pack(side="right", fill="y")
        
        # File selection frame - compact
        file_frame = tk.LabelFrame(scrollable_frame, text="📁 Select Program", 
                                  font=("Arial", 10, "bold"), bg="#f0f0f0")
        file_frame.pack(fill="x", pady=(0, 5))
        
        # File drop area - smaller
        self.drop_frame = tk.Frame(file_frame, bg="#e8f4fd", relief="ridge", bd=2, height=50)
        self.drop_frame.pack(fill="x", padx=5, pady=5)
        self.drop_frame.pack_propagate(False)
        
        self.drop_label = tk.Label(self.drop_frame,
                                  text="📝 Click 'Browse Files' to select your robot program",
                                  font=("Arial", 9),
                                  fg="#3498db", bg="#e8f4fd")
        self.drop_label.pack(expand=True)
        
        # File selection buttons - smaller
        button_frame = tk.Frame(file_frame, bg="#f0f0f0")
        button_frame.pack(fill="x", padx=5, pady=(0, 5))
        
        self.browse_btn = tk.Button(button_frame, text="📂 Browse",
                                   command=self.browse_file,
                                   font=("Arial", 9, "bold"),
                                   bg="#3498db", fg="white",
                                   relief="flat", padx=10, pady=5)
        self.browse_btn.pack(side="left", padx=(0, 5))
        
        self.new_btn = tk.Button(button_frame, text="✨ New",
                                command=self.create_new,
                                font=("Arial", 9, "bold"),
                                bg="#27ae60", fg="white",
                                relief="flat", padx=10, pady=5)
        self.new_btn.pack(side="left", padx=(0, 5))
        
        self.examples_btn = tk.Button(button_frame, text="📚 Examples",
                                     command=self.load_example,
                                     font=("Arial", 9, "bold"),
                                     bg="#f39c12", fg="white",
                                     relief="flat", padx=10, pady=5)
        self.examples_btn.pack(side="left")
        
        # Current file display
        self.file_label = tk.Label(file_frame, text="No file selected",
                                  font=("Arial", 8), fg="#7f8c8d", bg="#f0f0f0")
        self.file_label.pack(padx=5, pady=(0, 5))
        
        # Code editor frame - optimized height
        editor_frame = tk.LabelFrame(scrollable_frame, text="✏️ Robot Program Code", 
                                    font=("Arial", 10, "bold"), bg="#f0f0f0")
        editor_frame.pack(fill="both", expand=True, pady=(0, 5))
        
        # Code text area - perfect size
        self.code_text = scrolledtext.ScrolledText(editor_frame,
                                                  wrap=tk.WORD,
                                                  width=70, height=15,  # Optimal height
                                                  font=("Consolas", 9),
                                                  bg="#2c3e50", fg="#ecf0f1",
                                                  insertbackground="white",
                                                  selectbackground="#3498db")
        self.code_text.pack(fill="both", expand=True, padx=5, pady=5)
        
        # Add welcome code
        welcome_code = '''// Welcome to Arduino Kids Programming!
print "Hello Robot World!"
turn_on 13
wait 1000
blink 13 3
beep 8 500
print "My first robot program!"'''
        
        self.code_text.insert("1.0", welcome_code)
        
        # Compilation frame - compact but complete
        compile_frame = tk.LabelFrame(scrollable_frame, text="⚙️ Compile Your Program", 
                                     font=("Arial", 10, "bold"), bg="#f0f0f0")
        compile_frame.pack(fill="x", pady=(0, 5))
        
        # Compile controls - single row
        compile_control_frame = tk.Frame(compile_frame, bg="#f0f0f0")
        compile_control_frame.pack(fill="x", padx=5, pady=5)
        
        self.compile_btn = tk.Button(compile_control_frame, text="🚀 COMPILE TO ARDUINO",
                                    command=self.compile_program,
                                    font=("Arial", 11, "bold"),
                                    bg="#e74c3c", fg="white",
                                    relief="flat", padx=20, pady=8)
        self.compile_btn.pack(side="left")
        
        # Progress bar
        self.progress = ttk.Progressbar(compile_control_frame, mode='indeterminate', length=150)
        self.progress.pack(side="left", padx=(10, 0))
        
        # Options - compact
        self.show_code_var = tk.BooleanVar(value=False)
        self.show_code_check = tk.Checkbutton(compile_control_frame, text="Show Arduino Code",
                                             variable=self.show_code_var,
                                             font=("Arial", 8),
                                             bg="#f0f0f0")
        self.show_code_check.pack(side="right")
        
        # Output area - compact but scrollable
        output_label = tk.Label(compile_frame, text="📋 Output:",
                               font=("Arial", 9, "bold"), bg="#f0f0f0")
        output_label.pack(anchor="w", padx=5, pady=(5, 2))
        
        self.output_text = scrolledtext.ScrolledText(compile_frame,
                                                    wrap=tk.WORD,
                                                    width=70, height=8,  # Perfect height
                                                    font=("Consolas", 8),
                                                    bg="#2c3e50", fg="#ecf0f1")
        self.output_text.pack(fill="x", padx=5, pady=(0, 5))
        
        # Status bar
        self.status_bar = tk.Label(self.root, text="Ready to compile Arduino programs!",
                                  relief="sunken", anchor="w",
                                  font=("Arial", 8), bg="#ecf0f1", fg="#2c3e50")
        self.status_bar.pack(side="bottom", fill="x")
    
    def browse_file(self):
        file_path = filedialog.askopenfilename(
            title="Select Arduino Kids Program",
            filetypes=[
                ("Text files", "*.txt"),
                ("All files", "*.*")
            ]
        )
        
        if file_path:
            self.load_file(file_path)
    
    def load_file(self, file_path):
        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                content = file.read()
                self.code_text.delete("1.0", tk.END)
                self.code_text.insert("1.0", content)
                
            self.current_file = file_path
            self.file_label.config(text=f"📄 {os.path.basename(file_path)}")
            self.status_bar.config(text=f"Loaded: {file_path}")
            
        except Exception as e:
            messagebox.showerror("Error", f"Could not load file:\n{str(e)}")
    
    def create_new(self):
        response = messagebox.askyesno("New Program", 
                                      "Create a new program? This will clear the current code.")
        if response:
            self.code_text.delete("1.0", tk.END)
            self.code_text.insert("1.0", '''// New Arduino Kids Program
print "Starting my new robot!"
turn_on 13
wait 1000
print "Robot is ready!"''')
            self.current_file = None
            self.file_label.config(text="New program (unsaved)")
            self.status_bar.config(text="Created new program")
    
    def load_example(self):
        examples = {
            "🔥 Blinking LED": '''// Blinking LED Example
print "LED Demo Starting!"
turn_on 13
wait 1000
blink 13 5
turn_off 13
print "LED Demo Complete!"''',
            
            "🎵 Robot Sounds": '''// Robot Sounds Example
print "Making robot sounds..."
beep 8 500
wait 500
beep 8 300
wait 500
beep 8 700
print "Sound demo complete!"''',
            
            "🤖 Servo Motor": '''// Servo Motor Example
print "Moving robot arm..."
move_servo 9 0
wait 1000
move_servo 9 90
wait 1000
move_servo 9 180
print "Servo demo complete!"''',
            
            "📡 Smart Robot": '''// Smart Robot Example
print "Smart robot starting!"
repeat 3 {
    move_servo 9 45
    beep 8 300
    wait 1000
}
print "Mission complete!"''',
            
            "🌡️ Temperature": '''// Temperature Monitor
print "Checking temperature..."
read_temperature 2
wait 2000
beep 8 200
print "Temperature check done!"'''
        }
        
        # Create selection dialog
        example_window = tk.Toplevel(self.root)
        example_window.title("📚 Choose an Example")
        example_window.geometry("400x300")
        example_window.configure(bg="#f0f0f0")
        
        tk.Label(example_window, text="Select an Example Program:",
                font=("Arial", 10, "bold"), bg="#f0f0f0").pack(pady=10)
        
        listbox = tk.Listbox(example_window, font=("Arial", 9), height=8)
        listbox.pack(fill="both", expand=True, padx=15, pady=10)
        
        for example_name in examples.keys():
            listbox.insert(tk.END, example_name)
        
        def load_selected():
            selection = listbox.curselection()
            if selection:
                example_name = listbox.get(selection[0])
                self.code_text.delete("1.0", tk.END)
                self.code_text.insert("1.0", examples[example_name])
                self.file_label.config(text=f"Example: {example_name}")
                self.status_bar.config(text=f"Loaded example: {example_name}")
                example_window.destroy()
        
        tk.Button(example_window, text="✅ Load Example", command=load_selected,
                 font=("Arial", 9, "bold"), bg="#27ae60", fg="white",
                 relief="flat", padx=15, pady=8).pack(pady=10)
    
    def check_compiler(self):
        if not os.path.exists(self.compiler_path):
            self.status_bar.config(text="⚠️ Compiler not found! Please compile inter.c first")
            self.compile_btn.config(state="disabled")
            messagebox.showwarning("Compiler Not Found", 
                                 f"Arduino interpreter not found!\n\n"
                                 "Please compile first:\ngcc -o inter inter.c -lm")
        else:
            self.status_bar.config(text="✅ Compiler ready!")
    
    def save_current_code(self):
        temp_file = "temp_program.txt"
        try:
            code = self.code_text.get("1.0", tk.END)
            with open(temp_file, 'w', encoding='utf-8') as file:
                file.write(code)
            return temp_file
        except Exception as e:
            messagebox.showerror("Error", f"Could not save program:\n{str(e)}")
            return None
    
    def compile_program(self):
        threading.Thread(target=self._compile_thread, daemon=True).start()
    
    def _compile_thread(self):
        temp_file = self.save_current_code()
        if not temp_file:
            return
        
        # Update UI
        self.root.after(0, lambda: self.progress.start())
        self.root.after(0, lambda: self.compile_btn.config(state="disabled"))
        self.root.after(0, lambda: self.output_text.delete("1.0", tk.END))
        self.root.after(0, lambda: self.output_text.insert(tk.END, "🚀 Compiling your robot program...\n"))
        self.root.after(0, lambda: self.status_bar.config(text="Compiling..."))
        
        try:
            # Prepare command
            cmd = [self.compiler_path]
            if self.show_code_var.get():
                cmd.append("--dev")
            cmd.append(temp_file)
            
            # Run compiler
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            
            # Process output
            output = result.stdout + result.stderr
            
            # Update UI with results
            self.root.after(0, lambda: self.progress.stop())
            self.root.after(0, lambda: self.compile_btn.config(state="normal"))
            
            if result.returncode == 0:
                # Success!
                self.root.after(0, lambda: self.output_text.insert(tk.END, "✅ SUCCESS! Arduino code generated!\n\n"))
                self.root.after(0, lambda: self.output_text.insert(tk.END, output))
                self.root.after(0, lambda: self.status_bar.config(text="✅ Compilation successful!"))
                
                # Check for Arduino file
                arduino_file = "arduino_kids_program.ino"
                if os.path.exists(arduino_file):
                    self.root.after(0, lambda: self.output_text.insert(tk.END, f"\n📁 Arduino file: {arduino_file}\n"))
                    self.root.after(0, lambda: self.output_text.insert(tk.END, "🎯 Ready to upload to Arduino!\n"))
                    
                    # Success message
                    self.root.after(0, lambda: messagebox.showinfo("🎉 Success!", 
                                                                  f"Arduino code generated!\n\n"
                                                                  f"📁 File: {arduino_file}\n\n"
                                                                  f"Next steps:\n"
                                                                  f"1. Open Arduino IDE\n"
                                                                  f"2. Load the .ino file\n"
                                                                  f"3. Upload to your Arduino!"))
                
            else:
                # Error
                self.root.after(0, lambda: self.output_text.insert(tk.END, "❌ COMPILATION FAILED\n\n"))
                self.root.after(0, lambda: self.output_text.insert(tk.END, output))
                self.root.after(0, lambda: self.status_bar.config(text="❌ Compilation failed"))
                self.root.after(0, lambda: messagebox.showerror("❌ Compilation Error", 
                                                               f"Your program has errors:\n\n{output[:200]}"))
        
        except subprocess.TimeoutExpired:
            self.root.after(0, lambda: self.progress.stop())
            self.root.after(0, lambda: self.compile_btn.config(state="normal"))
            self.root.after(0, lambda: self.output_text.insert(tk.END, "❌ Compilation timed out!\n"))
            self.root.after(0, lambda: self.status_bar.config(text="❌ Timeout"))
            
        except Exception as e:
            self.root.after(0, lambda: self.progress.stop())
            self.root.after(0, lambda: self.compile_btn.config(state="normal"))
            self.root.after(0, lambda: self.output_text.insert(tk.END, f"❌ Error: {str(e)}\n"))
            self.root.after(0, lambda: self.status_bar.config(text="❌ Error"))
        
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.remove(temp_file)

def main():
    root = tk.Tk()
    app = ArduinoKidsCompilerGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
