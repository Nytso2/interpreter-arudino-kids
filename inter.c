#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ============================================================================
// ARDUINO KIDS PROGRAMMING LANGUAGE INTERPRETER
// Converts simple kid-friendly syntax to Arduino C++ code
// Resume Project - Professional Educational Compiler
// ============================================================================

// Token types for Arduino commands
typedef enum {
    // LED Commands
    TOKEN_TURN_ON, TOKEN_TURN_OFF, TOKEN_BLINK, TOKEN_FADE,
    
    // Pin Commands  
    TOKEN_SET_PIN, TOKEN_READ_PIN, TOKEN_ANALOG_READ,
    
    // Servo Commands
    TOKEN_MOVE_SERVO, TOKEN_ATTACH_SERVO,
    
    // Sensor Commands
    TOKEN_READ_TEMP, TOKEN_READ_DISTANCE, TOKEN_READ_LIGHT,
    
    // Sound Commands
    TOKEN_BEEP, TOKEN_PLAY_TONE, TOKEN_PLAY_MELODY,
    
    // Motor Commands
    TOKEN_MOTOR_FORWARD, TOKEN_MOTOR_BACKWARD, TOKEN_MOTOR_STOP,
    
    // Display Commands
    TOKEN_PRINT_LCD, TOKEN_CLEAR_LCD, TOKEN_PRINT_SERIAL,
    
    // Control Flow
    TOKEN_WAIT, TOKEN_REPEAT, TOKEN_IF, TOKEN_WHILE, TOKEN_FOREVER,
    
    // Comparison
    TOKEN_GREATER, TOKEN_LESS, TOKEN_EQUALS, TOKEN_NOT_EQUALS,
    
    // Values and Identifiers
    TOKEN_NUMBER, TOKEN_STRING, TOKEN_HIGH, TOKEN_LOW, TOKEN_ON, TOKEN_OFF,
    
    // Pin References
    TOKEN_PIN, TOKEN_LED_PIN, TOKEN_ANALOG_PIN,
    
    // Delimiters
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_COMMA, TOKEN_SEMICOLON,
    
    // Special
    TOKEN_NEWLINE, TOKEN_EOF, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int number;
    int line;
    int column;
} Token;

typedef struct {
    char* input;
    int pos;
    int length;
    int line;
    int column;
    int error_count;
    char errors[20][512];
} Lexer;

typedef struct {
    char output[16384];
    char setup_code[4096];
    char loop_code[8192];
    char includes[1024];
    char globals[2048];
    int indent_level;
    int has_servo;
    int has_lcd;
    int has_temperature;
    int has_ultrasonic;
    int used_pins[20];
    int pin_count;
} ArduinoGen;

// Error handling
void add_error(Lexer* lexer, const char* message) {
    if (lexer->error_count < 20) {
        snprintf(lexer->errors[lexer->error_count], 512, 
                "Line %d, Col %d: %s", lexer->line, lexer->column, message);
        lexer->error_count++;
    }
}

// Initialize lexer
Lexer* create_lexer(char* input) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->pos = 0;
    lexer->length = strlen(input);
    lexer->line = 1;
    lexer->column = 1;
    lexer->error_count = 0;
    return lexer;
}

// Skip whitespace and comments
void skip_whitespace_and_comments(Lexer* lexer) {
    while (lexer->pos < lexer->length) {
        char current = lexer->input[lexer->pos];
        
        if (isspace(current)) {
            if (current == '\n') {
                lexer->line++;
                lexer->column = 1;
            } else {
                lexer->column++;
            }
            lexer->pos++;
        }
        else if (current == '/' && lexer->pos + 1 < lexer->length && lexer->input[lexer->pos + 1] == '/') {
            while (lexer->pos < lexer->length && lexer->input[lexer->pos] != '\n') {
                lexer->pos++;
            }
        }
        else {
            break;
        }
    }
}

