#include <Arduino.h>
#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"

#define SIZE_ADDRESSPINS 8
#define SIZE_DATAPINS 8
#define SIZE_CARDPINS 4

#define INPUT_SELECTOR 0
#define OUTPUT_SELECTOR 1
#include <CmdMessenger.h>
#include <string.h>
// #include <RemoteDebugger.h>

enum class Register
{
  // 00H
  MEASURE = 0,
  // 01H
  BUSCON0 = 1,
  // 02H
  BUSCON1 = 2,
  // 03H
  GNDCON0 = 3,
  // 04H
  GNDCON1 = 4,
  // 05H
  SOURCE = 5,
  // 06H
  DACDATA0 = 6,
  // 07H
  DACDATA1 = 7,
  // 08H
  ERROR_FLAGS = 8,
  // 09H
  RANGE = 9,
  // 0AH
  WAVE_CTRL = 10,
  // 0BH
  WAVE_AMPL0 = 11,
  // 0CH
  WAVE_AMPL1 = 12,
  // 0DH
  WAVE_FREQ0 = 13,
  // 0EH
  WAVE_FREQ1 = 14,
  // 0FH
  WAVE_DUTY = 15,
  // FAH
  READ_SOURCE = 250,
  // FBH
  READ_BUSCON0 = 251,
  // FCH
  READ_BUSCON1 = 252,
  // FDH
  READ_GNDCON0 = 253,
  // FE
  READ_GNDCON1 = 254,
  // FFH
  IDENT = 255,
};
enum class MeasRange
{
  /// Do not strip down.
  Bi10 = 1,
  /// Strip down with a divider factor of 3
  Bi30 = 3,
  /// Strip down with a divider factor of 12
  Bi120 = 12,
};

bool led = false;
// BoardNr
// Must be able to be changed in GUI
int boardNumber = 0x00;

// Data pins
// const int d0_pin = 2;
// const int d1_pin = 3;
// const int d2_pin = 4;
// const int d3_pin = 5;
// const int d4_pin = 6;
// const int d5_pin = 7;
// const int d6_pin = 8;
// const int d7_pin = 9;
const int datapins[SIZE_DATAPINS] = {2, 3, 4, 5, 6, 7, 8, 9};

// Address pins
// const int a0_pin = 18;
// const int a1_pin = 19;
// const int a2_pin = 20;
// const int a3_pin = 21;
// const int a4_pin = 22;
// const int a5_pin = 23;
// const int a6_pin = 24;
// const int a7_pin = 25;
const int addresspins[SIZE_ADDRESSPINS] = {18, 19, 20, 21, 22, 23, 24, 25};

// Card address pins
// const int a8_pin = 37;
// const int a9_pin = 38;
// const int a10_pin = 39;
// const int a11_pin = 40;
const int cardAddresspins[SIZE_CARDPINS] = {37, 38, 39, 40};

// Controller pins
// Remark: active low
const int WR = 51;
const int RD = 52;
const int RESET = 53;

// Pull_up pins
const int ACK = 28;
const int ERR = 29;

const byte AD0 = A14;
const byte AD1 = A13;

/// The maximum number of acknowledge check retries.
const int MAX_ACK_CHECK_RETRIES = 100;
/// The number of AIO channels for one board.
const int AIO_CHANNELS = 16;
/// Time-out to switch relay on.
static int RELAY_ON_SETTLING = 5;
/// Time-out to switch relay off.
static int RELAY_OFF_SETTLING = 1;
/// The input impedance of the measure circuit. (1M2)
const double MEAS_INPUT_IMP = 1200000;

/// 2 registers - each 1 byte - in total 2 bytes
/// The data 0 status register, needed for u an i source
int dacData0Status;
/// The data 1 status register, needed for u an i source
int dacData1Status;

/// The source status register
int sourceStatus;

/// 2 registers - each 1 byte - in total 2 bytes
/// The bus cofnection 0 status register
int busCon0Status;
/// The bus cofnection 1 status register
int busCon1Status;

/// 2 registerf - each 1 byte - in total 2 bytes
/// The ground connection 0 status register.
int gndCon0Status;
/// The ground connection 1 status register.
int gndCon1Status;

