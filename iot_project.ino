#include <LiquidCrystal.h>
const int rs = 11, en = 10, d4 = 9, d5 = 8, d6 = 7, d7 = 6;
const int echoPin = 5, trigPin = 2;
const int green_led = 3, red_led = 4, buzzer = 12, motor = 13;
#define vibrator A4
#define smoke_detector A5

const int DELAY = 500;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int breakpoint0  = 4;
const int breakpoint1  = 15;
const int breakpoint2  = 25;
const int breakpoint3  = 40;
const int breakpoint4  = 330;
const int smoke_threshold = 80;

//function to calculate distance in centimeters
double microsecondsToCentimeters(double microseconds) {
    return microseconds/29/2; 
}

void setup() {
    pinMode(A0, INPUT);
    pinMode(smoke_detector, INPUT);
    pinMode(vibrator, OUTPUT);
    pinMode(trigPin, OUTPUT);     //distance sensor
    pinMode(echoPin, INPUT);      //distance sensor
    pinMode(green_led, OUTPUT);           //green led
    pinMode(red_led, OUTPUT);           //red led
    pinMode(buzzer, OUTPUT);          //vibrator and buzzer
    pinMode(motor, OUTPUT);          //dc motor
    pinMode(vibrator, OUTPUT);          //dc motor
    Serial.begin(9600);
}

//function to apply situational changes in the state of output devices 
void execute(bool SAFE_LED, bool WARNING_LED, bool BUZZER, bool BRAKE_SIGNAL) { 
    digitalWrite(green_led, SAFE_LED);     //to change state of green LED 
    digitalWrite(red_led, WARNING_LED);    //to change state of green LED
    digitalWrite(buzzer, BUZZER);          //to change state of buzzer
    digitalWrite(motor, BRAKE_SIGNAL ^ 1); //to change state of dc motor
}

//function to obtain distance from ultrasonic distance sensor
double getDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    double duration = pulseIn(echoPin, HIGH);
    return microsecondsToCentimeters(duration); //calling function to return distance in centimetres
}

void loop() {
        lcd.clear();
        execute(LOW, LOW, LOW, LOW);
        //if the vehicle has started
        int smoke_level = analogRead(smoke_detector); 
        smoke_level = map(smoke_level, 85, 385, 0, 255);
        Serial.println(smoke_level);
        if (smoke_level > smoke_threshold) {  //smoke concentration gets high
            lcd.clear();
            lcd.print("Too much smoke!");  //displays the message when the smoke concentration is high
            delay(DELAY);
            analogWrite(vibrator, 255); //vibrations when the smoke concentration is high (in case of fire or short circuit)
        }
        else {
           analogWrite(vibrator, 0);
        }
        
        
        double neighbour_distance = getDistance();
        //Serial.println(neighbour_distance);
        if(neighbour_distance > breakpoint4) {
            execute(LOW, LOW, LOW, LOW);
            lcd.clear();
        } 
        else if (neighbour_distance > breakpoint0 && neighbour_distance < breakpoint1) {
            lcd.clear();
            lcd.print("Min_Dis ");         //Warning light should be on
            lcd.print(neighbour_distance); //buzzer and steeringwheel vibrator should alert 
            delay(DELAY);                  //driver automatic brakes should be turned on
            execute(LOW, HIGH, HIGH, HIGH);//Turn off Safe distance indicating Led
        }
        else if(neighbour_distance >= breakpoint1 && neighbour_distance < breakpoint2) {
            lcd.clear();
            lcd.print("Min_Dis ");         //Warning light should be on
            lcd.print(neighbour_distance); //buzzer and steeringwheel vibrator should alert
            delay(DELAY);                  //driver automatic brakes should be turned off
            execute(LOW, HIGH, HIGH, LOW); //Turn off Safe distance indicating Led
        }
        else if(neighbour_distance >= breakpoint2 && neighbour_distance < breakpoint3) {
            lcd.clear();
            lcd.print("Min_Dis ");         //Warning light should be on
            lcd.print(neighbour_distance); //Turn off buzzer and steeringwheel vibrator
            delay(DELAY);                  //dont send automatic brake signal 
            execute(LOW, HIGH, LOW, LOW);  //Turn off Safe distance indicating Led 
        }
        else if(neighbour_distance >= breakpoint3 && neighbour_distance < breakpoint4) {
            lcd.clear();
            lcd.print("Min_Dis ");         //Warning light should be off
            lcd.print(neighbour_distance); //Turn off buzzer and steeringwheel vibrator
            delay(DELAY);                  //dont send automatic brake signal
            execute(HIGH, LOW, LOW, LOW);  //Turn on Safe distance indicating Led
        }
        else if(neighbour_distance <= breakpoint0) {
            lcd.clear();
            lcd.print("Accident Alert!"); //displays the message when the vehicles collides
            delay(DELAY); 
            execute(LOW, HIGH, LOW, HIGH);
        }
    }
