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

void printCompactArray(int arr[], int sizeArr)
{
  Serial.print("{");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(arr[i]);
  }
  Serial.print("} ");
}
void printCompactArray(String arr[], int sizeArr)
{
  Serial.print("{");
  for (int i = 0; i < sizeArr; i++)
  {
    Serial.print(arr[i]);
  }
  Serial.print("} ");
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

void flasher_display_number(int number)
{
  for (int i = 0; i < number; i++)
  {
    delay(500);
    toggleLed();
    delay(500);
    toggleLed();
  }
}

void toggleLed()
{
  led_status = !led_status;
  if (led_status)
    digitalWrite(led, HIGH);
  else if (!led_status)
    digitalWrite(led, LOW);
}