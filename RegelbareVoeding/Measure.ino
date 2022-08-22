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