/// The measure status register.
int measureStatus;
/// The U/I bus status register.
int rangeStatus = 0;

/// Status of channels (connected to ground/connected to bus)
bool gndChannelStatus[16];
bool busChannelStatus[16];

// ------------------  T E S T  F U N C T I O N A L I T Y ------------------
#line 158 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void setupStatus();
#line 188 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void testFullFunctionallity();
#line 232 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void attachCommandCallbacks();
#line 247 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void showPossibleCommands();
#line 258 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void onUnknownCommand();
#line 265 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void toggleLed();
#line 273 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void setVoltageSerial();
#line 290 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void connectToGroundSerial();
#line 301 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void connectToBusSerial();
#line 314 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void setup();
#line 333 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void loop();
#line 6 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void writeData(Register chosenReg, int data, int boardNumber);
#line 60 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
int readData(Register chosenReg, int boardNumber);
#line 117 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void printGNDStatus(int status0_before, int status0_after, int status1_before, int status1_after);
#line 140 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void connectToGround(int channel, bool status);
#line 197 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void printBusStatus(int status0_before, int status0_after, int status1_before, int status1_after);
#line 220 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void connectToBus(int channel, bool status);
#line 277 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void printConnectVoltageStatus(int before, int after);
#line 291 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void connectVoltageSource(bool status);
#line 317 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void printSetVoltageStatus(int status0_before, int status0_after, int status1_before, int status1_after);
#line 339 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
void setVoltage(double voltage);
#line 371 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
double measureVoltage(int channel);
#line 403 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
double measureCurrentUsource();
#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
int formatBinaryToInt(int arr[], int arrSize);
#line 11 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void formatIntToHex(int integer, char hex[]);
#line 15 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void formatIntToBin(int value, int data[], int length);
#line 27 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void printFullArray(int arr[], int sizeArr);
#line 37 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void printFullArray(char arr[], int sizeArr);
#line 47 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void printCompactArray(int arr[], int sizeArr);
#line 56 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void printCompactArray(String arr[], int sizeArr);
#line 66 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void fillArrayWithZeroes(int arr[], int size);
#line 74 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
int toPower(int base, int exponent);
#line 84 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
bool isChannelNumberValid(int channel);
#line 95 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
void sos_flasher_test();
#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\Measure.ino"
void selectChannel(int channel, bool status);
#line 26 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\Measure.ino"
void selectMeasRange(MeasRange range);
#line 52 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\Measure.ino"
void selectIchUsrc(bool connect);
#line 74 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\Measure.ino"
double measure(MeasRange range, int pin);
#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\PinController.ino"
void setupPins();
#line 31 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\PinController.ino"
void configDataPins(int io);
#line 53 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\PinController.ino"
void writePins(const int pin[], int pin_size, int inputData);
#line 66 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\PinController.ino"
int readPins(const int pin[], int pin_size);
#line 158 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\RegelbareVoeding.ino"
void setupStatus()
{
  dacData0Status = 0x00;
  dacData1Status = 0x80;
  sourceStatus = 0x00;
  busCon0Status = 0x00;
  busCon1Status = 0x00;
  gndCon0Status = 0x00;
  gndCon1Status = 0x00;
  measureStatus = 0x00;
  rangeStatus = 0x00;
  // The DAC is reset
  writeData(Register::DACDATA0, dacData0Status, boardNumber);
  writeData(Register::DACDATA1, dacData1Status, boardNumber);
  // The SOURCE register is reset
  writeData(Register::SOURCE, sourceStatus, boardNumber);
  // Rhe MEASURE register is reset
  writeData(Register::MEASURE, measureStatus, boardNumber);
  // All relays are switched off
  writeData(Register::BUSCON0, busCon0Status, boardNumber);
  writeData(Register::BUSCON1, busCon1Status, boardNumber);
  writeData(Register::GNDCON0, gndCon0Status, boardNumber);
  writeData(Register::GNDCON1, gndCon1Status, boardNumber);
  // The UI-bus register is reset.
  writeData(Register::RANGE, rangeStatus, boardNumber);
  // Read the errorflags to clear the register
  readData(Register::ERROR_FLAGS, boardNumber);
  // settling time
  delay(RELAY_OFF_SETTLING);
}
void testFullFunctionallity()
{
  digitalWrite(14, HIGH);
  connectToBus(1, true);
  connectVoltageSource(true);
  setVoltage(11);
  Serial.println("***********");
  double current = measureCurrentUsource();
  Serial.println("Measured current = " + String(current));
  current = measureVoltage(1);
  Serial.println("Measured Voltage = " + String(current));
  Serial.println("***********");
  Serial.println();
  delay(5000);
  digitalWrite(14, LOW);
  setVoltage(0);
  Serial.println("***********");
  current = measureCurrentUsource();
  Serial.println("Measured current = " + String(current));
  current = measureVoltage(1);
  Serial.println("Measured Voltage = " + String(current));
  Serial.println("***********");
  Serial.println();
  delay(5000);
}
// ------------------ E N D   T E S T   F U N C T I O N A L I T Y ------------------

