#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>

// === PIN CONFIGURATION ===
#define CS_PIN 5
#define INT_PIN 4
#define PWM_PIN 15
#define BATTERY_VOLTAGE_PIN A0

// === MCP CAN OBJECT ===
MCP_CAN CAN(CS_PIN);

// === FUNCTION PROTOTYPES ===
void initCAN();
void transmitHeartbeat(float dutyCycle, float resistance, float batteryVoltage);
void transmitPWMError();
void transmitCANMessage(const byte *data, byte length, uint16_t id);
void receiveCANMessage();
void readPWMandReport();
float readBatteryVoltage();

// === SETUP ===
void setup()
{
  Serial.begin(115200);
  pinMode(PWM_PIN, INPUT);
  pinMode(INT_PIN, INPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  initCAN();
}

// === MAIN LOOP ===
void loop()
{
  readPWMandReport();
  receiveCANMessage();
  delay(250);
}

// === INITIALIZE CAN ===
void initCAN()
{
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    Serial.println("MCP2515 Initialized");
  }
  else
  {
    Serial.println("Error Initializing MCP2515");
    while (1)
      ;
  }
  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN Bus active");
}

// === GENERAL CAN MESSAGE TRANSMISSION ===
void transmitCANMessage(const byte *data, byte length, uint16_t id)
{
  if (CAN.sendMsgBuf(id, 0, length, data) == CAN_OK)
  {
    Serial.println("CAN Message Sent");
  }
  else
  {
    Serial.println("CAN Message Send Error");
  }
}

// === PWM MONITORING + BATTERY READING + HEARTBEAT ===
void readPWMandReport()
{
  unsigned long highTime = pulseIn(PWM_PIN, HIGH, 1000000);
  unsigned long lowTime = pulseIn(PWM_PIN, LOW, 1000000);

  if (highTime == 0 || lowTime == 0)
  {
    Serial.println("PWM Error Detected!");
    transmitPWMError();
    return;
  }

  float dutyCycle = (float)highTime / (highTime + lowTime);
  // Protect against divide-by-near-zero around the model's lower duty threshold.
  if (dutyCycle <= 0.051f)
  {
    Serial.println("PWM Duty Cycle Out of Range!");
    transmitPWMError();
    return;
  }
  float resistance = (90.0f * 1.2f) / (dutyCycle - 0.05f) - 1.2f; // Example conversion model from datasheet guidance.

  float batteryVoltage = readBatteryVoltage();

  Serial.print("Duty Cycle: ");
  Serial.print(dutyCycle * 100.0);
  Serial.println("%");
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.println(" ohms");
  Serial.print("Battery: ");
  Serial.print(batteryVoltage);
  Serial.println(" V");

  transmitHeartbeat(dutyCycle, resistance, batteryVoltage);
}

// === HEARTBEAT TRANSMISSION ===
void transmitHeartbeat(float dutyCycle, float resistance, float batteryVoltage)
{
  byte data[8];
  data[0] = 0xAA;                    // Heartbeat ID
  data[1] = (byte)(dutyCycle * 100); // Percent (0-100)
  int r = (int)resistance;
  data[2] = (byte)(r >> 8);
  data[3] = (byte)(r & 0xFF);
  int bv = (int)(batteryVoltage * 100); // e.g., 12.34V becomes 1234
  data[4] = (byte)(bv >> 8);
  data[5] = (byte)(bv & 0xFF);

  transmitCANMessage(data, 6, 0x101);
}

// === PWM ERROR HANDLER ===
void transmitPWMError()
{
  byte errorData[2] = {0xEE, 0x01};
  transmitCANMessage(errorData, 2, 0x102); //Random Hex value to act as the error ID
}

// === BATTERY VOLTAGE READING ===
float readBatteryVoltage()
{
  int analogValue = analogRead(BATTERY_VOLTAGE_PIN);
  float voltage = analogValue * (3.3 / 4095.0);
  float batteryVoltage = voltage * (12.0 / 3.3);
  return batteryVoltage;
}

// === CAN RECEIVE FUNCTION ===
void receiveCANMessage()
{
  if (!digitalRead(INT_PIN))
  {
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    if (CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
    {
      Serial.print("Message Received ID: 0x");
      Serial.println(rxId, HEX);
      Serial.print("Data: ");
      for (int i = 0; i < len; i++)
      {
        Serial.print(rxBuf[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
