# 🤖 Arduino Kids Programming Language Interpreter

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Arduino-green.svg)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Education](https://img.shields.io/badge/Purpose-STEM_Education-purple.svg)](README.md)
[![GUI](https://img.shields.io/badge/GUI-Python_Tkinter-orange.svg)](arduino_gui.py)

> **A complete educational IDE with GUI that translates kid-friendly commands into production Arduino C++ code**

![Arduino Kids Programming GUI](screenshot.png)

## 🌟 Overview

Complete programming language interpreter with professional GUI for kids aged 8-16. Converts simple English commands into optimized Arduino sketches.

**Perfect for**: Schools, makerspaces, coding camps, and STEM education.

## ⚡ Quick Start

### GUI Mode (Recommended)
```bash
gcc -o inter inter.c -lm
python3 arduino_gui.py
# Click "Examples" → Choose program → "COMPILE TO ARDUINO"
```
### Command Line
```bash
gcc -o inter inter.c -lm
echo 'print "Hello!"
turn_on 13
blink 13 5' > test.txt
./inter test.txt

```

# 🚀 Features
## GUI Interface

- Drag & Drop: Easy file loading
- Code Editor: Built-in with syntax highlighting
- Examples: Pre-loaded robot programs
- One-Click Compile: Big compile button
- Real-time Feedback: Progress and error display

# 📖 Usage
## GUI Mode

Run: 
```bash 
python3 arduino_gui.py
```
Load program (Examples/Browse/New)
Edit code in built-in editor
Click "🚀 COMPILE TO ARDUINO"  
Upload .ino file to Arduino IDE
## Command Lne 
```bash
./inter program.txt          # Kid mode
./inter --dev program.txt    # Show Arduino code
./inter --showcase           # Run examples
./inter --help               # Commands
```
📋 Commands
CommandDescriptionExampleturn_on <pin>Turn on LEDturn_on 13turn_off <pin>Turn off LEDturn_off 13blink <pin> <times>Blink LEDblink 13 5beep <pin> <duration>Make soundbeep 8 500move_servo <pin> <angle>Move servomove_servo 9 90print "text"Serial outputprint "Hello!"wait <ms>Delaywait 1000repeat <n> { }Looprepeat 3 { blink 13 1 }

## 📋 Commands

| Command Syntax               | Description       | Example                |
|-----------------------------|-------------------|------------------------|
| `turn_on <pin>`             | Turn on LED       | `turn_on 13`          |
| `turn_off <pin>`            | Turn off LED      | `turn_off 13`         |
| `blink <pin> <times>`       | Blink LED         | `blink 13 5`          |
| `beep <pin> <duration>`     | Make sound        | `beep 8 500`          |
| `move_servo <pin> <angle>`  | Move servo        | `move_servo 9 90`     |
| `print "text"`              | Serial output     | `print "Hello!"`      |
| `wait <ms>`                 | Delay             | `wait 1000`           |
| `repeat <n> { ... }`        | Loop commands     | `repeat 3 { blink 13 1 }` |

## Example 
## Input

```bash
turn_on 13
blink 13 3
beep 8 500
```

### Generated code

```bash
void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  for(int i = 0; i < 3; i++) {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }
  tone(8, 1000, 500);
  delay(100);
}
```
## Architecture 
- GUI: Python Tkinter with kid-friendly design
- Compiler: C with lexer → parser → code generator
- Output: Production-ready Arduino sketches
- Integration: Seamless GUI to compiler pipeline