// ------------------  D E F I N E   C A L L B A C K S   +   C M D   M E S S E N G E R------------------
// Cmd Messenger setup and config for serial communication
char field_separator = ',';
char command_separator = ';';
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);

enum class CommandCalls
{
  TOGGLE_LED = 1,
  PUT_VOLTAGE = 2,
  CONNECT_TO_GROUND = 3,
  CONNECT_TO_BUS = 4,
  MEASURE_VOLTAGE = 5,
  MEASURE_CURRENT = 6,
  CHANGE_BOARDNUMBER = 7,
  GET_BOARDNUMBER = 8,
};
void attachCommandCallbacks()
{
  cmdMessenger.attach(0, onUnknownCommand);
  cmdMessenger.attach(static_cast<int>(CommandCalls::TOGGLE_LED), toggleLed);
  cmdMessenger.attach(static_cast<int>(CommandCalls::PUT_VOLTAGE), setVoltageSerial);
  cmdMessenger.attach(static_cast<int>(CommandCalls::CONNECT_TO_GROUND), connectToGroundSerial);
  cmdMessenger.attach(static_cast<int>(CommandCalls::CONNECT_TO_BUS), connectToBusSerial);
  cmdMessenger.attach(static_cast<int>(CommandCalls::MEASURE_VOLTAGE), toggleLed);
  cmdMessenger.attach(static_cast<int>(CommandCalls::MEASURE_CURRENT), toggleLed);
  cmdMessenger.attach(static_cast<int>(CommandCalls::CHANGE_BOARDNUMBER), toggleLed);
  cmdMessenger.attach(static_cast<int>(CommandCalls::GET_BOARDNUMBER), toggleLed);
}
// ------------------ E N D   D E F I N E   C A L L B A C K S +   C M D   M E S S E N G E R------------------

// ----------------------------------- C A L L B A C K S  M E T H O D S -------------------------------------
void showPossibleCommands()
{
  Serial.println("Toggle LED");
  Serial.println("Connect to Ground");
  Serial.println("Connect to Bus");
  Serial.println("Put Voltage");
  Serial.println("Change Boardnumber");
  Serial.println("Get Boardnumber");
  Serial.println("Measure Current");
  Serial.println("Measure Voltage");
}
void onUnknownCommand()
{
  toggleLed();
  delay(750);
  toggleLed();
  // showPossibleCommands();
}
void toggleLed()
{
  led = !led;
  if (led)
    digitalWrite(14, HIGH);
  else if (!led)
    digitalWrite(14, LOW);
}
void setVoltageSerial()
{
  sos_flasher_test();
  if (cmdMessenger.available())
  {
    int voltage_int = cmdMessenger.readInt16Arg();
    int voltage_dec = cmdMessenger.readInt32Arg();
    String combined = String(String(voltage_int) + "." + String(voltage_dec));

    double voltage = combined.toDouble();
    setVoltage(voltage);
  }
  else
  {
    // Error....
  }
}
void connectToGroundSerial()
{
  int channel;
  bool connect;
  while (cmdMessenger.available())
  {
    channel = cmdMessenger.readInt16Arg();
    connect = cmdMessenger.readBoolArg();
    connectToGround(channel, connect);
  }
}
void connectToBusSerial()
{
  int channel;
  bool connect;
  while (cmdMessenger.available())
  {
    channel = cmdMessenger.readInt16Arg();
    connect = cmdMessenger.readBoolArg();
    connectToBus(channel, connect);
  }
}
// -------------------------------- E N D  C A L L B A C K  M E T H O D S ----------------------------------

