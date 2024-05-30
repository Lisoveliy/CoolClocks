#include <Arduino.h>

class InputController
{
    bool _buttonState{true};
    bool _released{false};
    short updateTime;
    uint8_t port;

public:
    InputController(uint8_t port)
    {
        this->port = port;
        pinMode(port, INPUT_PULLUP);
        updateTime = millis();
    }
    void UpdateInput()
    {
        if(millis() - updateTime > 30){
            bool state = digitalRead(port);
            if(state != _buttonState){
                _buttonState = state;
                if(state != 0){
                    _released = 1;
                }
            }
            updateTime = millis();
        }
    }
    bool GetState()
    {
        return !_buttonState;
    }
    bool ButtonIsReleased(){
        bool released = _released;
        if(_released == 1){
            _released = 0;
        }
        return released;
    }
};