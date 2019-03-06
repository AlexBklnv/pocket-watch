/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */
#include <avr/eeprom.h> 
#include <OneButton.h>
#include <DS3231RTC.h>
#include <avr/sleep.h>

#define ALARM 2
#define BUTTON 3
#define BEEPER 1
#define BRIGHT A2
#define BATTERY A3

#define tm_displayOff 0				// Пока выкуриваем или отключаемся нет смысла нам гореть, завершаем дела и спать
#define tm_showTime 1               // дежурный режим + настройка времени (3 секунды)
#define tm_showBattery 2            // отображение уровня батареи (3 секунды)
#define tm_showTemperature 3        // отображение уровня температуры (3 секунды)
#define tm_showDate 4               // отображение даты день месяц (3 секунды)
#define tm_showSec 5                // отображение секунд (до отключения)
#define tm_showBright 6             // отображение уровня яркости для настройки
#define tm_showAlarmState 7         // отображение активности будильника для настройки
#define tm_showAlarmTime 8          // отображение времени будильника для настройки
#define tm_showAlarmDow 9           // отображение дней недели будильника d1-d7 для настройки
#define tm_showHourBeepState 10     // отображение активности часового для настройки
#define tm_showHourBeepTime 11      // отображение старта-конца периода активности часового для настройки
#define tm_showYear 12              // отображение текущего года для настройки

#define mw_clock 0                  // Дежурный режим
#define mw_setHour 1                // Режим установки часов времени
#define mw_setMin 2                 // Режим установки минут времени
#define mw_setBright 3              // Режим установки яркости экрана
#define mw_setAlarmState 4          // Режим установки активности будильника
#define mw_setAlarmHour 5           // Режим установки часов будильника
#define mw_setAlarmMin 6            // Режим установки минут будильника
#define mw_setAlarmDow 7            // Режим установки дней недели работы будильника
#define mw_setHourBeepState 8       // Режим установки активности часового
#define mw_setHourBeepStart 9       // Режим установки старта работы часового включительно
#define mw_setHourBeepStop 10       // Режим установки остановки работы часового включительно
#define mw_setYear 11               // Режим установки текущего года
#define mw_setMonth 12              // Режим установки месяца даты
#define mw_setDay 13                // Режим установки дня даты
#define mw_lastMode mw_setDay		// Последний режим для перевода на дежурный
#define mw_alarm 14					// Специальный режим для будильника


byte EEMEM eeprom_init = 123;
byte EEMEM eeprom_bright = 3;

byte EEMEM eeprom_isHourBeepActive = 1;
byte EEMEM eeprom_hourBeepStart = 7;
byte EEMEM eeprom_hourBeepStop = 22;

byte EEMEM eeprom_isAlarmActive = 0;
byte EEMEM eeprom_alarmHour = 7;
byte EEMEM eeprom_alarmMin = 0;
byte EEMEM eeprom_alarmDow = 1;

// сделать под мелодии
struct Beeper {
	boolean isCanInit;
	boolean canBeep;
	boolean isPinActive;
	byte	count;
	byte	initCount;
	int durationActive;
	int durationInactive;
	int timeOut;
	uint64_t timeStamp;
};

struct Tube {
	byte value[4];
	boolean isFlash[4];
	boolean isDisabled[4];
	boolean isDoteActive[4];
	unsigned long switchTime;
};

struct Alarm {
	byte isActive;
	byte hour;
	byte minute;
	byte curDow;
	byte dowState;	// B00010100 ср и пт активны
	Beeper beeper;
	unsigned long alarmAutoStopDelay;
};

struct HourBeep {
	byte isActive;
	byte start;
	byte stop;
};


struct Button {
	int longPressTimeDetector;
	unsigned long pressStamp;
};

struct EditValue {
	int value;
	byte max;
	byte min;	
};

struct Bright {
	byte mode;            // Уровни яркости. 0 - автомат; 1-3 установка
	int delayDuration;
};


unsigned long syncTimeStamp = 0;
DateTime dateTime;

// Все необходимое для кнопки
Button button;
OneButton buttonCallback(BUTTON, false);

Bright bright;
Tube tube;
Alarm alarmClock;
HourBeep hourBeep;
EditValue editValue;

unsigned long tubeModeStamp = 0;
byte returnTime = 0;
byte prevSec = 0;
byte currentRank = 0;
byte setDigit;
byte modeWork = 0;
byte tubeMode;
// Сегменты     | G  | F | E | D | C | B | A | DP |
byte pinsD[8] = { 10,  9,  8,  7,  6,  5,  4,  11 };
// Катоды слева-направо
//byte rankD[4] = { 13, 12, A1, A0 };
byte rankD[4] = { A0, A1, 12, 13 };

