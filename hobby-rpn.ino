#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <fp64lib.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET    -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 500000, 100000);

#define MAX_DIGIT     10

byte pin_col[] = {0, 1, 2, 3};   // PD
byte pin_row[] = {3, 2, 1, 0};   // PC

float64_t x, y, z, t;

void push() {
    t = z;
    z = y;
    y = x;
}

void pop() {
    x = y;
    y = z;
    z = t;
}

char key_scan() {
    PORTD |= _BV(pin_col[0]) | _BV(pin_col[1]) | _BV(pin_col[2]) | _BV(pin_col[3]); // set high

    for (byte i = 0; i < 4; i++) {
        PORTD &= ~_BV(pin_col[i]);  // set low

        for (byte j = 0; j < 4; j++) {
            if ((PINC & _BV(pin_row[j])) == LOW) {
                switch (4 * j + i) {
                    case  0: return '7';
                    case  1: return '8';
                    case  2: return '9';
                    case  3: return '/';

                    case  4: return '4';
                    case  5: return '5';
                    case  6: return '6';
                    case  7: return '*';

                    case  8: return '1';
                    case  9: return '2';
                    case 10: return '3';
                    case 11: return '-';

                    case 12: return '0';
                    case 13: return '.';
                    case 14: return '=';
                    case 15: return '+';
                }
            }
        }
        PORTD |= _BV(pin_col[i]);   // set high
    }
    return 0;
}

void blink_display() {
    display.clearDisplay();
    display.display();
    delay(20);
}

void init_display() {
    display.clearDisplay();
    display.setCursor(4, 12);
}

String fp64_to_string_wrap(float64_t n) {
    if (fp64_signbit(n)) {  // minus
        return fp64_to_string(n, MAX_DIGIT, MAX_DIGIT - 3);
    }
    else {
        return fp64_to_string(n, MAX_DIGIT, MAX_DIGIT - 2);
    }
}

void setup() {
    CLKPR = 0x80; 
    CLKPR = 0x1;    // 8MHz

    // col
    DDRD  |= _BV(pin_col[0]) | _BV(pin_col[1]) | _BV(pin_col[2]) | _BV(pin_col[3]);     // OUTPUT
    PORTD |= _BV(pin_col[0]) | _BV(pin_col[1]) | _BV(pin_col[2]) | _BV(pin_col[3]);     // HIGH

    // row
    DDRC  &= ~(_BV(pin_row[0]) | _BV(pin_row[1]) | _BV(pin_row[2]) | _BV(pin_row[3]));  // INPUT
    PORTC |=   _BV(pin_row[0]) | _BV(pin_row[1]) | _BV(pin_row[2]) | _BV(pin_row[3]);   // PULLUP

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    init_display();
    display.print("0.00");
    display.display();

    x = y = z = t = 0;
}

void loop() {
    static String x_disp = "";
    static char prev_loop_key = 0;
    static byte prev_pushed_key_type = 0;    // 0:numeral 1:operator 2:enter

    // key scan
    char key = key_scan();
    if (key != 0 && key != prev_loop_key) {

        // detect long push
        boolean long_push = false;
        unsigned long pushed_time = millis();
        while (key_scan() == key) {
            if (millis() - pushed_time > 1000 / 2) {    // 1sec
                long_push = true;
                break;
            }
        }

        // operator
        if (key == '+' || key == '-' || key == '*' || key == '/') {
            float64_t acc1 = x;
            pop();
            float64_t acc2 = x;

            switch (key) {
                case '+': x = fp64_add(acc2, acc1); break;
                case '-': x = fp64_sub(acc2, acc1); break;
                case '*': x = fp64_mul(acc2, acc1); break;
                case '/': x = fp64_div(acc2, acc1); break;
            }

            x_disp = fp64_to_string_wrap(x);
            prev_pushed_key_type = 1;
            blink_display();
        }
        // enter
        else if (key == '=') {
            // AC
            if (long_push) {
                x = y = z = t = 0;
                x_disp = "";
                prev_pushed_key_type = 0;
            }
            // enter
            else {
                push();
                
                x_disp = fp64_to_string_wrap(x);
                prev_pushed_key_type = 2;
            }
            blink_display();
        }
        // numeral or .
        else {
            if (prev_pushed_key_type == 1) push();          // operator
            if (prev_pushed_key_type > 0) x_disp = "";      // operator or enter
            
            if (x_disp.length() < MAX_DIGIT) {
                if (x_disp.indexOf(".") == -1 || key != '.') {
                    x_disp.concat(key);
                    x = fp64_atof((char*)x_disp.c_str());
                }
            }
            prev_pushed_key_type = 0;
        }

        // display
        init_display();
        if (x_disp == "") {
            display.print("0.00");
        }
        else {
            display.print(x_disp);
        }
        display.display();
    }
    prev_loop_key = key;
    delay(1);
}
