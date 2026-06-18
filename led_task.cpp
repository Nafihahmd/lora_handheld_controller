#include "led_task.h"
#include "app.h"

#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel pixels(
    NUM_PIXELS,
    PIXEL_PIN,
    NEO_GRB + NEO_KHZ800);

void ledTaskInit(void)
{
    pixels.begin();
    pixels.clear();
    pixels.show();
}

void startupLedPattern()
{
    for(int i=0; i<(NUM_PIXELS/2); i++){
        pixels.setPixelColor(i*2, pixels.Color(30,30,30));
        pixels.setPixelColor((i*2+1), pixels.Color(30,30,30));
        pixels.show();
        delay(200);
        pixels.clear();
    }

}

void ledTask(void *pv)
{
    bool blink = false;
    pinMode(LED_CTRL, OUTPUT);
    digitalWrite(LED_CTRL, LOW);

    startupLedPattern();

    while(1)
    {
        blink = !blink;

        for(int i=0;i<NUM_PIXELS;i++)
        {
            switch(ledStates[i])
            {
                case LED_OFF:

                    pixels.setPixelColor(
                        i,
                        0);
                    break;

                case LED_READY:

                    pixels.setPixelColor(
                        i,
                        pixels.Color(
                            0,100,0));
                    break;

                case LED_TRIGGERED:

                    pixels.setPixelColor(
                        i,
                        pixels.Color(
                            100,0,0));
                    break;

                case LED_ERROR:

                    pixels.setPixelColor(
                        i,
                        pixels.Color(
                            100,100,0));
                    break;

                case LED_WAITING:

                    if(blink)
                    {
                        pixels.setPixelColor(
                            i,
                            pixels.Color(
                                0,0,100));
                    }
                    else
                    {
                        pixels.setPixelColor(
                            i,
                            0);
                    }
                    break;
                
                case LED_TIMEOUT:
                        pixels.setPixelColor(
                            i,
                            pixels.Color(
                                0,0,100));
                    break;

            }
        }

        pixels.show();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}