#include <stdlib.h>

MeasRange DEFAULT_BOARD_RANGE = MeasRange::Bi30;
MeasRange boardrange = DEFAULT_BOARD_RANGE;

void writeData(Register chosenReg, int data, int boardNumber)
{
    // Expect a high ack line
    int ack_value = digitalRead(ACK);
    // ACK initial state needs to be HIGH
    if (ack_value != HIGH)
    {
        Serial.println("||ERROR: ACK was already low||");
    }
    else
    {
        // To write data, the datapins need to be configurated as output
        configDataPins(OUTPUT_SELECTOR);
        // Write dataBits to pins
        writePins(datapins, SIZE_DATAPINS, data);
        // Select board using cardAddressPins
        writePins(cardAddresspins, SIZE_CARDPINS, boardNumber);
        // Write register address to pins
        writePins(addresspins, SIZE_ADDRESSPINS, (int)chosenReg);
        // Enable write
        digitalWrite(WR, LOW);

        for (int i = 1; i <= MAX_ACK_CHECK_RETRIES; i++)
        {
            ack_value = digitalRead(ACK);
            // ACK received
            if (ack_value == LOW)
            {
                // Ack received
                break;
            }
            // ACK expired
            if (i == MAX_ACK_CHECK_RETRIES)
            {
                digitalWrite(WR, HIGH);
                Serial.println("||ERROR: ACK expired||");
                break;
            }
        }
        digitalWrite(WR, HIGH);

        // Check for errors
        int err_value = digitalRead(ERR);
        if (err_value != HIGH)
        {
            Serial.println("||ERROR detected||");
        }
    }
}
int readData(Register chosenReg, int boardNumber)
{
    // Expect a high ack line
    int ack_value = digitalRead(ACK);
    // ACK initial state needs to be HIGH
    if (ack_value != HIGH)
    {
        Serial.println("||ERROR: ACK was already low ||");
    }
    else
    {
        // To read data, the datapins need to be configurated as input
        configDataPins(INPUT_SELECTOR);
        // Select board using cardAddressPins
        writePins(cardAddresspins, SIZE_CARDPINS, boardNumber);
        // Write register address to pins
        writePins(addresspins, SIZE_ADDRESSPINS, (int)chosenReg);
        // Enable read
        digitalWrite(RD, LOW);

        // loop till ACK goes low
        for (int i = 1; i <= MAX_ACK_CHECK_RETRIES; i++)
        {
            ack_value = digitalRead(ACK);
            // ACK received
            if (ack_value == LOW)
            {
                break;
            }
            // ACK expired
            if (i == MAX_ACK_CHECK_RETRIES)
            {
                digitalWrite(RD, HIGH);
                Serial.println("||ERROR: ACK expired ||");
                break;
            }
        }
        // Read in data
        int data = readPins(datapins, SIZE_DATAPINS);
        digitalWrite(RD, HIGH);
        // Check for errors
        int err_value = digitalRead(ERR);
        if (err_value != HIGH)
        {
            Serial.println("||ERROR detected||");
        }
        return data;
    }
}

void printGNDStatus(int status0_before, int status0_after, int status1_before, int status1_after)
{
    Serial.print("((STATUS GND UPDATE:: ");
    Serial.print("_ gndCon0Status before: ");
    int statusGnd[8];
    // Serial.print(status0_before);
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> gndCon0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("_ gndCon1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> gndCon1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println("))");
}
void connectToGround(int channel, bool status)
{
    // status: true to close (enable relay), false to open
    // Check if no out of range errors
    if (isChannelNumberValid(channel))
    {
        if (status != gndChannelStatus[channel - 1])
        {
            if (status)
                Serial.println("##Connect channel " + String(channel) + " to the ground ##");
            else
                Serial.println("##Disconnect channel " + String(channel) + " from the ground ##");
            gndChannelStatus[channel - 1] = status;
            // Copy to check for chanbges
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
    }
    else
    {
        Serial.println("||ERROR: Invalid channel selection. Fault in communication where wrong format/number is chosen as channel number..." + String(channel) + "is not a valid channel number... It most be in range of  [1...16] ||");
    }
}

void printBusStatus(int status0_before, int status0_after, int status1_before, int status1_after)
{
    Serial.print("((STATUS BUS:: ");
    Serial.print("_ busCon0Status before: ");
    int statusGnd[8];
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> busCon0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("_ busCon1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> busCon1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println("))");
}
void connectToBus(int channel, bool status)
{
    // status: true to connect (enable relay), false to disconnect
    // Check if no out of range errors
    if (isChannelNumberValid(channel))
    {
        if (status != busChannelStatus[channel - 1])
        {
            if (status)
                Serial.println("##Connect channel " + String(channel) + " to the BUS##");
            else
                Serial.println("##Disconnect channel " + String(channel) + " from BUS##");
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
    }
}

void printConnectVoltageStatus(int before, int after)
{
    int data[8];
    Serial.print("((STATUS CONNECTING VOLTAGE SOURCE:: ");
    Serial.print("_ Status voltage source before: ");
    fillArrayWithZeroes(data, 8);
    formatIntToBin(before, data, 8);
    printCompactArray(data, 8);
    Serial.print("=> Status voltage source after: ");
    fillArrayWithZeroes(data, 8);
    formatIntToBin(after, data, 8);
    printCompactArray(data, 8);
    Serial.println("))");
}
void connectVoltageSource(bool status)
{
    if (status)
        Serial.println("##Connect Voltage source##");
    else
    {
        currentVoltage = 0;
        Serial.println("##Disconnect Voltage source##");
    }
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
    Serial.print("((STATUS DATA SETVOLTAGE:: ");
    Serial.print("_ data0Status before: ");
    int statusGnd[8];
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> data0Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status0_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("_ data1Status before: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_before, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.print("=> data1Status after: ");
    fillArrayWithZeroes(statusGnd, 8);
    formatIntToBin(status1_after, statusGnd, 8);
    printCompactArray(statusGnd, 8);
    Serial.println("))");
}
void setVoltage(float voltage)
{
    Serial.println("##Set Voltage to " + String(voltage) + "##");
    currentVoltage = voltage;
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
        Serial.println("||ERROR: INVALID CHANNEL NUMBER||");
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

// Store 'permanent' variables
void permanentWrite(bool permanent)
{
    byte eeprom_value;
    if (permanent)
        eeprom_value = 1;
    else
        eeprom_value = 0;
    
    EEPROM.update(REGISTER_PERMANENT, eeprom_value);
    if (permanent)
    {
        EEPROM.update(REGISTER_BOARDNR, boardNumber);
        EEPROM.update(REGISTER_VOLTAGE, currentVoltage);
    }
}