#include "DigitalIn.h"
#include "PinNames.h"
#include "PinNamesTypes.h"
#include "mbed.h"
#include "mbed_wait_api.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>

using namespace std;

int B1_Byte_Shift = 0x01;
int B2_Byte_Shift = 0x02;
int B3_Byte_Shift = 0x04;

int ComandletDirectorTresholdMs = 150;

PinName B1_PIN = D2;
PinName B2_PIN = D4;
PinName B3_PIN = D7;

string password = "1113";


DigitalIn K1(B1_PIN);
DigitalIn K2(B2_PIN);
DigitalIn K3(B3_PIN);
DigitalOut myled(LED1);

void setUp(){
    K1.mode(PullUp);
    K2.mode(PullUp);
    K3.mode(PullUp);
}

void readButtons(){
        K1.read();
        K2.read();
        K3.read();
}

string inputString(int length){
    int r = 0;
    string entered = "";

    while (r != length) {
        readButtons();

        if(K1 == 0){
            entered.append("1");
        }
        if(K2 == 0){
            entered.append("2");
        }
        if(K3 == 0){
            entered.append("3");
        }

        if(entered[r]){
            r++;
            wait_ms(250);
        }
    }

    return entered.c_str();
}



char parseByte(int digit){
    if(digit == 0x04){
        return '3';
    }

    return '0' + digit;
}

bool enterPassword(char first) {
    string entered = "";
    string externalInput = "";

    if(first == '0'){
        externalInput = inputString(4);
    } else {
        entered += first;
        entered.c_str();
        externalInput = inputString(3);
    }
    entered = entered.append(externalInput);

    if(entered.compare(password) == 0){
        myled = true;
        wait_ms(175);
        myled = false;
        wait_ms(225);
        myled = true;
        wait_ms(175);
        myled = false;
        wait_ms(225);
        myled = true;
        wait_ms(175);
        myled = false;

        return true;
    } else {
        myled = true;
        wait_ms(500);
        myled =false;
        wait_ms(500);
        myled = true;
        wait_ms(500);
        myled =false;

        return false;
    }
}

void rememberPassword(char first){
    if(enterPassword(first)){
        myled = true;

        string newPassword = inputString(4);

        password = newPassword;

        myled = false;
    }
}

void commandletListner() {
    int buffer = 0x00;
    int tresholdCounter = 5;

    while (1) {
        int tempBuffer = 0x00;

        readButtons();

        if(tresholdCounter == 0){

            switch (buffer) {
                case 0x01:
                case 0x02:
                case 0x04:
                    enterPassword(parseByte(buffer));
                    break;

                case 0x03:
                    rememberPassword('0');
                    break;
            }

            return;
        }

        if (K1 == 0){
            tempBuffer += B1_Byte_Shift;
        }
        if (K2 == 0){
            tempBuffer += B2_Byte_Shift;
        }
        if (K3 == 0){
            tempBuffer += B3_Byte_Shift;
        }

        if(tempBuffer == 0x00 & buffer == 0x00){
            tresholdCounter = 5;
            buffer = 0x00;
        }

        if(tempBuffer == 0x00 & buffer != 0x00){
            tresholdCounter = 0;
            continue;
        }

        if(tempBuffer == buffer){
            tresholdCounter--;
        } else {
            tresholdCounter = 5;
        }

        buffer = tempBuffer;

        wait_ms(ComandletDirectorTresholdMs);
    }
}


int main() {
    setUp();

    while(1) {
        commandletListner();
    }
}