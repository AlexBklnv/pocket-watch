#include "main.h"


void loop() {
	syncTime(false);
	buttonCallback.tick();
	if (modeWork == mw_clock) {
			
	} else {
		if (returnTime == 0) {
			if (modeWork == mw_setBright) {
				setBrightDuration(bright.mode);
			}
		} else if (modeWork > mw_clock && prevSec != dateTime.Second) {
			returnTime--;
			prevSec = dateTime.Second;
		}
	}
	tubeAsMode();
	resetTubeMode();
	beepController(&test);
}

void setEditDigitLimit(char lower, char upper) {
  editValue.min = lower;
  editValue.max = upper;
}

void resetTubeFlash() {
	tube.isFlash[0] = false;
	tube.isFlash[1] = false;
	tube.isFlash[2] = false;
	tube.isFlash[3] = false;
	tube.isDisabled[0] = false;
	tube.isDisabled[1] = false;
	tube.isDisabled[2] = false;
	tube.isDisabled[3] = false;
}

void assignEditDigit() {
	resetTubeFlash();
	switch (modeWork) {
		case mw_setHour:
			editValue.value = dateTime.Hour;
			setEditDigitLimit(0, 23);
			tubeMode = tm_showTime;
			tube.isFlash[0] = true;
			tube.isFlash[1] = true;
			break;
		case mw_setMin:
			editValue.value = dateTime.Minute;
			setEditDigitLimit(0, 59);
			tubeMode = tm_showTime;
			tube.isFlash[2] = true;
			tube.isFlash[3] = true;
			break;
		case mw_setBright:
			editValue.value = bright.mode;
			setEditDigitLimit(0, 3);
			tubeMode = tm_showBright;
			tube.isFlash[3] = true;
			break;
		case mw_setAlarmState:
			editValue.value = alarmClock.isActive;
			setEditDigitLimit(0, 1);
			tubeMode = tm_showAlarmState;
			tube.isFlash[3] = true;
			break;
		case mw_setAlarmHour:
			editValue.value = alarmClock.hour;
			setEditDigitLimit(0, 23);
			tubeMode = tm_showAlarmTime;
			tube.isFlash[0] = true;
			tube.isFlash[1] = true;
			break;
		case mw_setAlarmMin:
			editValue.value = alarmClock.minute;
			setEditDigitLimit(0, 59);
			tubeMode = tm_showAlarmTime;
			tube.isFlash[2] = true;
			tube.isFlash[3] = true;
			break;
		case mw_setAlarmDow:
			editValue.value = bitRead(alarmClock.dowState, alarmClock.curDow);
			setEditDigitLimit(0, 1);
			alarmClock.curDow = 0;
			tubeMode = tm_showAlarmDow;
			tube.isFlash[3] = true;
			break;
		case mw_setHourBeepState:
			editValue.value = hourBeep.isActive;
			setEditDigitLimit(0, 1);
			tubeMode = tm_showHourBeepState;
			tube.isFlash[3] = true;
			break;
		case mw_setHourBeepStart:
			editValue.value = hourBeep.start;
			setEditDigitLimit(0, 23);
			tubeMode = tm_showHourBeepTime;
			tube.isFlash[0] = true;
			tube.isFlash[1] = true;
			break;
		case mw_setHourBeepStop:
			editValue.value = hourBeep.stop;
			setEditDigitLimit(0, 23);
			tubeMode = tm_showHourBeepTime;
			tube.isFlash[2] = true;
			tube.isFlash[3] = true;
			break;
		case mw_setYear:
			editValue.value = dateTime.Year;
			setEditDigitLimit(0, 99);
			tubeMode = tm_showYear;
			tube.isFlash[0] = true;
			tube.isFlash[1] = true;
			tube.isFlash[2] = true;
			tube.isFlash[3] = true;
			break;
		case mw_setDay:
			editValue.value = dateTime.Day;
			setEditDigitLimit(1, getMaxMounthDay());
			tubeMode = tm_showDate;
			tube.isFlash[0] = true;
			tube.isFlash[1] = true;
			break;
		case mw_setMonth:
			editValue.value = dateTime.Month;
			setEditDigitLimit(0, 12);
			tubeMode = tm_showDate;
			tube.isFlash[2] = true;
			tube.isFlash[3] = true;
			break;
	}
}
/*
 раскидываем по парам значения чисел для обычного режима, 
 для кастомки внутри свича само раскидывается
 startIndex - 0(1)|2(3) пара диодов, слева направо
 value - значение для разбиения
 */