// Keyword recognition for kid-friendly Arduino commands
TokenType get_keyword_type(const char* word) {
    static struct { const char* word; TokenType type; } keywords[] = {
        // LED Commands (super simple for kids)
        {"turn_on", TOKEN_TURN_ON}, {"light_up", TOKEN_TURN_ON}, {"on", TOKEN_TURN_ON},
        {"turn_off", TOKEN_TURN_OFF}, {"light_off", TOKEN_TURN_OFF}, {"off", TOKEN_TURN_OFF},
        {"blink", TOKEN_BLINK}, {"flash", TOKEN_BLINK},
        {"fade", TOKEN_FADE}, {"dim", TOKEN_FADE},
        
        // Pin Commands
        {"set_pin", TOKEN_SET_PIN}, {"pin", TOKEN_SET_PIN},
        {"read_pin", TOKEN_READ_PIN}, {"check_pin", TOKEN_READ_PIN},
        {"analog_read", TOKEN_ANALOG_READ}, {"read_sensor", TOKEN_ANALOG_READ},
        
        // Servo Commands
        {"move_servo", TOKEN_MOVE_SERVO}, {"turn_servo", TOKEN_MOVE_SERVO},
        {"attach_servo", TOKEN_ATTACH_SERVO}, {"connect_servo", TOKEN_ATTACH_SERVO},
        
        // Sensor Commands
        {"read_temperature", TOKEN_READ_TEMP}, {"temp", TOKEN_READ_TEMP}, {"temperature", TOKEN_READ_TEMP},
        {"read_distance", TOKEN_READ_DISTANCE}, {"distance", TOKEN_READ_DISTANCE},
        {"read_light", TOKEN_READ_LIGHT}, {"light", TOKEN_READ_LIGHT}, {"brightness", TOKEN_READ_LIGHT},
        
        // Sound Commands
        {"beep", TOKEN_BEEP}, {"buzz", TOKEN_BEEP},
        {"play_tone", TOKEN_PLAY_TONE}, {"tone", TOKEN_PLAY_TONE},
        {"play_melody", TOKEN_PLAY_MELODY}, {"melody", TOKEN_PLAY_MELODY},
        
        // Motor Commands
        {"motor_forward", TOKEN_MOTOR_FORWARD}, {"forward", TOKEN_MOTOR_FORWARD},
        {"motor_backward", TOKEN_MOTOR_BACKWARD}, {"backward", TOKEN_MOTOR_BACKWARD},
        {"motor_stop", TOKEN_MOTOR_STOP}, {"stop", TOKEN_MOTOR_STOP},
        
        // Display Commands
        {"print_lcd", TOKEN_PRINT_LCD}, {"lcd", TOKEN_PRINT_LCD}, {"display", TOKEN_PRINT_LCD},
        {"clear_lcd", TOKEN_CLEAR_LCD}, {"clear_display", TOKEN_CLEAR_LCD},
        {"print", TOKEN_PRINT_SERIAL}, {"say", TOKEN_PRINT_SERIAL},
        
        // Control Flow
        {"wait", TOKEN_WAIT}, {"delay", TOKEN_WAIT}, {"pause", TOKEN_WAIT},
        {"repeat", TOKEN_REPEAT}, {"loop", TOKEN_REPEAT},
        {"if", TOKEN_IF}, {"when", TOKEN_IF},
        {"while", TOKEN_WHILE},
        {"forever", TOKEN_FOREVER}, {"always", TOKEN_FOREVER},
        
        // Values
        {"high", TOKEN_HIGH}, {"low", TOKEN_LOW},
        {"on", TOKEN_ON}, {"off", TOKEN_OFF},
        
        // Pin types
        {"led_pin", TOKEN_LED_PIN}, {"analog_pin", TOKEN_ANALOG_PIN},
        
        {NULL, TOKEN_EOF}
    };
    
    for (int i = 0; keywords[i].word != NULL; i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_PIN; // Default to pin reference
}

// Enhanced tokenizer
Token get_next_token(Lexer* lexer) {
    Token token;
    memset(&token, 0, sizeof(Token));
    
    skip_whitespace_and_comments(lexer);
    
    if (lexer->pos >= lexer->length) {
        token.type = TOKEN_EOF;
        return token;
    }
    
    char current = lexer->input[lexer->pos];
    token.line = lexer->line;
    token.column = lexer->column;
    
    // Numbers
    if (isdigit(current)) {
        int start = lexer->pos;
        while (lexer->pos < lexer->length && isdigit(lexer->input[lexer->pos])) {
            lexer->pos++;
            lexer->column++;
        }
        
        int len = lexer->pos - start;
        strncpy(token.value, lexer->input + start, len);
        token.value[len] = '\0';
        token.number = atoi(token.value);
        token.type = TOKEN_NUMBER;
        return token;
    }
    
    // Strings
    if (current == '"') {
        lexer->pos++; // skip opening quote
        lexer->column++;
        int start = lexer->pos;
        
        while (lexer->pos < lexer->length && lexer->input[lexer->pos] != '"') {
            lexer->pos++;
            lexer->column++;
        }
        
        if (lexer->pos >= lexer->length) {
            add_error(lexer, "Unterminated string");
            token.type = TOKEN_ERROR;
            return token;
        }
        
        int len = lexer->pos - start;
        strncpy(token.value, lexer->input + start, len);
        token.value[len] = '\0';
        token.type = TOKEN_STRING;
        
        lexer->pos++; // skip closing quote
        lexer->column++;
        return token;
    }
    
    // Two-character operators
    if (lexer->pos + 1 < lexer->length) {
        char next = lexer->input[lexer->pos + 1];
        if (current == '=' && next == '=') {
            token.type = TOKEN_EQUALS;
            strcpy(token.value, "==");
            lexer->pos += 2;
            lexer->column += 2;
            return token;
        }
        if (current == '!' && next == '=') {
            token.type = TOKEN_NOT_EQUALS;
            strcpy(token.value, "!=");
            lexer->pos += 2;
            lexer->column += 2;
            return token;
        }
    }
    
    // Single character tokens
    switch (current) {
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case ',': token.type = TOKEN_COMMA; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case '<': token.type = TOKEN_LESS; break;
        case '>': token.type = TOKEN_GREATER; break;
        case '\n': token.type = TOKEN_NEWLINE; break;
        default:
            if (isalpha(current) || current == '_') {
                // Parse identifier/keyword
                int start = lexer->pos;
                while (lexer->pos < lexer->length && 
                       (isalnum(lexer->input[lexer->pos]) || lexer->input[lexer->pos] == '_')) {
                    lexer->pos++;
                    lexer->column++;
                }
                
                int len = lexer->pos - start;
                strncpy(token.value, lexer->input + start, len);
                token.value[len] = '\0';
                
                // Convert to lowercase for keyword matching
                char lower_value[256];
                strcpy(lower_value, token.value);
                for (int i = 0; lower_value[i]; i++) {
                    lower_value[i] = tolower(lower_value[i]);
                }
                
                token.type = get_keyword_type(lower_value);
                if (token.type == TOKEN_PIN && isdigit(token.value[0])) {
                    token.number = atoi(token.value);
                    token.type = TOKEN_NUMBER;
                }
                return token;
            } else {
                add_error(lexer, "Unknown character");
                token.type = TOKEN_ERROR;
            }
            break;
    }
    
    token.value[0] = current;
    token.value[1] = '\0';
    lexer->pos++;
    lexer->column++;
    
    return token;
}

// Arduino code generator
ArduinoGen* create_arduino_gen() {
    ArduinoGen* gen = malloc(sizeof(ArduinoGen));
    memset(gen, 0, sizeof(ArduinoGen));
    
    strcpy(gen->includes, "// Generated by Arduino Kids Programming Language\n");
    strcpy(gen->setup_code, "void setup() {\n  Serial.begin(9600);\n");
    strcpy(gen->loop_code, "\nvoid loop() {\n");
    gen->indent_level = 1;
    
    return gen;
}

void add_indent_arduino(ArduinoGen* gen, char* target) {
    for (int i = 0; i < gen->indent_level; i++) {
        strcat(target, "  ");
    }
}

void add_line_arduino(ArduinoGen* gen, char* target, const char* line) {
    add_indent_arduino(gen, target);
    strcat(target, line);
    strcat(target, "\n");
}

void add_pin_usage(ArduinoGen* gen, int pin) {
    for (int i = 0; i < gen->pin_count; i++) {
        if (gen->used_pins[i] == pin) return; // Already added
    }
    if (gen->pin_count < 20) {
        gen->used_pins[gen->pin_count++] = pin;
    }
}

// Forward declarations
void parse_statement(Lexer* lexer, ArduinoGen* gen);
void parse_block(Lexer* lexer, ArduinoGen* gen);

void parse_block(Lexer* lexer, ArduinoGen* gen) {
    Token token;
    while ((token = get_next_token(lexer)).type != TOKEN_RBRACE && token.type != TOKEN_EOF) {
        // Put token back and parse statement
        lexer->pos -= strlen(token.value);
        lexer->column -= strlen(token.value);
        parse_statement(lexer, gen);
    }
}

void parse_statement(Lexer* lexer, ArduinoGen* gen) {
    Token token = get_next_token(lexer);
    
    switch (token.type) {
        case TOKEN_TURN_ON: {
            Token pin = get_next_token(lexer);
            char code[256];
            
            add_pin_usage(gen, pin.number);
            snprintf(code, sizeof(code), "pinMode(%d, OUTPUT);", pin.number);
            add_line_arduino(gen, gen->setup_code, code);
            
            snprintf(code, sizeof(code), "digitalWrite(%d, HIGH);  // Turn on pin %d", pin.number, pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            
            snprintf(code, sizeof(code), "Serial.println(\" Pin %d turned ON\");", pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_TURN_OFF: {
            Token pin = get_next_token(lexer);
            char code[256];
            
            add_pin_usage(gen, pin.number);
            snprintf(code, sizeof(code), "pinMode(%d, OUTPUT);", pin.number);
            add_line_arduino(gen, gen->setup_code, code);
            
            snprintf(code, sizeof(code), "digitalWrite(%d, LOW);  // Turn off pin %d", pin.number, pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            
            snprintf(code, sizeof(code), "Serial.println(\"💡 Pin %d turned OFF\");", pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_BLINK: {
            Token pin = get_next_token(lexer);
            Token times = get_next_token(lexer);
            char code[512];
            
            add_pin_usage(gen, pin.number);
            snprintf(code, sizeof(code), "pinMode(%d, OUTPUT);", pin.number);
            add_line_arduino(gen, gen->setup_code, code);
            
            snprintf(code, sizeof(code), "// Blink pin %d for %d times", pin.number, times.number);
            add_line_arduino(gen, gen->loop_code, code);
            snprintf(code, sizeof(code), "for(int i = 0; i < %d; i++) {", times.number);
            add_line_arduino(gen, gen->loop_code, code);
            
            gen->indent_level++;
            snprintf(code, sizeof(code), "digitalWrite(%d, HIGH);", pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            add_line_arduino(gen, gen->loop_code, "delay(500);");
            snprintf(code, sizeof(code), "digitalWrite(%d, LOW);", pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            add_line_arduino(gen, gen->loop_code, "delay(500);");
            gen->indent_level--;
            
            add_line_arduino(gen, gen->loop_code, "}");
            snprintf(code, sizeof(code), "Serial.println(\" Pin %d blinked %d times\");", pin.number, times.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_BEEP: {
            Token pin = get_next_token(lexer);
            Token duration = get_next_token(lexer);
            char code[256];
            
            add_pin_usage(gen, pin.number);
            snprintf(code, sizeof(code), "pinMode(%d, OUTPUT);", pin.number);
            add_line_arduino(gen, gen->setup_code, code);
            
            snprintf(code, sizeof(code), "tone(%d, 1000, %d);  // Beep on pin %d", pin.number, duration.number, pin.number);
            add_line_arduino(gen, gen->loop_code, code);
            snprintf(code, sizeof(code), "delay(%d);", duration.number);
            add_line_arduino(gen, gen->loop_code, code);
            
            snprintf(code, sizeof(code), "Serial.println(\"🔊 Beep on pin %d for %dms\");", pin.number, duration.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_READ_TEMP: {
            Token pin = get_next_token(lexer);
            char code[512];
            
            if (!gen->has_temperature) {
                strcat(gen->includes, "#include <DHT.h>\n");
                snprintf(code, sizeof(code), "#define DHT_PIN %d\n", pin.number);
                strcat(gen->includes, code);
                strcat(gen->includes, "#define DHT_TYPE DHT22\n");
                strcat(gen->includes, "DHT dht(DHT_PIN, DHT_TYPE);\n\n");
                
                add_line_arduino(gen, gen->setup_code, "dht.begin();");
                gen->has_temperature = 1;
            }
            
            add_line_arduino(gen, gen->loop_code, "float temperature = dht.readTemperature();");
            add_line_arduino(gen, gen->loop_code, "if (!isnan(temperature)) {");
            gen->indent_level++;
            add_line_arduino(gen, gen->loop_code, "Serial.print(\"🌡️  Temperature: \");");
            add_line_arduino(gen, gen->loop_code, "Serial.print(temperature);");
            add_line_arduino(gen, gen->loop_code, "Serial.println(\"°C\");");
            gen->indent_level--;
            add_line_arduino(gen, gen->loop_code, "} else {");
            gen->indent_level++;
            add_line_arduino(gen, gen->loop_code, "Serial.println(\"❌ Temperature sensor error\");");
            gen->indent_level--;
            add_line_arduino(gen, gen->loop_code, "}");
            break;
        }
        
        case TOKEN_READ_DISTANCE: {
            Token trig_pin = get_next_token(lexer);
            Token echo_pin = get_next_token(lexer);
            char code[512];
            
            if (!gen->has_ultrasonic) {
                snprintf(code, sizeof(code), "#define TRIG_PIN %d\n", trig_pin.number);
                strcat(gen->includes, code);
                snprintf(code, sizeof(code), "#define ECHO_PIN %d\n\n", echo_pin.number);
                strcat(gen->includes, code);
                
                snprintf(code, sizeof(code), "pinMode(TRIG_PIN, OUTPUT);");
                add_line_arduino(gen, gen->setup_code, code);
                snprintf(code, sizeof(code), "pinMode(ECHO_PIN, INPUT);");
                add_line_arduino(gen, gen->setup_code, code);
                gen->has_ultrasonic = 1;
            }
            
            add_line_arduino(gen, gen->loop_code, "// Read ultrasonic distance");
            add_line_arduino(gen, gen->loop_code, "digitalWrite(TRIG_PIN, LOW);");
            add_line_arduino(gen, gen->loop_code, "delayMicroseconds(2);");
            add_line_arduino(gen, gen->loop_code, "digitalWrite(TRIG_PIN, HIGH);");
            add_line_arduino(gen, gen->loop_code, "delayMicroseconds(10);");
            add_line_arduino(gen, gen->loop_code, "digitalWrite(TRIG_PIN, LOW);");
            add_line_arduino(gen, gen->loop_code, "long duration = pulseIn(ECHO_PIN, HIGH);");
            add_line_arduino(gen, gen->loop_code, "float distance = duration * 0.034 / 2;");
            add_line_arduino(gen, gen->loop_code, "Serial.print(\"📏 Distance: \");");
            add_line_arduino(gen, gen->loop_code, "Serial.print(distance);");
            add_line_arduino(gen, gen->loop_code, "Serial.println(\" cm\");");
            break;
        }
        
        case TOKEN_MOVE_SERVO: {
            Token pin = get_next_token(lexer);
            Token angle = get_next_token(lexer);
            char code[256];
            
            if (!gen->has_servo) {
                strcat(gen->includes, "#include <Servo.h>\n");
                strcat(gen->globals, "Servo myServo;\n\n");
                gen->has_servo = 1;
            }
            
            snprintf(code, sizeof(code), "myServo.attach(%d);", pin.number);
            add_line_arduino(gen, gen->setup_code, code);
            
            snprintf(code, sizeof(code), "myServo.write(%d);  // Move servo to %d degrees", angle.number, angle.number);
            add_line_arduino(gen, gen->loop_code, code);
            snprintf(code, sizeof(code), "Serial.println(\"🔄 Servo moved to %d degrees\");", angle.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_PRINT_LCD: {
            Token message = get_next_token(lexer);
            char code[512];
            
            if (!gen->has_lcd) {
                strcat(gen->includes, "#include <LiquidCrystal.h>\n");
                strcat(gen->globals, "LiquidCrystal lcd(12, 11, 5, 4, 3, 2);\n\n");
                add_line_arduino(gen, gen->setup_code, "lcd.begin(16, 2);");
                gen->has_lcd = 1;
            }
            
            add_line_arduino(gen, gen->loop_code, "lcd.clear();");
            snprintf(code, sizeof(code), "lcd.print(\"%s\");", message.value);
            add_line_arduino(gen, gen->loop_code, code);
            snprintf(code, sizeof(code), "Serial.println(\"📺 LCD: %s\");", message.value);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_PRINT_SERIAL: {
            Token message = get_next_token(lexer);
            char code[256];
            
            snprintf(code, sizeof(code), "Serial.println(\"%s\");", message.value);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_WAIT: {
            Token time = get_next_token(lexer);
            char code[256];
            
            snprintf(code, sizeof(code), "delay(%d);  // Wait %d milliseconds", time.number, time.number);
            add_line_arduino(gen, gen->loop_code, code);
            break;
        }
        
        case TOKEN_REPEAT: {
            Token times = get_next_token(lexer);
            Token lbrace = get_next_token(lexer);
            
            if (lbrace.type != TOKEN_LBRACE) {
                add_error(lexer, "Expected '{' after repeat count");
                return;
            }
            
            char code[256];
            snprintf(code, sizeof(code), "for(int i = 0; i < %d; i++) {", times.number);
            add_line_arduino(gen, gen->loop_code, code);
            
            gen->indent_level++;
            parse_block(lexer, gen);
            gen->indent_level--;
            
            add_line_arduino(gen, gen->loop_code, "}");
            break;
        }
        
        case TOKEN_FOREVER: {
            Token lbrace = get_next_token(lexer);
            
            if (lbrace.type != TOKEN_LBRACE) {
                add_error(lexer, "Expected '{' after forever");
                return;
            }
            
            add_line_arduino(gen, gen->loop_code, "while(true) {");
            
            gen->indent_level++;
            parse_block(lexer, gen);
            gen->indent_level--;
            
            add_line_arduino(gen, gen->loop_code, "}");
            break;
        }
        
        case TOKEN_NEWLINE:
        case TOKEN_EOF:
            break;
            
        default:
            break;
    }
}

void finalize_arduino_code(ArduinoGen* gen) {
    strcat(gen->setup_code, "  Serial.println(\" Arduino Kids Program Starting!\");\n");
    strcat(gen->setup_code, "}\n");
    strcat(gen->loop_code, "  \n  delay(100);  // Small delay for stability\n}\n");
}

void interpret_arduino_kids(const char* code, int show_details) {
    if (show_details) {
        printf("🔧 Arduino Kids Programming Language Interpreter\n");
        printf("===============================================\n");
        printf("Input Program:\n%s\n", code);
        printf("Generating Arduino C++ code...\n\n");
    } else {
        printf("Arduino Kids Compiler\n");
        printf("========================\n");
        printf("Converting your commands to Arduino code...\n\n");
    }
    
    Lexer* lexer = create_lexer((char*)code);
    ArduinoGen* gen = create_arduino_gen();
    
    Token token;
    do {
        token = get_next_token(lexer);
        if (token.type == TOKEN_EOF) break;
        
        lexer->pos -= strlen(token.value);
        lexer->column -= strlen(token.value);
        parse_statement(lexer, gen);
    } while (token.type != TOKEN_EOF);
    
    if (lexer->error_count > 0) {
        printf("⚠Parsing Errors Found:\n");
        for (int i = 0; i < lexer->error_count; i++) {
            printf("   %s\n", lexer->errors[i]);
        }
        printf("\n");
    }
    
    finalize_arduino_code(gen);
    
    // Create complete Arduino sketch
    strcpy(gen->output, gen->includes);
    strcat(gen->output, gen->globals);
    strcat(gen->output, gen->setup_code);
    strcat(gen->output, gen->loop_code);
    
    if (show_details) {
        printf("Generated Arduino Code:\n");
        printf("=========================\n");
        printf("%s", gen->output);
        printf("=========================\n\n");
    }
    
    // Write Arduino sketch file
    FILE* file = fopen("arduino_kids_program.ino", "w");
    if (file) {
        fprintf(file, "%s", gen->output);
        fclose(file);
        
        if (show_details) {
            printf("Arduino sketch saved as 'arduino_kids_program.ino'\n");
            printf("Upload this file to your Arduino using the Arduino IDE!\n\n");
            
            printf("Pin Usage Summary:\n");
            printf("---------------------\n");
            for (int i = 0; i < gen->pin_count; i++) {
                printf("   Pin %d: Used in program\n", gen->used_pins[i]);
            }
            printf("\n");
            
            printf("Required Libraries:\n");
            printf("----------------------\n");
            if (gen->has_servo) printf("   - Servo library (built-in)\n");
            if (gen->has_lcd) printf("   - LiquidCrystal library (built-in)\n");
            if (gen->has_temperature) printf("   - DHT sensor library (install from Library Manager)\n");
            if (!gen->has_servo && !gen->has_lcd && !gen->has_temperature) {
                printf("   - No additional libraries needed!\n");
            }
        } else {
            printf(" Arduino code generated successfully!\n");
            printf(" Saved as: arduino_kids_program.ino\n");
            printf(" Ready to upload to your Arduino!\n");
        }
    } else {
        printf(" Error: Could not create Arduino sketch file\n");
    }
    
    free(lexer);
    free(gen);
}

// Example programs showcase
void run_arduino_examples() {
    printf(" Arduino Kids Programming Language\n");
    printf("====================================\n");
    printf(" Professional Educational Compiler for Resume\n");
    printf("Converts kid-friendly commands to Arduino C++ code\n\n");
    
    printf(" Example 1: Blinking LED\n");
    printf("--------------------------\n");
    interpret_arduino_kids(
        "// Simple LED blink\n"
        "turn_on 13\n"
        "wait 1000\n"
        "turn_off 13\n"
        "wait 1000\n"
        "blink 13 5\n"
        "print \"LED demo complete!\"", 1
    );
    
    printf("\n\nExample 2: Servo Motor Control\n");
    printf("---------------------------------\n");
    interpret_arduino_kids(
        "// Servo sweep\n"
        "print \"Moving servo motor\"\n"
        "move_servo 9 0\n"
        "wait 1000\n"
        "move_servo 9 90\n"
        "wait 1000\n"
        "move_servo 9 180\n"
        "wait 1000\n"
        "print \"Servo sweep complete!\"", 1
    );
    
    printf("\n\nExample 3: Temperature Sensor\n");
    printf("--------------------------------\n");
    interpret_arduino_kids(
        "// Temperature monitoring\n"
        "print \"Reading temperature...\"\n"
        "read_temperature 2\n"
        "wait 2000\n"
        "beep 8 500\n"
        "print \"Temperature check done!\"", 1
    );
    
    printf("\n\nExample 4: Distance Sensor\n");
    printf("-----------------------------\n");
    interpret_arduino_kids(
        "// Ultrasonic distance sensor\n"
        "print \"Measuring distance...\"\n"
        "read_distance 7 6\n"
        "wait 1000\n"
        "beep 8 200\n"
        "print \"Distance measured!\"", 1
    );
    
    printf("\n\nExample 5: Complex Robot Behavior\n");
    printf("------------------------------------\n");
    interpret_arduino_kids(
        "// Smart robot behavior\n"
        "print \"Smart robot starting!\"\n"
        "print_lcd \"Robot Active\"\n"
        "repeat 3 {\n"
        "    turn_on 13\n"
        "    beep 8 300\n"
        "    read_distance 7 6\n"
        "    move_servo 9 45\n"
        "    wait 1000\n"
        "    turn_off 13\n"
        "    wait 500\n"
        "}\n"
        "print_lcd \"Mission Complete\"\n"
        "print \"Robot program finished!\"", 1
    );
}

// Simple mode for kids (hide technical details)
void run_kid_friendly_examples() {
    printf("🎮 Arduino Robot Programming for Kids!\n");
    printf("======================================\n");
    printf("Let's program your Arduino robot with simple commands!\n\n");
    
    printf("Example: Making an LED Blink\n");
    printf("-------------------------------\n");
    interpret_arduino_kids(
        "turn_on 13\n"
        "wait 1000\n"
        "blink 13 3\n"
        "print \"My LED is working!\"", 0
    );
    
    printf("\n Example: Making Sounds\n");
    printf("-------------------------\n");
    interpret_arduino_kids(
        "print \"Making robot sounds!\"\n"
        "beep 8 500\n"
        "wait 500\n"
        "beep 8 300\n"
        "print \"Beep beep!\"", 0
    );
    
    printf("\n Example: Moving a Servo\n");
    printf("--------------------------\n");
    interpret_arduino_kids(
        "print \"Robot arm moving!\"\n"
        "move_servo 9 0\n"
        "wait 1000\n"
        "move_servo 9 180\n"
        "print \"Robot arm moved!\"", 0
    );
}

// Main function with multiple modes
int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf(" Arduino Kids Programming Language Interpreter\n");
            printf("================================================\n\n");
            printf(" For Kids Mode:\n");
            printf("   %s <filename>         - Compile kid-friendly Arduino program\n", argv[0]);
            printf("   %s --kids             - Run kid-friendly examples\n", argv[0]);
            printf("\n🔧 For Developers/Resume Mode:\n");
            printf("   %s --dev <filename>   - Show full Arduino C++ code generation\n", argv[0]);
            printf("   %s --showcase         - Full technical demonstration\n", argv[0]);
            printf("   %s --examples         - All example programs with details\n", argv[0]);
            printf("\n Kid-Friendly Arduino Commands:\n");
            printf("   LED Control: turn_on <pin>, turn_off <pin>, blink <pin> <times>\n");
            printf("   Sound: beep <pin> <duration>, play_tone <pin> <frequency>\n");
            printf("   Servo: move_servo <pin> <angle>\n");
            printf("   Sensors: read_temperature <pin>, read_distance <trig> <echo>\n");
            printf("   Display: print_lcd \"message\", print \"message\"\n");
            printf("   Control: wait <ms>, repeat <times> { ... }, forever { ... }\n");
            printf("\n Example Arduino Kids Program:\n");
            printf("   turn_on 13\n");
            printf("   wait 1000\n");
            printf("   blink 13 5\n");
            printf("   beep 8 500\n");
            printf("   print \"Hello Arduino!\"\n\n");
            return 0;
        }
        
        if (strcmp(argv[1], "--kids") == 0) {
            run_kid_friendly_examples();
            return 0;
        }
        
        if (strcmp(argv[1], "--showcase") == 0 || strcmp(argv[1], "--examples") == 0) {
            run_arduino_examples();
            return 0;
        }
        
        if (strcmp(argv[1], "--dev") == 0 && argc > 2) {
            // Developer mode - show full Arduino C++ generation
            FILE* file = fopen(argv[2], "r");
            if (file) {
                char code[4096];
                size_t bytes_read = fread(code, 1, sizeof(code) - 1, file);
                code[bytes_read] = '\0';
                fclose(file);
                interpret_arduino_kids(code, 1);  // Show technical details
            } else {
                printf(" Error: Could not open file '%s'\n", argv[2]);
                return 1;
            }
            return 0;
        }
        
        // Default: kid-friendly mode for file input
        FILE* file = fopen(argv[1], "r");
        if (file) {
            char code[4096];
            size_t bytes_read = fread(code, 1, sizeof(code) - 1, file);
            code[bytes_read] = '\0';
            fclose(file);
            interpret_arduino_kids(code, 0);  // Hide technical details
        } else {
            printf(" Could not find file '%s'\n", argv[1]);
            printf(" Try: %s --help for usage information\n", argv[0]);
            return 1;
        }
    } else {
        // Default: show kid-friendly examples
        printf(" Welcome to Arduino Kids Programming!\n");
        printf("======================================\n");
        printf(" Easy Arduino programming for kids!\n");
        printf(" Try: %s --help for all options\n\n", argv[0]);
        
        run_kid_friendly_examples();
    }
    
    return 0;
}
