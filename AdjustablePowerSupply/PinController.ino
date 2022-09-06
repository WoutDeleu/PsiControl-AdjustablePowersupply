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