void fillTubeDigitSymb(byte startIndex, byte value) {
	tube.value[startIndex] = digitSymb[ value < 10 ? 0: value / 10];
	tube.value[startIndex + 1] = digitSymb[value < 10? value: value % 10];
}

void fillTubeTemperature() {
	int value = RTC.temperatureC();
	tube.value[1] =	value >= 100? digitSymb[value / 100]: specSymb[0];
	value = value % 100;
	tube.value[2] = digitSymb[value / 10];
	tube.value[3] = digitSymb[value % 10];
} 

void tubeAsMode() {
	switch(tubeMode) {
		case tm_showTime:
			fillTubeDigitSymb(0, modeWork == mw_setHour?editValue.value: dateTime.Hour);
			fillTubeDigitSymb(2, modeWork == mw_setMin?editValue.value: dateTime.Minute);
			break;
		case tm_showBattery:
			tube.value[0] = specSymb[1];
			// процент
			tube.value[1] = specSymb[0];
			tube.value[2] = specSymb[0];
			tube.value[3] = specSymb[0];
			break;
		case tm_showTemperature:
			tube.value[0] = specSymb[4];
			fillTubeTemperature();
			break;
		case tm_showSec:
			fillTubeDigitSymb(0, dateTime.Minute);
			fillTubeDigitSymb(2, dateTime.Second);
			break;
		case tm_showBright:
			tube.value[0] = specSymb[6];
			tube.value[1] = specSymb[0];
			tube.value[2] = specSymb[0];
			tube.value[3] = digitSymb[editValue.value];
			setBrightDuration(editValue.value);
			break;
		case tm_showAlarmState:
			tube.value[0] = specSymb[5];
			tube.value[1] = specSymb[1];
			tube.value[2] = specSymb[0];
			tube.value[3] = digitSymb[editValue.value];
			break;
		case tm_showAlarmTime:
			fillTubeDigitSymb(0, modeWork == mw_setAlarmHour?editValue.value: alarmClock.hour);
			fillTubeDigitSymb(2, modeWork == mw_setAlarmMin?editValue.value: alarmClock.minute);
			break;
		case tm_showAlarmDow:
			tube.value[0] = specSymb[2];
			tube.value[1] = digitSymb[alarmClock.curDow + 1];
			tube.value[2] = specSymb[0];
			tube.value[3] = digitSymb[editValue.value];
			break;
		case tm_showHourBeepState:
			tube.value[0] = specSymb[5];
			tube.value[1] = digitSymb[4];
			tube.value[2] = specSymb[0];
			tube.value[3] = digitSymb[editValue.value];
			break;
		case tm_showHourBeepTime:
			fillTubeDigitSymb(0, modeWork == mw_setHourBeepStart?editValue.value: hourBeep.start);
			fillTubeDigitSymb(2, modeWork == mw_setHourBeepStop?editValue.value: hourBeep.stop);
			break;
		case tm_showYear:
			fillTubeDigitSymb(0, 20);
			fillTubeDigitSymb(2, editValue.value);
			break;
		case tm_showDate:
			fillTubeDigitSymb(0, modeWork == mw_setDay?editValue.value: dateTime.Day);
			fillTubeDigitSymb(2, modeWork == mw_setMonth?editValue.value:  dateTime.Month);
			break;
		case tm_displayOff:
			tube.value[0] = specSymb[0];
			tube.value[1] = specSymb[0];
			tube.value[2] = specSymb[0];
			tube.value[3] = specSymb[0];
			break;
	}

	if (modeWork != mw_setBright) {
		getAutoBright(false);
	}
	
	// хватаем следующую лампу
	currentRank = currentRank == 3 ? 0 : currentRank + 1;
	for (byte i = 0; i < 8; i++) {
		digitalWrite(pinsD[i], bitRead(tube.value[currentRank], i));
	}
	
	if ((tubeMode == tm_showTime || tubeMode == tm_showSec) && currentRank == 1) {
		digitalWrite(pinsD[7], dateTime.Second % 2);
	}
	if (tubeMode == tm_showTemperature && currentRank == 2) {
		digitalWrite(pinsD[7], HIGH);
	}
	
	
	if (modeWork > mw_clock) {
		for (byte i = 0; i < 4; i++) {
			if (tube.isFlash[i]) {
				if (millis() - tube.switchTime >= 150) {
					tube.isDisabled[i] =  true;
					if (millis() - tube.switchTime >= 550) {
						tube.switchTime = millis() ;
					}
					} else {
					tube.isDisabled[i] = false;
				}
			} else {
				tube.isDisabled[i] = false;
			}
		}
	}
	
	if (!tube.isDisabled[currentRank]) {
		// врубаем лампу
		digitalWrite(rankD[currentRank], 0);
	}
	
	// чем больше задержка тем ярче горит
	delayMicroseconds(bright.delayDuration);

	// вырубаем текущую лампу
	digitalWrite(rankD[currentRank], 1);
	// вырубаем все катоды и вырубаем лампу
	for (byte i = 0; i < 8; i++) {
		digitalWrite(pinsD[i], LOW);
	}
	delayMicroseconds(1200);
}