void setup()
{
  Serial.begin(115200);
  setupPins();
  setupStatus();

  attachCommandCallbacks();
  cmdMessenger.printLfCr();

  led = true;
  digitalWrite(14, LOW);
  for (int i = 0; i < 16; i++)
  {
    busChannelStatus[i] = false;
    gndChannelStatus[i] = false;
  }
}

String incomingByte = "";
void loop()
{
  // delay(2000);
  cmdMessenger.feedinSerialData();
  // if (Serial.available() > 0)
  // {
  //   // read the incoming bytes
  //   incomingByte = Serial.read();
  //   // incomingByte = incomingByte.substring(4);

  //   // say what you got:
  //   Serial.print("I received: ");
  //   Serial.println(incomingByte);
  // }
  // // delay(1000);
}

#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\BoardFunctions.ino"
#include <stdlib.h>

MeasRange DEFAULT_BOARD_RANGE = MeasRange::Bi30;
MeasRange boardrange = DEFAULT_BOARD_RANGE;

void writeData(Register chosenReg, int data, int boardNumber)
{
    // Serial.println("WRITE OPERATION");
    // Expect a high ack line
    int ack_value = digitalRead(ACK);
    // ACK initial state needs to be HIGH
    if (ack_value != HIGH)
    {
        Serial.println("ERROR: ACK was already low \n");
    }
    else
    {
        configDataPins(OUTPUT_SELECTOR);
        // Write dataBits to pins
        writePins(datapins, SIZE_DATAPINS, data);
        // Select board using cardAddressPins
        writePins(cardAddresspins, SIZE_CARDPINS, boardNumber);
        // Write register address to pins
        writePins(addresspins, SIZE_ADDRESSPINS, (int)chosenReg);
        // Enable write
        digitalWrite(WR, LOW);
        // Serial.println("Datapins, addresspins and cardAddressPins are configured and written");
        // Serial.println("WriteLine is set LOW");

        for (int i = 1; i <= MAX_ACK_CHECK_RETRIES; i++)
        {
            ack_value = digitalRead(ACK);
            // ACK received
            // Serial.println("ACK value = " + String(ack_value));
            if (ack_value == LOW)
            {
                // Serial.println("ACK received");
                break;
            }
            // ACK expired
            if (i == MAX_ACK_CHECK_RETRIES)
            {
                digitalWrite(WR, HIGH);
                Serial.println("ERROR: ACK expired");
                break;
            }
        }
        digitalWrite(WR, HIGH);
        // Serial.println("Writeline is reset (low)");

        // Check for errors
        int err_value = digitalRead(ERR);
        if (err_value != HIGH)
        {
            Serial.println("ERROR detected");
        }
        // Serial.println("Write finished \n");
    }
}
int readData(Register chosenReg, int boardNumber)
{
    Serial.println("READ OPERATION");
    // Expect a high ack line
    int ack_value = digitalRead(ACK);
    // ACK initial state needs to be HIGH
    if (ack_value != HIGH)
    {
        Serial.println("ERROR: ACK was already low \n");
    }
    else
    {
        configDataPins(INPUT_SELECTOR);
        // Select board using cardAddressPins
        writePins(cardAddresspins, SIZE_CARDPINS, boardNumber);
        // Write register address to pins
        writePins(addresspins, SIZE_ADDRESSPINS, (int)chosenReg);
        // Enable read
        digitalWrite(RD, LOW);
        Serial.println("Datapins are configured, addresspins and cardAddressPins are configured and written");
        Serial.println("Readline is set LOW");

        // loop till ACK goes low
        for (int i = 1; i <= MAX_ACK_CHECK_RETRIES; i++)
        {
            ack_value = digitalRead(ACK);
            // ACK received
            if (ack_value == LOW)
            {
                Serial.println("ACK received");
                break;
            }
            // ACK expired
            if (i == MAX_ACK_CHECK_RETRIES)
            {
                digitalWrite(RD, HIGH);
                Serial.println("ERROR: ACK expired");
                break;
            }
        }
        // Read in data
        int data = readPins(datapins, SIZE_DATAPINS);
        Serial.println("pins are read");
        digitalWrite(RD, HIGH);
        Serial.println("Readline is reset (low)");

        // Check for errors
        int err_value = digitalRead(ERR);
        if (err_value != HIGH)
        {
            Serial.println("ERROR detected");
        }
        Serial.println("Read finished \n");
        return data;
    }
}

