#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED1_PIN   0
#define LED2_PIN   1

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 30

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip1(LED_COUNT, LED1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED2_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void initialize(){
  for(int i=0; i<6; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    for(int j=0; j<5; j++){
      strip1.setPixelColor(i*5+j, strip1.Color(0, 50, 0));
      strip2.setPixelColor(i*5+j, strip2.Color(0, 50, 0));
    }
    strip1.show();   // Send pixels1.setPixelColor(i, pixels.Color(0, 150, 0));the updated pixel colors to the hardware.
    strip2.show();   // Send pixels1.setPixelColor(i, pixels.Color(0, 150, 0));the updated pixel colors to the hardware.
    delay(1050); // Pause before next pass through loop
  }
} 

// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip2.begin();
  strip1.show();            // Turn OFF all pixels ASAP
  strip2.show();
  strip1.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip2.setBrightness(50);
  initialize();
  strip1.clear();         //   Set all pixels in RAM to 0 (off)
  strip2.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  Fire(55,120,15);
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[LED_COUNT];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < LED_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / LED_COUNT) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= LED_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = LED_COUNT; j > 0; j--) {
    setPixelHeatColor(j, heat[abs(j-30)] );
  }

  strip1.show();
  strip2.show();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    strip1.setPixelColor(Pixel, 255, 255, heatramp);
    strip2.setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    strip1.setPixelColor(Pixel, 255, heatramp, 0);
    strip2.setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    strip1.setPixelColor(Pixel, heatramp, 0, 0);
    strip2.setPixelColor(Pixel, heatramp, 0, 0);
  }
}