void sleepMode() {
	attachInterrupt(digitalPinToInterrupt(BUTTON), wakeUp, FALLING );
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();
	detachInterrupt(digitalPinToInterrupt(BUTTON));
	if (alarmInt) {
		modeWork = mw_alarm;
		tubeMode = tm_showTime;
	} else {
		modeWork = mw_clock;
		tubeMode = tm_displayOff;
		tubeModeStamp = millis();
		buttonClick();
	}
}

void buttonClick() {
	if (modeWork == mw_clock) {
		if (tubeMode == tm_showSec) {
			sleepMode();
		} else {
			tubeMode++;
			tubeModeStamp = millis();
		}
	} else if (modeWork == mw_alarm) {
		// отложить будильник
	} else {
		prevSec = dateTime.Second;
		returnTime = 10;
		editValue.value++;
		if (editValue.value > editValue.max) {
			editValue.value = editValue.min;
		}
	}
}

void resetTubeMode() {
	if (modeWork == mw_clock) {
		if(tubeMode != tm_showSec) {
			if (millis() - tubeModeStamp >= 3000) {
				sleepMode();
			}
		}
	}
}


void changeModeWork() {
	prevSec = dateTime.Second;
	returnTime = 10;
	if (modeWork != mw_clock) {
		saveSettings();
	}
	switch(modeWork) {
		case mw_setAlarmState:
			if (!alarmClock.isActive) {
				modeWork = mw_setHourBeepState;
				assignEditDigit();
				return;
			}
			break;
		case mw_setHourBeepState: 
			if (!hourBeep.isActive) {
				modeWork = mw_setHourBeepState;
				assignEditDigit();
				return;
			}
			break;
		case mw_setAlarmDow:
			if (alarmClock.curDow < 6) {
				alarmClock.curDow++;
				assignEditDigit();
				return;
			}
			break;
	}
	if (modeWork == mw_lastMode) {
		modeWork = mw_clock;
		resetTubeFlash();
	} else {
		modeWork++;
		assignEditDigit();
	}

}

void buttonDoubleClick() {
	if (modeWork != mw_clock) {
		prevSec = dateTime.Second;
		returnTime = 10;
		editValue.value--;
		if (editValue.value <= editValue.min) {
			editValue.value = editValue.max;
		}
	}
}

void buttonLongPressStart() {
  changeModeWork();
  button.pressStamp = millis();
}

void buttonLongPress() {
  if (millis() - button.pressStamp >= button.longPressTimeDetector) {
    changeModeWork();
    button.pressStamp = millis();
  }
}

void setBrightDuration(byte brightMode) {
	switch(brightMode) {
		case 0:
			getAutoBright(true);
			break;
		case 1:
			bright.delayDuration = 50;
			break;
		case 2:
			bright.delayDuration = 200;
			break;
		case 3:
			bright.delayDuration = 2000;
			break;
	}
}

void getAutoBright(bool immediately) {
	if (immediately || bright.mode == 0) {
		int _brightValue = analogRead(BRIGHT) * 4.76;
		bright.delayDuration = _brightValue > 2000 ? 0: 2000 - _brightValue;
		if (bright.delayDuration < 50) {
			bright.delayDuration = 50;
		}
	}
}

byte getMaxMounthDay() {
	if (dateTime.Month == 2) {
		return dateTime.Year % 4 == 0 ? 29 : 28;
	} else {
		if (dateTime.Month < 8) {
			return dateTime.Month % 2 == 0? 30 : 31;
		} else {
			return dateTime.Month % 2 == 1? 30 : 31;
		}
	}
}

// деление без остатка
unsigned int f_div(unsigned int x, unsigned int y) {
	return (unsigned int)(x- (x % y)) / y;
}


void setDayOfWeek() {
	byte _a = f_div((14-dateTime.Month),12);
	unsigned int y = dateTime.Year + 2000 - _a;
	byte result = (dateTime.Day + (dateTime.Year + 2000 - _a) + f_div(y, 4) - f_div(y, 100) 
		+ f_div(y, 400) + f_div((dateTime.Month + 12 * _a - 2) * 31, 12)) % 7;
	dateTime.Wday = result == 0? 7: result;
}

