#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

long microsecondsToCentimeters(long microseconds);
void handleResuming();
void handlePaused();
void handleCalibrating();
void handleReady();
void handleListening();
void handleMotionDetected();
void handleBuzzerSinging();
void handleCheckingDistance();
void handleTooFar();
void handleMotorRunning();
void handleContinuousRunning();

// set I2C address and LCD size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// boolean that tells if the system is running or not
boolean isRunning = true;

// pin for motion sensor
const int PIR_PIN = 3;

// pin for led and buzzer
const int LED_PIN = 13;

// pin for motor
const int MOTOR_PIN = 12;

// pins for proximity sensor
const int TRIGGER_PIN = 7;
const int ECHO_PIN = 8;

// setup for IR remote
const int RECEIVER_PIN = 11;
IRrecv irrecv(RECEIVER_PIN);
decode_results results;

// state machine states
enum State {
    CALIBRATING = 0,      // system is calibrating at startup
    READY,                // system is ready to listen
    LISTENING,            // waiting for motion detection
    MOTION_DETECTED,      // motion detected by PIR sensor
    BUZZER_SINGING,       // led and buzzer are activated
    CHECKING_DISTANCE,    // measuring distance with ultrasonic sensor
    TOO_FAR,              // user is too far
    MOTOR_RUNNING,        // motor is running
    PAUSED,               // system is paused from remote
    RESUMING,             // system is resuming after pause
    CONTINUOUS_RUNNING    // motor runs continuously (manual mode)
};
State currentState = CALIBRATING;

// time in milliseconds when a state started
unsigned long stateStartTime = 0;

// time in milliseconds when the led and buzzer started
unsigned long ledBuzzerStartTime = 0;

void setup() {
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // setup for remote control
    irrecv.enableIRIn();

    // setup for LCD
    lcd.init();
    // turn on the LCD screen backlight
    lcd.backlight();
    Serial.begin(9600);
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(PIR_PIN, LOW);

    // initialize state
    lcd.setCursor(1, 0);
    Serial.print("Calibrating...");
    lcd.print("Calibrating...");
    stateStartTime = millis();
    Serial.println("System started. State: CALIBRATING");
}

// remote control pressed time
unsigned long remotePressedTime = 0;

void check_remote() {
    // debounce time for remote control (ms)
    static const int CHECK_REMOTE_DEBOUNCE = 1000;
    // debounce time for remote control
    if (millis() - remotePressedTime < CHECK_REMOTE_DEBOUNCE) {
        return;
    }
    if (irrecv.decode(&results)) {
        Serial.print("Remote control pressed: ");
        Serial.println(results.value, HEX);
        remotePressedTime = millis();

        // FFC23D is for pause
        if (results.value == 0xFFC23D) {
            if (currentState == PAUSED) {
                currentState = RESUMING;
                stateStartTime = millis();
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Resuming...");
                Serial.println("Transition: PAUSED -> RESUMING");
            } else {
                // turn off motor, buzzer, led
                digitalWrite(MOTOR_PIN, LOW);
                digitalWrite(LED_PIN, LOW);

                currentState = PAUSED;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Paused");
                Serial.println("Transition: any state -> PAUSED");
            }
        }

        // FFA857 is for continuous running
        if (results.value == 0xFFA857) {
            currentState = CONTINUOUS_RUNNING;
            // turn on the motor
            digitalWrite(MOTOR_PIN, HIGH);
            // turn off the led and buzzer
            digitalWrite(LED_PIN, LOW);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Running...");
            Serial.println("Transition: any state -> CONTINUOUS_RUNNING");
        }
        // FFE01F is for stop continuous running
        if (results.value == 0xFFE01F) {
            currentState = LISTENING;
            // turn off the motor
            digitalWrite(MOTOR_PIN, LOW);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Listening...");
            Serial.println("Transition: CONTINUOUS_RUNNING -> LISTENING");
        }

        irrecv.resume(); // Receive the next value
    }
}

void loop() {
    check_remote();

    switch (currentState) {
        case RESUMING:
            handleResuming();
            break;
        case PAUSED:
            handlePaused();
            break;
        case CALIBRATING:
            handleCalibrating();
            break;
        case READY:
            handleReady();
            break;
        case LISTENING:
            handleListening();
            break;
        case MOTION_DETECTED:
            handleMotionDetected();
            break;
        case BUZZER_SINGING:
            handleBuzzerSinging();
            break;
        case CHECKING_DISTANCE:
            handleCheckingDistance();
            break;
        case TOO_FAR:
            handleTooFar();
            break;
        case MOTOR_RUNNING:
            handleMotorRunning();
            break;
        case CONTINUOUS_RUNNING:
            handleContinuousRunning();
            break;
    }
}

// handlers for each state

