#include <Arduino.h>
#include "consts.h"
#include "utils.h"
#include "Logger.h"

void utils::read_cstring_from_serial(char* str, const index_t& length)
{
    index_t i = 0;
    str[0] = '\0';

    char ch = '\0';

    const millis_t end_time = millis() + KEYPRESS_TIMEOUT;

    do {
        if (Serial.available()) {
            ch = Serial.read();
            if (ch == 0x7F) { /* backspace ASCII char */
                if (i>0) {
                    i--;
                    Serial.write(8); /* backspace */
                    Serial.print(F(" ")); /* replace character with a space */
                    Serial.write(8); /* backspace */
                }
            } else {
                str[i++] = ch;
                Serial.print(str[i-1]); // echo
                if (i == length-1) break;
            }
        }
    } while (ch != '\r' && in_future(end_time));

    Serial.println(F(""));
    Serial.flush();
    str[i] = '\0';
}


uint32_t utils::read_uint32_from_serial()
{
    const index_t BUFF_LENGTH = 15; /* 10 chars + 1 sentinel char + extras for whitespace.
                                     * The max value a uint32 can store is
                                     * 4 billion (10 chars decimal). */
    char buff[BUFF_LENGTH];
    read_cstring_from_serial(buff, BUFF_LENGTH);

    if (buff[0] == '\0') {
        log(INFO, PSTR("timeout"));
        return UINT32_INVALID;
    }

    return  strtoul(buff, NULL, 0);
}


bool utils::in_future(const millis_t& deadline)
{
    const millis_t PUSH_FORWARD = 100000;

    if (millis() < deadline)
        return true;
    else if ((millis() + PUSH_FORWARD) < (deadline + PUSH_FORWARD))
        /* Try pushing both millis and deadline forward so they both roll over */
        return true;
    else
        return false;
}
