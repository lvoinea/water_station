#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const Time& dt)
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
