#include <headers/displaytime.h>
#include <MD_MAX72xx.h>

class DisplayController
{
public:
    static void setTime(Time *time, MD_MAX72XX* display, char length, char numberLength)
    {
        char counter = length;
        display->setChar(counter, time->GetCharSegment(HourSegment1));
        counter -= numberLength;
        display->setChar(counter, time->GetCharSegment(HourSegment2));

        counter--;
        counter -= numberLength;
        display->setChar(counter, time->GetCharSegment(MinuteSegment1));
        counter -= numberLength;
        display->setChar(counter, time->GetCharSegment(MinuteSegment2));

        counter--;
        counter -= numberLength;
        display->setChar(counter, time->GetCharSegment(SecondSegment1));
        counter -= numberLength;
        display->setChar(counter, time->GetCharSegment(SecondSegment2));
        delete time;
    }
    static void print(char* word, char len, char screenLen, MD_MAX72XX* display)
    {
        char symLen = 5;
        for(int i = 0; i < len; i++){
            char l = word[i] == 'i' ? symLen : symLen + 1;
			display->setChar(screenLen - (l * i), word[i]);
        }
    }
};