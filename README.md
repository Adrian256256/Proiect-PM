# 🧠 Uscător de mâini inteligent

**Autor:** Harea Teodor-Adrian, 333CA  
**Contact:** teodor_adrian.harea@stud.acs.upb.ro

---

## 📘 Introducere

### ✅ Ce face proiectul?

Acest proiect este un sistem inteligent ce utilizează trei senzori (mișcare, distanță și infraroșu) pentru a detecta prezența și acțiunile utilizatorului. Funcționalitățile principale includ:

- Detectarea mișcării cu senzorul **PIR HC-SR501**
- Măsurarea distanței cu senzorul ultrasonic **HC-SR04P**
- Controlul sistemului prin **telecomandă IR**
- Afișaj pe **LCD 16x2 cu I2C** și în consola serială
- Activarea unui **LED**, a unui **buzzer** și a unui **motor DC** în funcție de intrările senzorilor

### 🎯 Scopul proiectului

Demonstrarea integrării și utilizării eficiente a mai multor senzori într-un sistem coordonat și practic.

### 💡 Ideea de pornire

Inspirația a venit din:

- Observarea sistemelor automate din viața de zi cu zi (ex: uscătoare de mâini)
- Dorința de a combina mai multe funcționalități hardware într-un singur sistem inteligent

### 🎓 Utilitatea proiectului

- Exemplu practic de integrare senzori (PIR, ultrasonic, IR)
- Demonstrarea folosirii **ATmega328P** și **LCD I2C** într-un context real
- Consolidarea cunoștințelor despre embedded programming
- Dobândirea de experiență practică în gestionarea perifericelor

---

## 🛠️ Descriere generală

### 📦 Listă Componente

#### 🧠 Microcontroller

- ATmega328P (16MHz, 32KB Flash)

#### 📡 Senzori

- HC-SR501 – senzor mișcare PIR  
- HC-SR04P – senzor ultrasonic  
- VS1838 – receptor infraroșu

#### 🖥️ Afișaje

- LCD 16x2 cu interfață I2C

#### ⚙️ Dispozitive de acțiune

- Motor DC 3–5V cu elice  
- Buzzer activ 5V  
- LED albastru cu rezistență 220Ω

#### 🔋 Alimentare

- Placă de dezvoltare cu regulator 5V  
- Alimentare prin USB

#### 📚 Biblioteci Software

- `LiquidCrystal_I2C` (v1.1.2)  
- `IRremote` (v3.6.1)

---

## ⚙️ Hardware Design

### 📋 Bill Of Materials (BOM)

> ⚠️ **Resurse oficiale și linkuri pentru cumpărare**

#### 1. Microcontroller ATmega328P

| | |
|---|---|
| **Datasheet** | [Link oficial](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) |
| **Cumpărare** | [Arduino UNO R3 (OptimusDigital)](https://www.optimusdigital.ro/ro/placi-dezvoltare/58-placa-de-dezvoltare-arduino-uno-r3.html) |

#### 2. Senzori

| Componentă | Datasheet | Cumpărare |
|-----------|-----------|-----------|
| LCD 16x2 cu I2C | [Specificații](https://www.handsontec.com/dataspecs/module/I2C_1602_LCD.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/optoelectronice-lcd-uri/62-lcd-1602-cu-interfata-i2c-si-backlight-galben-verde.html) |
| PIR HC-SR501 | [Specificații](https://www.mpja.com/download/31227sc.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/senzori-senzori-pir/106-modul-senzor-pir-hc-sr501.html) |
| HC-SR04P | [Specificații](https://www.electronicoscaldas.com/datasheet/HC-SR04.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/senzori-senzori-de-distanta/8150-senzor-de-distana-ultrasonic-hc-sr04p-3-55-v.html) |
| Receptor IR VS1838 | [Specificații](https://eeshop.unl.edu/pdf/VS1838-Infrared-Receiver-datasheet.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/altele/755-modul-receptor-telecomanda-infrarou.html) |

#### 3. Dispozitive de acțiune

| Componentă | Datasheet | Cumpărare |
|-----------|-----------|-----------|
| Buzzer activ 5V | [Specificații](https://www.electronicoscaldas.com/datasheet/LTE12-Series.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/audio-buzzere/12513-pcb-mounted-active-buzzer-module.html) |
| Motor DC | [Specificații](https://www.makerhero.com/img/files/download/MotorDC-3V-37-0140-Datasheet.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/motoare-altele/3265-motor-in-miniatura.html) |
| LED 3mm | [Specificații](https://www.farnell.com/datasheets/1498852.pdf) | [OptimusDigital](https://www.optimusdigital.ro/ro/optoelectronice-led-uri/699-led-albastru-de-3-mm-cu-lentile-transparente.html) |

---

## 🧑‍💻 Software Design

### 🧩 Biblioteci folosite

- `Arduino.h` – Funcționalități de bază
- `Wire.h` – Comunicare I2C
- `LiquidCrystal_I2C.h` – Afișaj LCD
- `IRremote.h` – Control IR

### 🔌 Funcționalități implementate

- **GPIO** – Control senzori și actuatori
- **UART** – Mesaje debug prin Serial Monitor
- **Timere** – Temporizări non-blocante (`millis()`)
- **I2C** – Comunicare eficientă cu LCD

### 🧠 Despre implementare

> 🗂️ Proiectul este bazat pe o **mașină de stări finite**, în care fiecare stare corespunde unei etape logice (ex: așteptare, detecție, rulare motor etc.).

- `setup()` – inițializarea componentelor
- `loop()` – verificare stări și tranziții
- Fiecare stare are blocuri dedicate și funcții proprii
- Tranziții declanșate de senzori sau comenzi IR
- Feedback vizual și sonor prin LCD, LED și buzzer

### ✅ Validare

- Testare individuală pentru fiecare componentă
- Mesaje de debug în consolă
- Simulare de utilizare reală pentru verificare integrată

### 🚀 Optimizări

- Cod modular și lizibil
- Funcții izolate pentru fiecare stare
- Tranziții clare cu mesaje informative
- Temporizări corecte cu variabile statice locale
- Conversii rapide și precise pentru datele senzorilor

---

## ✅ Concluzii

> Proiectul este un exemplu eficient de automatizare cu senzori și motor, implementat printr-o arhitectură clară și ușor extensibilă. Codul modular și optimizările asigură un răspuns rapid și stabil în utilizare.

---