void printGNDStatus(int status0_before, int status0_after, int status1_before, int status1_after)
{
    Serial.println("STATUS GND:");
    Serial.print("gndCon0Status before: ");
    int statusGnd[8];
    // Serial.print(status0_before);
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("gndCon0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("gndCon1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("gndCon1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println();
}
void connectToGround(int channel, bool status)
{
    if (status)
        Serial.println("Connect channel" + String(channel) + " to the ground");
    else
        Serial.println("Disconnect channel" + String(channel) + " from the ground");
    // status: true to close (enable relay), false to open
    // Check if no out of range errors
    if (isChannelNumberValid(channel))
    {
        if (status != gndChannelStatus[channel - 1])
        {
            gndChannelStatus[channel - 1] = status;
            // Copy to check for changes
            int gndCon0StatusCopy = gndCon0Status;
            int gndCon1StatusCopy = gndCon1Status;
            // Based if you need to close or open connection - select correct timeout
            int switchTime = status ? RELAY_ON_SETTLING : RELAY_OFF_SETTLING;
            // Binary operations to modify correct bit
            // Select correct channel (e.g. channel 4 -> true? 0000 0011  -> 0000 1011)
            // channel count originally 1-8, we want from 0-7 => -1
            if (channel <= 8)
            {
                gndCon0StatusCopy = (status) ? (gndCon0StatusCopy | (1 << (channel - 1))) : (gndCon0StatusCopy & ~(1 << (channel - 1)));
            }
            else
            {
                gndCon1StatusCopy = (status) ? (gndCon1StatusCopy | (1 << (channel - 9))) : (gndCon1StatusCopy & ~(1 << (channel - 9)));
            }

            // Print GND status changes
            printGNDStatus(gndCon0Status, gndCon0StatusCopy, gndCon1Status, gndCon1StatusCopy);
            // Check if state is changed, and from which register it changed
            if (gndCon0StatusCopy != gndCon0Status)
            {
                // Status update
                gndCon0Status = gndCon0StatusCopy;
                // write information to pins (configure pins correctly)
                writeData(Register::GNDCON0, gndCon0Status, boardNumber);
                delay(switchTime);
            }
            if (gndCon1StatusCopy != gndCon1Status)
            {
                // Status update
                gndCon1Status = gndCon1StatusCopy;
                // write information to pins (configure pins correctly)
                writeData(Register::GNDCON1, gndCon1Status, boardNumber);
                delay(switchTime);
            }
        }
        else
        {
            Serial.println("State is already correct");
        }
    }
}

void printBusStatus(int status0_before, int status0_after, int status1_before, int status1_after)
{
    Serial.println("STATUS BUS:");
    Serial.print("busCon0Status before: ");
    int statusGnd[8];
    // Serial.print(status0_before);
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("busCon0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("busCon1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("busCon1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println();
}
void connectToBus(int channel, bool status)
{
    if (status)
        Serial.println("Connect channel " + String(channel) + " to the BUS");
    else
        Serial.println("Disconnect channel " + String(channel) + " from the BUS");
    // status: true to connect (enable relay), false to disconnect
    // Check if no out of range errors
    if (isChannelNumberValid(channel))
    {
        if (status != busChannelStatus[channel - 1])
        {
            busChannelStatus[channel - 1] = status;
            // Copy to check for changes
            int busCon0StatusCopy = busCon0Status;
            int busCon1StatusCopy = busCon1Status;
            // Based if you need to close or open connection - select correct timeout
            int switchTime = status ? RELAY_ON_SETTLING : RELAY_OFF_SETTLING;
            // Binary operations to modify correct bit
            // Select correct channel (e.g. channel 4 -> true? 0000 0011  -> 0000 1011)
            // channel count originally 1-8, we want from 0-7 => -1
            if (channel <= 8)
            {
                busCon0StatusCopy = (status) ? (busCon0StatusCopy | (1 << (channel - 1))) : (busCon0StatusCopy & ~(1 << (channel - 1)));
            }
            else
            {
                busCon1StatusCopy = (status) ? (busCon1StatusCopy | (1 << (channel - 9))) : (busCon1StatusCopy & ~(1 << (channel - 9)));
            }

            // Print GND status changes
            printBusStatus(busCon0Status, busCon0StatusCopy, busCon1Status, busCon1StatusCopy);
            // Check if state is changed, and from which register it changed
            if (busCon0StatusCopy != busCon0Status)
            {
                // Status update
                busCon0Status = busCon0StatusCopy;
                // write information to pins (configure pins correctly)
                writeData(Register::BUSCON0, busCon0Status, boardNumber);
                delay(switchTime);
            }
            if (busCon1StatusCopy != busCon1Status)
            {
                // Status update
                busCon1Status = busCon1StatusCopy;
                // write information to pins (configure pins correctly)
                writeData(Register::BUSCON1, busCon1Status, boardNumber);
                delay(switchTime);
            }
        }
        else
        {
            Serial.println("State is already correct");
        }
    }
}

void printConnectVoltageStatus(int before, int after)
{
    int data[8];
    Serial.println("STATUS CONNECTING VOLTAGE SOURCE");
    Serial.print("Status voltage source before: ");
    fillArrayWithZeroes(data, 8);
    formatIntToBin(before, data, 8);
    printCompactArray(data, 8);
    Serial.print("Status voltage source after: ");
    fillArrayWithZeroes(data, 8);
    formatIntToBin(after, data, 8);
    printCompactArray(data, 8);
    Serial.println();
}
void connectVoltageSource(bool status)
{
    if (status)
        Serial.println("Connect voltage source");
    else
        Serial.println("Disconnect voltage source");
    int switchTime = status ? RELAY_ON_SETTLING : RELAY_OFF_SETTLING;
    int sourceStatusCopy = sourceStatus;
    if (status)
    {
        sourceStatusCopy &= ~0x40; // set b6 low (~0x40 = 1011 1111)
        sourceStatusCopy |= 0x01;  // Set b1 high
    }
    else
    {
        sourceStatusCopy &= ~0x01; // Set first bit low
    }
    printConnectVoltageStatus(sourceStatus, sourceStatusCopy);
    if (sourceStatusCopy != sourceStatus)
    {
        sourceStatus = sourceStatusCopy;
        writeData(Register::SOURCE, sourceStatus, boardNumber);
        delay(switchTime);
    }
}

void printSetVoltageStatus(int status0_before, int status0_after, int status1_before, int status1_after)
{
    Serial.println("STATUS DATA SETVOLTAGE:");
    Serial.print("data0Status before: ");
    int statusGnd[8];
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("data0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("data1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("data1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println();
}
void setVoltage(double voltage)
{
    Serial.println("Set voltage to " + String(voltage));
    int status0_before = dacData0Status;
    int status1_before = dacData1Status;
    // After the DAC the voltage is multiplied with 3
    voltage /= 3.0;
    // make voltage positive
    voltage += 10.0;
    if (voltage >= 20)
    {
        dacData0Status = 0xFF;
        dacData1Status = 0xFF;
    }
    else if (voltage <= 0)
    {
        dacData0Status = 0x00;
        dacData1Status = 0x00;
    }
    else
    {
        unsigned int rescaledVoltage = (unsigned int)(voltage * ((double)0xFFFF / (double)20));
        // Serial.println("Rescaled Voltage: " + String(rescaledVoltage));
        dacData0Status = (int)(rescaledVoltage & 0xFF);
        dacData1Status = (int)((rescaledVoltage >> 8) & 0xFF);
    }
    printSetVoltageStatus(status0_before, dacData0Status, status1_before, dacData1Status);
    // write data
    writeData(Register::DACDATA0, dacData0Status, boardNumber);
    writeData(Register::DACDATA1, dacData1Status, boardNumber);
}

double measureVoltage(int channel)
{
    // Check if channelNr is in range [0...15]
    if (isChannelNumberValid(channel))
    {
        boardrange = MeasRange::Bi10;
        // Define range, depending the size of the voltage
        selectMeasRange(boardrange);
        // Select channel you want to measure
        selectChannel(channel, true);
        double voltage_measured = measure(boardrange, AD0);
        if (voltage_measured > 4)
        {
            boardrange = MeasRange::Bi30;
            selectMeasRange(boardrange);
            voltage_measured = measure(boardrange, AD0);
            if (voltage_measured > 10)
            {
                boardrange = MeasRange::Bi120;
                selectMeasRange(boardrange);
                voltage_measured = measure(boardrange, AD0);
            }
        }
        // Disconnect Channel
        selectChannel(channel, false);
        boardrange = MeasRange::Bi10;
        selectMeasRange(boardrange);
        return voltage_measured;
    }
    else
        Serial.println("INVALID CHANNEL NUMBER");
}
double measureCurrentUsource()
{
    // connect current channel and select U source.
    selectIchUsrc(true);
    double current_measured = measure(boardrange, AD1);
    // disconnect current channel
    selectIchUsrc(false);
    return current_measured;
}

#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\GlobalFunctions.ino"
int formatBinaryToInt(int arr[], int arrSize)
{
  int ret = 0, weight;
  for (int i = 0; i < arrSize; i++)
  {
    weight = arrSize - i - 1;
    ret = (arr[i] * toPower(2, weight)) + ret;
  }
  return ret;
}
void formatIntToHex(int integer, char hex[])
{
  sprintf(hex, "%x", integer);
}
void formatIntToBin(int value, int data[], int length)
{
  int i = length - 1;
  while (value > 0)
  {
    // Storing remainder in binary array
    data[i] = value % 2;
    value = value / 2;
    i--;
  }
}

void printFullArray(int arr[], int sizeArr)
{
  Serial.print("Size: " + String(sizeArr) + "\n");
  Serial.print("Array : {");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(String(arr[i]));
  }
  Serial.print("} \n\n");
}
void printFullArray(char arr[], int sizeArr)
{
  Serial.print("Size: " + String(sizeArr) + "\n");
  Serial.print("Array : {");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(arr[i]);
  }
  Serial.print("} \n\n");
}
void printCompactArray(int arr[], int sizeArr)
{
  Serial.print("{");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(arr[i]);
  }
  Serial.println("}");
}
void printCompactArray(String arr[], int sizeArr)
{
  Serial.print("{");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(arr[i]);
  }
  Serial.println("}");
}

void fillArrayWithZeroes(int arr[], int size)
{
  for (int i = 0; i < size; i++)
  {
    arr[i] = 0;
  }
}

int toPower(int base, int exponent)
{
  int ret = 1;
  for (int i = 0; i < exponent; i++)
  {
    ret = ret * base;
  }
  return ret;
}

bool isChannelNumberValid(int channel)
{
  if ((channel > AIO_CHANNELS) || (channel < 1))
  {
    Serial.println("channel out of range [1...16] \n");
    return false;
  }
  else
    return true;
}

void sos_flasher_test()
{
  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);
  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);
  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);

  digitalWrite(14, HIGH);
  delay(500);
  digitalWrite(14, LOW);
  delay(500);
  digitalWrite(14, HIGH);
  delay(500);
  digitalWrite(14, LOW);
  delay(500);
  digitalWrite(14, HIGH);
  delay(500);
  digitalWrite(14, LOW);
  delay(500);

  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);
  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);
  digitalWrite(14, HIGH);
  delay(200);
  digitalWrite(14, LOW);
  delay(200);
}

