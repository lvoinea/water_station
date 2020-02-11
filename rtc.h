#define countof(a) (sizeof(a) / sizeof(a[0]))

void print_date_time(const Time& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.mon,
            dt.date,
            dt.year,
            dt.hour,
            dt.minute,
            dt.sec);
    Serial.print(datestring);
}

void print_time(const Time& dt)
{
    char timestring[9];

    snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u:%02u:%02u"),
            dt.hour,
            dt.minute,
            dt.sec);
    Serial.print(timestring);
}

void read_date_time(byte& day, byte& month, byte& year, byte& dow, 
    byte& hour, byte& minute, byte& second) {
    
    // Call this if you notice something coming in on 
    // the serial port. The stuff coming in should be in 
    // the order DD/MM/YY w HH:MM:SS, with an '.' at the end.

    boolean string_complete = false;
    char in_char;
    byte number_part1, number_part2;
    char in_string[20];

    byte j=0;
    while (!string_complete) {
        if (Serial.available()) {
            in_char = Serial.read();
            in_string[j] = in_char;
            j += 1;
            if (in_char == '.') {
                string_complete = true;
                in_string[j]=0;
            }
        }
    }

    number_part1 = (byte)in_string[0] -48;
    number_part2 = (byte)in_string[1] -48;
    day = number_part1*10 + number_part2;

    number_part1 = (byte)in_string[3] -48;
    number_part2 = (byte)in_string[4] -48;
    month = number_part1*10 + number_part2;

    number_part1 = (byte)in_string[6] -48;
    number_part2 = (byte)in_string[7] -48;
    year = number_part1*10 + number_part2;

    dow = (byte)in_string[9] -48;

    number_part1 = (byte)in_string[11] -48;
    number_part2 = (byte)in_string[12] -48;
    hour = number_part1*10 + number_part2;

    number_part1 = (byte)in_string[14] -48;
    number_part2 = (byte)in_string[15] -48;
    minute = number_part1*10 + number_part2;

    number_part1 = (byte)in_string[17] -48;
    number_part2 = (byte)in_string[18] -48;
    second = number_part1*10 + number_part2;
}
