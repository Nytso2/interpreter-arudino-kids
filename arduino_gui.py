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
        self.root.geometry("800x600")
        self.root.configure(bg="#f0f0f0")
        
        # Variables
        self.current_file = None
        self.compiler_path = "./inter"  # Path to your compiled interpreter
        
        self.setup_ui()
        self.check_compiler()
    
    def setup_ui(self):
        # Main title
        title_frame = tk.Frame(self.root, bg="#2c3e50", height=60)
        title_frame.pack(fill="x")
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(title_frame, 
                              text="🤖 Arduino Kids Programming Compiler",
                              font=("Arial", 16, "bold"),
                              fg="white", bg="#2c3e50")
        title_label.pack(expand=True)
        
        # Main content frame
        main_frame = tk.Frame(self.root, bg="#f0f0f0")
        main_frame.pack(fill="both", expand=True, padx=15, pady=15)
        
        # File selection frame
        file_frame = tk.LabelFrame(main_frame, text="📁 Select Your Robot Program", 
                                  font=("Arial", 10, "bold"), bg="#f0f0f0")
        file_frame.pack(fill="x", pady=(0, 10))
        
        # File selection area
        self.drop_frame = tk.Frame(file_frame, bg="#e8f4fd", relief="solid", bd=1, height=80)
        self.drop_frame.pack(fill="x", padx=10, pady=10)
        self.drop_frame.pack_propagate(False)
        
        self.drop_label = tk.Label(self.drop_frame,
                                  text="📄 Click 'Browse Files' to select your .txt program file",
                                  font=("Arial", 10),
                                  fg="#3498db", bg="#e8f4fd")
        self.drop_label.pack(expand=True)
        
        # File selection buttons
        button_frame = tk.Frame(file_frame, bg="#f0f0f0")
        button_frame.pack(fill="x", padx=10, pady=(0, 10))
        
        self.browse_btn = tk.Button(button_frame, text="📂 Browse Files",
                                   command=self.browse_file,
                                   font=("Arial", 10, "bold"),
                                   bg="#3498db", fg="white",
                                   relief="flat", padx=15, pady=8)
        self.browse_btn.pack(side="left", padx=(0, 10))
        
        self.examples_btn = tk.Button(button_frame, text="📚 Load Example",
                                     command=self.load_example,
                                     font=("Arial", 10, "bold"),
                                     bg="#f39c12", fg="white",
                                     relief="flat", padx=15, pady=8)
        self.examples_btn.pack(side="left")
        
        # Current file display
        self.file_label = tk.Label(file_frame, text="No file selected",
                                  font=("Arial", 9), fg="#7f8c8d", bg="#f0f0f0")
        self.file_label.pack(padx=10, pady=(0, 10))
        
        # Code editor frame
        editor_frame = tk.LabelFrame(main_frame, text="✏️ Robot Program Code", 
                                    font=("Arial", 10, "bold"), bg="#f0f0f0")
        editor_frame.pack(fill="both", expand=True, pady=(0, 10))
        
        # Code text area
        self.code_text = scrolledtext.ScrolledText(editor_frame,
                                                  wrap=tk.WORD,
                                                  width=70, height=12,
                                                  font=("Consolas", 10),
                                                  bg="#2c3e50", fg="#ecf0f1",
                                                  insertbackground="white")
        self.code_text.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Add sample code
        sample_code = '''// Welcome to Arduino Kids Programming!
print "Hello Robot World!"
turn_on 13
wait 1000
blink 13 3
beep 8 500
print "My first robot program!"'''
        
        self.code_text.insert("1.0", sample_code)
        
        # Compilation frame
        compile_frame = tk.LabelFrame(main_frame, text="⚙️ Compilation", 
                                     font=("Arial", 10, "bold"), bg="#f0f0f0")
        compile_frame.pack(fill="x", pady=(0, 10))
        
        # Compile button and options
        compile_control_frame = tk.Frame(compile_frame, bg="#f0f0f0")
        compile_control_frame.pack(fill="x", padx=10, pady=10)
        
        self.compile_btn = tk.Button(compile_control_frame, text="🚀 Compile to Arduino",
                                    command=self.compile_program,
                                    font=("Arial", 12, "bold"),
                                    bg="#e74c3c", fg="white",
                                    relief="flat", padx=25, pady=12)
        self.compile_btn.pack(side="left")
        
        # Progress bar
        self.progress = ttk.Progressbar(compile_control_frame, mode='indeterminate')
        self.progress.pack(side="left", fill="x", expand=True, padx=(15, 0))
        
        # Options
        options_frame = tk.Frame(compile_control_frame, bg="#f0f0f0")
        options_frame.pack(side="right")
        
        self.show_code_var = tk.BooleanVar(value=False)
        self.show_code_check = tk.Checkbutton(options_frame, text="Show Arduino C++ Code",
                                             variable=self.show_code_var,
                                             font=("Arial", 8),
                                             bg="#f0f0f0")
        self.show_code_check.pack()
        
        # Output area
        self.output_text = scrolledtext.ScrolledText(compile_frame,
                                                    wrap=tk.WORD,
                                                    width=70, height=6,
                                                    font=("Consolas", 9),
                                                    bg="#2c3e50", fg="#ecf0f1")
        self.output_text.pack(fill="x", padx=10, pady=(0, 10))
        
        # Status bar
        self.status_bar = tk.Label(self.root, text="Ready to compile Arduino programs!",
                                  relief="sunken", anchor="w",
                                  font=("Arial", 9), bg="#ecf0f1", fg="#2c3e50")
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
    
    def load_example(self):
        examples = {
            "🔥 Blinking LED": '''// Blinking LED Example
print "LED Demo Starting!"
turn_on 13
wait 1000
blink 13 5
print "LED Demo Complete!"''',
            
            "🎵 Robot Sounds": '''// Robot Sounds Example
print "Robot making sounds..."
beep 8 500
wait 500
beep 8 300
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
    read_distance 7 6
    beep 8 300
    wait 1000
}
print "Mission complete!"''',
            
            "🌡️ Temperature": '''// Temperature Monitoring
print "Checking temperature..."
read_temperature 2
wait 2000
beep 8 200
print "Temperature check done!"'''
        }
        
        # Create selection dialog
        example_window = tk.Toplevel(self.root)
        example_window.title("📚 Choose an Example")
        example_window.geometry("350x250")
        example_window.configure(bg="#f0f0f0")
        
        tk.Label(example_window, text="Select an Example Program:",
                font=("Arial", 11, "bold"), bg="#f0f0f0").pack(pady=10)
        
        listbox = tk.Listbox(example_window, font=("Arial", 10), height=6)
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
                 font=("Arial", 10, "bold"), bg="#27ae60", fg="white",
                 relief="flat", padx=15, pady=8).pack(pady=10)
    
    def check_compiler(self):
        # Check if the Arduino Kids interpreter is available
        if not os.path.exists(self.compiler_path):
            self.status_bar.config(text="⚠️ Compiler not found! Please compile inter.c first")
            self.compile_btn.config(state="disabled")
            messagebox.showwarning("Compiler Not Found", 
                                 f"Arduino Kids interpreter not found!\n\n"
                                 "Please compile your interpreter first:\n"
                                 "gcc -o inter inter.c -lm")
        else:
            self.status_bar.config(text="✅ Compiler ready!")
    
    def save_current_code(self):
        # Save current code to a temporary file
        temp_file = "temp_arduino_program.txt"
        try:
            code = self.code_text.get("1.0", tk.END)
            with open(temp_file, 'w', encoding='utf-8') as file:
                file.write(code)
            return temp_file
        except Exception as e:
            messagebox.showerror("Error", f"Could not save temporary file:\n{str(e)}")
            return None
    
    def compile_program(self):
        # Start compilation in a separate thread
        threading.Thread(target=self._compile_thread, daemon=True).start()
    
    def _compile_thread(self):
        # Save current code
        temp_file = self.save_current_code()
        if not temp_file:
            return
        
        # Update UI
        self.root.after(0, lambda: self.progress.start())
        self.root.after(0, lambda: self.compile_btn.config(state="disabled"))
        self.root.after(0, lambda: self.output_text.delete("1.0", tk.END))
        self.root.after(0, lambda: self.output_text.insert(tk.END, "🚀 Starting compilation...\n"))
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
                # Success
                self.root.after(0, lambda: self.output_text.insert(tk.END, "✅ Compilation successful!\n\n"))
                self.root.after(0, lambda: self.output_text.insert(tk.END, output))
                self.root.after(0, lambda: self.status_bar.config(text="✅ Compilation successful!"))
                
                # Check if Arduino file was generated
                arduino_file = "arduino_kids_program.ino"
                if os.path.exists(arduino_file):
                    self.root.after(0, lambda: self.output_text.insert(tk.END, f"\n📁 Arduino sketch saved as: {arduino_file}\n"))
                    self.root.after(0, lambda: self.output_text.insert(tk.END, "🎯 Ready to upload to Arduino IDE!\n"))
                    
                    # Show success message
                    self.root.after(0, lambda: messagebox.showinfo("Success!", 
                                                                  f"✅ Arduino code generated successfully!\n\n"
                                                                  f"📁 File: {arduino_file}\n"
                                                                  f"🎯 Ready to upload to Arduino IDE!"))
                    
                    # Open file location
                    self.root.after(0, self.open_output_folder)
                
            else:
                # Error
                self.root.after(0, lambda: self.output_text.insert(tk.END, "❌ Compilation failed!\n\n"))
                self.root.after(0, lambda: self.output_text.insert(tk.END, output))
                self.root.after(0, lambda: self.status_bar.config(text="❌ Compilation failed"))
                self.root.after(0, lambda: messagebox.showerror("Compilation Error", 
                                                               f"Compilation failed!\n\n{output}"))
        
        except subprocess.TimeoutExpired:
            self.root.after(0, lambda: self.progress.stop())
            self.root.after(0, lambda: self.compile_btn.config(state="normal"))
            self.root.after(0, lambda: self.output_text.insert(tk.END, "❌ Compilation timed out!\n"))
            self.root.after(0, lambda: self.status_bar.config(text="❌ Compilation timed out"))
            
        except Exception as e:
            self.root.after(0, lambda: self.progress.stop())
            self.root.after(0, lambda: self.compile_btn.config(state="normal"))
            self.root.after(0, lambda: self.output_text.insert(tk.END, f"❌ Error: {str(e)}\n"))
            self.root.after(0, lambda: self.status_bar.config(text="❌ Compilation error"))
        
        finally:
            # Clean up temp file
            if os.path.exists(temp_file):
                os.remove(temp_file)
    
    def open_output_folder(self):
        # Open the folder containing the generated files
        try:
            if os.name == 'nt':  # Windows
                os.startfile('.')
            elif os.name == 'posix':  # macOS and Linux
                subprocess.Popen(['xdg-open', '.'])
        except:
            pass

def main():
    root = tk.Tk()
    app = ArduinoKidsCompilerGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()