byte digitSymb[10] = {
	B01111110,  // 0
	B00110000,  // 1
	B01101101,  // 2
	B01111001,  // 3
	B00110011,  // 4
	B01011011,  // 5
	B01011111,  // 6
	B01110000,  // 7
	B01111111,  // 8
	B01111011   // 9
};

byte specSymb[8] = {
	B00000000,	// 0 - Выключенный разряд
	B01011111,	// 1 - Символ "Б" - Будильник
	B00111101,  // 2 - Символ "d" - день недели.
	B01110111,	// 3 - Символ "A" - аккумулятор
	B00001111,	// 4 - Символ "t" - температура
	B01001110,	// 5 - Символ "C" - секунды
	B01000111,	// 6 - Символ "F" - частота (яркость)
	B00001110	// 7 - Символ "L" - Составляющая для AL
};

Beeper test;

void setEditDigitLimit(char lower, char upper);
void assignEditDigit();
void tubeAsMode();
void showInfo();
void buttonClick();
void changeModeWork();
void buttonDoubleClick();
void buttonLongPressStart();
void buttonLongPress();
void chasngeModeWork();
void setBrightDuration(byte brightMode);
void getAutoBright(boolean = false);
void resetTubeMode();
void saveSettings();
void resetTubeFlash();
void syncTime(boolean = true);
byte getMaxMounthDay();
void beepController(Beeper* beeper);
void sleepMode();

volatile boolean alarmInt = false;

void wakeUp() {};
void alarmWakeUp() {
	alarmInt = true;
};

void setup() { 
	RTC = new DS3231RTC();
	syncTime(true);
	
	test.canBeep = true;
	test.count = 10;
	test.durationActive = 1000;
	test.durationInactive = 500;
	test.initCount = 10;
	test.isPinActive = false;
	test.timeOut = 3000;
	test.isCanInit = true;
	 
	if (eeprom_read_byte(&eeprom_init) != 100) {
		eeprom_write_byte(&eeprom_bright, 3);
		eeprom_write_byte(&eeprom_isHourBeepActive, 1);
		eeprom_write_byte(&eeprom_hourBeepStart, 7);
		eeprom_write_byte(&eeprom_hourBeepStop, 22);
		eeprom_write_byte(&eeprom_isAlarmActive, 0);
		eeprom_write_byte(&eeprom_alarmHour, 7);
		eeprom_write_byte(&eeprom_alarmMin, 0);
		eeprom_write_byte(&eeprom_alarmDow, 1);
		eeprom_write_byte(&eeprom_init, 100);
	}

	pinMode(BRIGHT, INPUT);
	pinMode(BATTERY, INPUT);
	pinMode(13, OUTPUT);
	pinMode(BEEPER, OUTPUT);

	for (byte i = 0; i < 4; i++) {
		pinMode(rankD[i], OUTPUT);
		digitalWrite(rankD[i], 1);
	}
	digitalWrite(rankD[0], 0);

	for (byte i = 0; i < 9; i++) {
		pinMode(pinsD[i], OUTPUT);
	}
	
	alarmClock.isActive = eeprom_read_byte(&eeprom_isAlarmActive);
	alarmClock.hour = eeprom_read_byte(&eeprom_alarmHour);
	alarmClock.minute = eeprom_read_byte(&eeprom_alarmMin);
	alarmClock.dowState = eeprom_read_byte(&eeprom_alarmDow);
	alarmClock.alarmAutoStopDelay = 120000; // 2 минуты орет и потом откладывает ор на 5 минут
	
	hourBeep.isActive = eeprom_read_byte(&eeprom_isHourBeepActive);
	hourBeep.start = eeprom_read_byte(&eeprom_hourBeepStart);
	hourBeep.stop = eeprom_read_byte(&eeprom_hourBeepStop);
	
	bright.mode = eeprom_read_byte(&eeprom_bright);
	setBrightDuration(bright.mode);
	
	button.pressStamp = 0;
	button.longPressTimeDetector = 1300;
	buttonCallback.attachClick(buttonClick);
	buttonCallback.attachDoubleClick(buttonDoubleClick);
	buttonCallback.attachLongPressStart(buttonLongPressStart);
	buttonCallback.attachDuringLongPress(buttonLongPress);
	buttonCallback.setDebounceTicks(30);
	buttonCallback.setClickTicks(200);
	buttonCallback.setPressTicks(button.longPressTimeDetector);
	
	modeWork = mw_clock;
	tubeMode = tm_showTime;
	tubeModeStamp = millis();
	
	attachInterrupt(digitalPinToInterrupt(ALARM), alarmWakeUp, FALLING );
}