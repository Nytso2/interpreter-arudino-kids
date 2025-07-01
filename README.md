h# 🤖 Arduino Kids Programming Language Interpreter

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Arduino-green.svg)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Education](https://img.shields.io/badge/Purpose-STEM_Education-purple.svg)](README.md)

> **A professional educational compiler that translates kid-friendly commands into production Arduino C++ code**

## 🌟 Overview

This project implements a complete programming language interpreter designed to make Arduino programming accessible to children aged 8-16. The system features a full compiler pipeline that converts intuitive English-like commands into optimized, production-ready Arduino sketches.

**🎯 Perfect for**: Schools, makerspaces, coding camps, and parents teaching kids programming with tangible hardware results.

## ⚡ Quick Start

```bash
# 1. Compile the interpreter
gcc -o inter inter.c -lm

# 2. Create your first robot program
echo 'print "Hello Robot World!"
turn_on 13
blink 13 5
beep 8 500' > my_first_robot.txt

# 3. Generate Arduino code
./inter my_first_robot.txt

# 4. Upload the generated arduino_kids_program.ino to your Arduino!
```

## 🚀 Features

### **🎮 Kid-Friendly Programming**
- **Simple English Commands**: `turn_on`, `blink`, `beep`, `move_servo`
- **Hardware Abstraction**: No need to understand registers or timers
- **Instant Feedback**: Commands produce immediate visible results
- **Error Prevention**: Built-in safety checks and helpful error messages

### **🔧 Professional Compiler Features**
- **Complete Lexer**: Tokenization with line/column error reporting
- **Recursive Descent Parser**: Handles complex nested structures
- **Code Generator**: Produces optimized Arduino C++ with proper libraries
- **Memory Management**: Safe string handling and buffer management
- **Library Integration**: Automatic inclusion of required Arduino libraries

### **🎯 Educational Benefits**
- **Bridge Learning**: Transition from visual to text-based programming
- **Immediate Results**: Physical hardware responds to code changes
- **Conceptual Understanding**: Learn programming through robotics
- **Scalable Complexity**: Start simple, add advanced features gradually

## 📖 Usage Guide

### **For Educators/Parents**
```bash
# Kid-friendly mode (clean output)
./inter student_program.txt

# Developer mode (show generated Arduino code)
./inter --dev student_program.txt

# Run all examples
./inter --showcase

# Help and command reference
./inter --help
```

### **For Students/Kids**
Create simple text files with robot commands:

**🔥 Blinking LED Example** (`blink_demo.txt`):
```
print "My LED is starting!"
turn_on 13
wait 1000
blink 13 10
print "Blinking complete!"
```

**🎵 Robot Sounds** (`robot_sounds.txt`):
```
print "Robot making sounds..."
beep 8 500
wait 200
beep 8 300
wait 200
beep 8 700
print "Sound demo done!"
```

**🤖 Smart Robot** (`smart_robot.txt`):
```
print "Smart robot starting!"
repeat 3 {
    read_distance 7 6
    move_servo 9 90
    beep 8 300
    wait 1000
}
print "Mission complete!"
```

## 📋 Command Reference

### **LED Control**
| Command | Description | Example |
|---------|-------------|---------|
| `turn_on <pin>` | Turn on LED/output | `turn_on 13` |
| `turn_off <pin>` | Turn off LED/output | `turn_off 13` |
| `blink <pin> <times>` | Blink LED multiple times | `blink 13 5` |

### **Sound & Music**
| Command | Description | Example |
|---------|-------------|---------|
| `beep <pin> <duration>` | Make beep sound | `beep 8 500` |
| `play_tone <pin> <freq>` | Play specific frequency | `play_tone 8 1000` |

### **Motors & Movement**
| Command | Description | Example |
|---------|-------------|---------|
| `move_servo <pin> <angle>` | Control servo motor | `move_servo 9 90` |
| `motor_forward <pin>` | Run motor forward | `motor_forward 5` |
| `motor_stop <pin>` | Stop motor | `motor_stop 5` |

### **Sensors**
| Command | Description | Example |
|---------|-------------|---------|
| `read_temperature <pin>` | Read temperature sensor | `read_temperature 2` |
| `read_distance <trig> <echo>` | Ultrasonic distance | `read_distance 7 6` |
| `read_light <pin>` | Read light sensor | `read_light A0` |

### **Display & Communication**
| Command | Description | Example |
|---------|-------------|---------|
| `print "message"` | Serial monitor output | `print "Hello!"` |
| `print_lcd "text"` | LCD display output | `print_lcd "Robot Ready"` |

### **Control Flow**
| Command | Description | Example |
|---------|-------------|---------|
| `wait <milliseconds>` | Pause execution | `wait 1000` |
| `repeat <times> { }` | Loop commands | `repeat 5 { blink 13 1 }` |
| `forever { }` | Run continuously | `forever { read_distance 7 6 }` |

## 🎯 Generated Arduino Code

**Input (Kid-Friendly)**:
```
turn_on 13
blink 13 3
beep 8 500
```

**Output (Production Arduino C++)**:
```cpp
// Generated by Arduino Kids Programming Language
void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("🚀 Arduino Kids Program Starting!");
}

void loop() {
  digitalWrite(13, HIGH);  // Turn on pin 13
  Serial.println("💡 Pin 13 turned ON");
  
  // Blink pin 13 for 3 times
  for(int i = 0; i < 3; i++) {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }
  
  tone(8, 1000, 500);  // Beep on pin 8
  delay(500);
  Serial.println("🔊 Beep on pin 8 for 500ms");
  
  delay(100);  // Small delay for stability
}
```

## 🧪 Testing

Run the included test script to verify everything works:

```bash
./test_interpreter.sh
```

This will test all major features and create example Arduino sketches.

## 🏗️ Technical Architecture

### **Compiler Pipeline**
1. **Lexical Analysis**: Converts source text into tokens
2. **Syntax Parsing**: Builds abstract syntax tree
3. **Semantic Analysis**: Validates hardware constraints
4. **Code Generation**: Produces optimized Arduino C++
5. **Library Management**: Auto-includes required dependencies

### **Error Handling**
- **Syntax Errors**: Clear messages with line/column numbers
- **Hardware Validation**: Warns about invalid pin usage
- **Safety Checks**: Prevents harmful operations

## 💼 Skills Demonstrated

This project showcases:
- **Compiler Design** (lexers, parsers, code generators)
- **Domain-Specific Languages** (Arduino-focused syntax)
- **Embedded Systems** (hardware abstraction, optimization)
- **Educational Technology** (age-appropriate interfaces)
- **Software Architecture** (modular, extensible design)
- **Memory Management** (safe C programming practices)

## 🎓 Educational Impact

### **For Students**
- **Immediate Gratification**: Code creates visible, physical results
- **Conceptual Bridge**: Links abstract programming to tangible outcomes
- **Confidence Building**: Success with simple commands builds programming confidence
- **STEM Integration**: Combines programming, electronics, and engineering

### **For Educators**
- **Curriculum Ready**: Fits into existing STEM programs
- **Scalable Difficulty**: Supports beginners through advanced students
- **Assessment Friendly**: Clear success criteria (does the robot work?)
- **Cross-Curricular**: Connects math, science, and technology

## 📊 Hardware Requirements

### **Minimum Setup**
- Arduino Uno/Nano/ESP32
- LED (built-in pin 13 works)
- Breadboard and jumper wires

### **Recommended Setup**
- Arduino Uno R3
- LEDs and resistors
- Piezo buzzer
- Servo motor
- Ultrasonic sensor (HC-SR04)
- Temperature sensor (DHT22)
- 16x2 LCD display
- Breadboard and jumper wires

## 🤝 Contributing

We welcome contributions! Areas for enhancement:
- Additional sensor support
- More complex control structures
- Visual programming interface
- Real-time debugging features
- Bluetooth/WiFi communication

## 📄 License

MIT License - Free for educational and commercial use.

## 🌟 Acknowledgments

Inspired by Logo, Scratch, and the maker education movement. Built for educators who believe programming should be accessible, engaging, and immediately rewarding.

---

**Made with ❤️ for young programmers and educators worldwide**
