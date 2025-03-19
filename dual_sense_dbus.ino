// Include the BMC_SBUS library
#include <BMC_SBUS.h>
#include <ps5Controller.h>

//Declare BMC_SBUS Object
BMC_SBUS mySBUS;

// Sbus delay value
const int sbusWAIT = 7;      //frame timing delay in msecs

// Declare sbus control channels
int panChannel = 1;
int tiltChannel = 2;
int rollChannel = 4;

void setup() {
  // Start BMC_SBUS object
  mySBUS.begin();

  // Start serial
  Serial.begin(115200);
  Serial.println("Ready.");

  // Start 
  ps5.begin("24:A6:FA:09:85:4A"); //replace with MAC address of your controller
}

void loop() {
    while (ps5.isConnected() == true) {
      if (ps5.Right()) Serial.println("Right Button");
      if (ps5.Down()) Serial.println("Down Button");
      if (ps5.Up()) Serial.println("Up Button");
      if (ps5.Left()) Serial.println("Left Button");

      if (ps5.Square()) Serial.println("Square Button");
      if (ps5.Cross()) Serial.println("Cross Button");
      if (ps5.Circle()) Serial.println("Circle Button");
      if (ps5.Triangle()) Serial.println("Triangle Button");

      if (ps5.UpRight()) Serial.println("Up Right");
      if (ps5.DownRight()) Serial.println("Down Right");
      if (ps5.UpLeft()) Serial.println("Up Left");
      if (ps5.DownLeft()) Serial.println("Down Left");

      if (ps5.L1()) Serial.println("L1 Button");
      if (ps5.R1()) Serial.println("R1 Button");

      if (ps5.Share()) Serial.println("Share Button");
      if (ps5.Options()) Serial.println("Options Button");
      if (ps5.L3()) Serial.println("L3 Button");
      if (ps5.R3()) Serial.println("R3 Button");

      if (ps5.PSButton()) Serial.println("PS Button");
      if (ps5.Touchpad()) Serial.println("Touch Pad Button");

      if (ps5.L2()) {
        Serial.printf("L2 button at %d\n", ps5.L2Value());
      }
      if (ps5.R2()) {
        Serial.printf("R2 button at %d\n", ps5.R2Value());
      }

      if (ps5.LStickX()) {
        Serial.printf("Left Stick x at %d\n", ps5.LStickX());
      }
      if (ps5.LStickY()) {
        Serial.printf("Left Stick y at %d\n", ps5.LStickY());
      }
      if (ps5.RStickX()) {
        Serial.printf("Right Stick x at %d\n", ps5.RStickX());
      }
      if (ps5.RStickY()) {
        Serial.printf("Right Stick y at %d\n", ps5.RStickY());
      }
      Serial.println();

      // Map dualsense values which go from -128-128 to Sbus values 0-2047
      // For no move send 1023
      int sendValueY = map(ps5.LStickX(),0,255,0,2047);
      int sendValueX = map(ps5.LStickY(),0,255,0,2047);
      int sendValueZ = map(ps5.RStickX(),0,255,0,2047);

      // Set sbus tilt
      mySBUS.Servo(tiltChannel,sendValueY);
      mySBUS.Servo(rollChannel,sendValueX);         
      mySBUS.Servo(panChannel,sendValueZ);

      // Update SBUS object and send data
      mySBUS.Update();
      mySBUS.Send();
      // Delay for SBUS
      delay(sbusWAIT);

      // This delay is to make the output more human readable
      // Remove it when you're not trying to see the output
      delay(300);
    }
}