void saveSettings() {
	byte _maxMonthDay;
	switch (modeWork) {
		case mw_setHour:
			RTC.writeRTC(RTC_HOURS, RTC.dec2bcd(editValue.value));
			syncTime();
			break;
		case mw_setMin:
			RTC.writeRTC(RTC_MINUTES, RTC.dec2bcd(editValue.value));
			syncTime();
			break;
		case mw_setBright:
			bright.mode = editValue.value;
			eeprom_update_byte(&eeprom_bright, bright.mode);
			setBrightDuration(bright.mode);
			break;
		case mw_setAlarmState:
			alarmClock.isActive = editValue.value;
			eeprom_update_byte(&eeprom_isAlarmActive, alarmClock.isActive);
			break;
		case mw_setAlarmHour:
			alarmClock.hour = editValue.value;
			eeprom_update_byte(&eeprom_alarmHour, alarmClock.hour);
			break;
		case mw_setAlarmMin:
			alarmClock.minute = editValue.value;
			eeprom_update_byte(&eeprom_alarmMin, alarmClock.minute);
			break;
		case mw_setAlarmDow:
			bitWrite(alarmClock.dowState, alarmClock.curDow, editValue.value);
			if (alarmClock.curDow == 6) {
				eeprom_update_byte(&eeprom_alarmDow, alarmClock.dowState);
			}
			break;
		case mw_setHourBeepState:
			hourBeep.isActive = editValue.value;
			eeprom_update_byte(&eeprom_isHourBeepActive, hourBeep.isActive);
			break;
		case mw_setHourBeepStart:
			hourBeep.start = editValue.value;
			eeprom_update_byte(&eeprom_hourBeepStart, hourBeep.start);
			break;
		case mw_setHourBeepStop:
			hourBeep.stop = editValue.value;
			eeprom_update_byte(&eeprom_hourBeepStop, hourBeep.stop);
			break;
		case mw_setYear:
			RTC.writeRTC(RTC_YEAR, RTC.dec2bcd(editValue.value));
			_maxMonthDay = getMaxMounthDay();
			if (_maxMonthDay > dateTime.Day) {
				RTC.writeRTC(RTC_DATE, RTC.dec2bcd(_maxMonthDay));
			}
			setDayOfWeek();
			RTC.writeRTC(RTC_DAY, dateTime.Wday);
			syncTime();
			break;
		case mw_setDay:
			RTC.writeRTC(RTC_DATE, RTC.dec2bcd(editValue.value));
			setDayOfWeek();
			RTC.writeRTC(RTC_DAY, dateTime.Wday);
			syncTime();
			break;
		case mw_setMonth:
			RTC.writeRTC(RTC_MONTH, RTC.dec2bcd(editValue.value));
			_maxMonthDay = getMaxMounthDay();
			if (_maxMonthDay > dateTime.Day) {
				RTC.writeRTC(RTC_DATE, RTC.dec2bcd(_maxMonthDay));
			}
			setDayOfWeek();
			RTC.writeRTC(RTC_DAY, dateTime.Wday);
			syncTime();
			break;
	}
	
	
}

void syncTime(boolean ignoreStamp) {
	if (millis() - syncTimeStamp >= 300 || ignoreStamp) {
		RTC.read(dateTime);
		syncTimeStamp = millis();
	}
}


void beepController(Beeper* beeper) {
	if (!beeper->canBeep) {
		digitalWrite(BEEPER, LOW);
		beeper->isPinActive = false;
		return;
	}
	if (beeper->isCanInit) {
		beeper->isPinActive = true;
		beeper->count = beeper->initCount - 1;
		beeper->isCanInit = false;
		digitalWrite(BEEPER, HIGH);
		beeper->timeStamp = millis();
	} else {
		if (beeper->isPinActive) {
			if (millis() - beeper->timeStamp >= beeper->durationActive) {
				beeper->isPinActive = false;
				digitalWrite(BEEPER, LOW);
				beeper->timeStamp = millis();
			}
		} else {
			if (beeper->count > 0) {
				if (millis() - beeper->timeStamp >= beeper->durationInactive) {
					beeper->isPinActive = true;
					beeper->count--;
					digitalWrite(BEEPER, HIGH);
					beeper->timeStamp = millis();
				}
			} else if (beeper->timeOut != 0 && millis() - beeper->timeStamp >= beeper->timeOut){
				beeper->isCanInit = true;
			}
		}
	}
}