void handleResuming() {
    // time for displaying "Resuming..." (ms)
    static const int RESUMING_TIME = 1000;
    if (millis() - stateStartTime >= RESUMING_TIME) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Listening...");
        Serial.println("Transition: RESUMING -> LISTENING");
        currentState = LISTENING;
        stateStartTime = millis();
    }
}

void handlePaused() {
    // do nothing, just wait for the remote control to be pressed again
    //Serial.println("System is PAUSED");
}

void handleCalibrating() {
    // time for calibration (ms)
    static const int CALIBRATION_TIME = 2000;
    if (millis() - stateStartTime >= CALIBRATION_TIME) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sensor is ready.");
        Serial.println("Transition: CALIBRATING -> READY");
        stateStartTime = millis();
        currentState = READY;
    }
}

void handleReady() {
    // time for "Sensor is ready." message (ms)
    static const int READY_MESSAGE_TIME = 2000;
    if (millis() - stateStartTime >= READY_MESSAGE_TIME) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Listening...");
        Serial.println("Transition: READY -> LISTENING");
        currentState = LISTENING;
        stateStartTime = millis();
    }
}

void handleListening() {
    if (digitalRead(PIR_PIN) == HIGH) {
        Serial.println("Motion detected! Transition: LISTENING -> MOTION_DETECTED");
        currentState = MOTION_DETECTED;
        stateStartTime = millis();
    }
}

void handleMotionDetected() {
    static bool firstEntry = true;
    static unsigned long motionMsgStart = 0;

    if (firstEntry) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Motion detected!");
        Serial.println("Displaying: Motion detected!");
        motionMsgStart = millis();
        firstEntry = false;
    }

    // show the message for 1000 ms
    if (millis() - motionMsgStart >= 1000) {
        firstEntry = true;
        stateStartTime = millis();
        Serial.println("Transition: MOTION_DETECTED -> CHECKING_DISTANCE");
        currentState = CHECKING_DISTANCE;
    }
}

void handleBuzzerSinging() {
    // time for the led and buzzer to be on (ms)
    static const int LED_BUZZER_TIME = 100;
    if (millis() - stateStartTime >= LED_BUZZER_TIME) {
        digitalWrite(LED_PIN, LOW);
        // if you have a buzzer pin, add here: digitalWrite(BUZZER_PIN, LOW);
        // now start the motor
        digitalWrite(MOTOR_PIN, HIGH);
        Serial.println("Transition: BUZZER_SINGING -> MOTOR_RUNNING");
        currentState = MOTOR_RUNNING;
        stateStartTime = millis();
    }
}

void handleCheckingDistance() {
    // maximum distance for the motor to run (cm)
    static const int MAXIMUM_DISTANCE = 10;
    // trigger the ultrasonic sensor
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    // read the echo pin
    long duration = pulseIn(ECHO_PIN, HIGH);
    long distance = microsecondsToCentimeters(duration);

    Serial.print("Distance measured: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance >= MAXIMUM_DISTANCE) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Too far away!");
        lcd.setCursor(0, 1);
        lcd.print(distance);
        lcd.print(" cm");
        Serial.println("Transition: CHECKING_DISTANCE -> TOO_FAR (Too far away!)");
        currentState = TOO_FAR;
        stateStartTime = millis();
    } else {
        // turn on the led and buzzer shortly before starting the motor
        digitalWrite(LED_PIN, HIGH);
        // if you have a buzzer pin, add here: digitalWrite(BUZZER_PIN, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Motor running!");
        Serial.println("Transition: CHECKING_DISTANCE -> BUZZER_SINGING (Distance OK)");
        currentState = BUZZER_SINGING;
        stateStartTime = millis();
    }
}

void handleTooFar() {
    // time for "Too far away!" message (ms)
    static const int TOO_FAR_MESSAGE_TIME = 2000;
    if (millis() - stateStartTime >= TOO_FAR_MESSAGE_TIME) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Listening...");
        Serial.println("Transition: TOO_FAR -> LISTENING");
        currentState = LISTENING;
        stateStartTime = millis();
    }
}

void handleMotorRunning() {
    // time for the motor to run (ms)
    static const int MOTOR_RUNNING_TIME = 5000;
    digitalWrite(MOTOR_PIN, HIGH);
    if (millis() - stateStartTime >= MOTOR_RUNNING_TIME) {
        digitalWrite(MOTOR_PIN, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Listening...");
        Serial.println("Transition: MOTOR_RUNNING -> LISTENING");
        currentState = LISTENING;
        stateStartTime = millis();
    }
}

void handleContinuousRunning() {
    // do nothing, just wait for the remote control to be pressed again
    //Serial.println("System is in CONTINUOUS_RUNNING mode");
}

long microsecondsToCentimeters(long microseconds) {
    // formula for converting microseconds to centimeters
    return microseconds / 29 / 2;
}