enum SegmentType
{
    HourSegment1,
    HourSegment2,
    MinuteSegment1,
    MinuteSegment2,
    SecondSegment1,
    SecondSegment2
};

class Time
{
private:
    char _seconds;
    char _minutes;
    char _hours;

public:
    Time(char hours, char minutes, char seconds)
    {
        this->_seconds = seconds > 99 ? 0 : seconds;
        this->_minutes = minutes > 99 ? 0 : minutes;
        this->_hours = hours > 99 ? 0 : hours;
    }
    char GetSeconds()
    {
        return _seconds;
    }
    char GetMinutes()
    {
        return _minutes;
    }
    char GetHours()
    {
        return _hours;
    }

    char GetCharSegment(SegmentType charType)
    {
        switch (charType)
        {
        case HourSegment1:
            return (_hours - _hours % 10) / 10 + 48;
            break;
        case HourSegment2:
            return _hours % 10 + 48;
            break;
        case MinuteSegment1:
            return (_minutes - _minutes % 10) / 10 + 48;
            break;
        case MinuteSegment2:
            return _minutes % 10 + 48;
            break;
        case SecondSegment1:
            return (_seconds - _seconds % 10) / 10 + 48;
            break;
        case SecondSegment2:
            return _seconds % 10 + 48;
            break;
        }
        return 0;
    }
};