#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\Measure.ino"
void selectChannel(int channel, bool status)
{
    if (status)
    {
        if (channel <= 8)
        {
            // voltage is connected with the AD input
            measureStatus = 0x28 | (channel - 1);
        }
        else
        {
            // voltage is connected with the AD input
            measureStatus = 0x30 | (channel - 9);
        }
    }
    else
    {
        measureStatus &= 0x80;
    }
    writeData(Register::MEASURE, measureStatus, boardNumber);
    if (status)
    {
        delay(RELAY_ON_SETTLING);
    }
}
void selectMeasRange(MeasRange range)
{
    // Range lowers voltage send to measure channel depending on the range
    // e.g. voltage = 15, Bi30 -> voltage to measuring channel = 5
    int rangeStatusCopy = rangeStatus;
    switch (range)
    {
    case MeasRange::Bi10:
        rangeStatusCopy = (rangeStatusCopy & 0x3F) | 0x40;
        break;
    case MeasRange::Bi120:
        rangeStatusCopy = (rangeStatusCopy & 0x3F) | 0x80;
        break;
    default:
        rangeStatusCopy &= 0x3F;
        break;
    }
    if (rangeStatus != rangeStatusCopy)
    {
        rangeStatus = rangeStatusCopy;
        writeData(Register::RANGE, rangeStatusCopy, boardNumber);
        // settling time for the measure path.
        delay(RELAY_ON_SETTLING);
    }
}
// Connect current to the measurement channel (AD1)
void selectIchUsrc(bool connect)
{
    int measureStatusCopy = measureStatus;
    // Enables current measurement
    // Toggles b6, and only b6!
    measureStatusCopy = (connect) ? (measureStatusCopy | 0x40) : (measureStatusCopy & ~0x40);
    // b7 U(0) or I(1) source current
    // Controls RE40 -> To choose measurement from voltage_source or current_source
    measureStatusCopy &= ~0x80;
    if (measureStatus != measureStatusCopy)
    {
        measureStatus = measureStatusCopy;
        int data[8];
        Serial.print("Measure data: ");
        fillArrayWithZeroes(data, 8);
        formatIntToBin(measureStatus, data, 8);
        printCompactArray(data, 8);
        writeData(Register::MEASURE, measureStatus, boardNumber);
        delay(RELAY_ON_SETTLING);
    }
}

double measure(MeasRange range, int pin)
{
    double measuredValue = analogRead(pin);
    // Convert from analog value to correct
    // 10 bits -> 1023 values
    // Default 5, needs to be multiplied with ranges
    double measured = ((double)(range)*5 / (double)1023) * measuredValue;
    return measured;
}

#line 1 "c:\\Users\\wdl\\OneDrive - Picanol Group\\Documents\\PsiControl_RegelbareVoeding_V3\\RegelbareVoeding\\PinController.ino"
void setupPins()
{
  //  datapins are controlled by the arduino
  for (int i = 0; i < 8; i++)
  {
    pinMode(addresspins[i], OUTPUT);
  }

  // Decision for R/W/RESET controlled by the arduino
  pinMode(WR, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(WR, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(RESET, HIGH);

  pinMode(ACK, INPUT_PULLUP);
  pinMode(ERR, INPUT_PULLUP);
  pinMode(AD0, INPUT);
  pinMode(AD1, INPUT);

  // Address of the card, can be used to validate card
  for (int i = 0; i < 4; i++)
  {
    pinMode(cardAddresspins[i], OUTPUT);
  }

  // Datapins are bidirectional, need to be changed at runtime
}

void configDataPins(int io)
{
  switch (io)
  {
  case INPUT_SELECTOR:
    for (int i = 0; i < 8; i++)
    {
      pinMode(datapins[i], INPUT);
    }
    break;
  case OUTPUT_SELECTOR:
    for (int i = 0; i < 8; i++)
    {
      pinMode(datapins[i], OUTPUT);
    }
    break;
  default:
    Serial.print("Error: fault in config");
    break;
  }
}

void writePins(const int pin[], int pin_size, int inputData)
{
  int data[pin_size];
  fillArrayWithZeroes(data, pin_size);
  formatIntToBin(inputData, data, pin_size);
  int j = pin_size - 1;
  for (int i = 0; i < pin_size; i++)
  {
    digitalWrite(pin[i], data[j]);
    j--;
  }
}

int readPins(const int pin[], int pin_size)
{
  int data[pin_size];
  int j = pin_size - 1;
  // For loop moet nog worden gecontrolleerd...
  for (int i = 0; i < pin_size; i++)
  {
    // MSB of data = data[0]
    // MSB of pins = pin[pin_size-1]
    data[i] = digitalRead(pin[j]);
    j--;
  }
  int ret_data = formatBinaryToInt(data, pin_size);
  return ret_data;
}

