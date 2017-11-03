/**************************************************************************/
/*!
    @file     cie_Nfc_Mock.cpp
    @author   Developers Italia
    @license  BSD (see License)

	
	A mock implementation that's used for unit testing the cie_PN532 library

	@section  HISTORY

	v1.0  - Allows the setup of expected commands and baked responses
*/
/**************************************************************************/
#include "cie_Nfc_Mock.h"


/**************************************************************************/
/*!
  @brief  Does nothing
*/
/**************************************************************************/
void cie_Nfc_Mock::begin() {
  //Do nothing, we do not need to initialize anything in this mock class
}

/**************************************************************************/
/*!
  @brief  Simulates a card is present at the terminal

  @returns  Always true
*/
/**************************************************************************/
bool cie_Nfc_Mock::detectCard() {
  //Always return true, simulate a card is present
  return true;
}


/**************************************************************************/
/*!
  @brief  Checks whether the command was expected and provides a baked response
  
  @param  command A pointer to the APDU command bytes
  @param  commandLength Length of the command
  @param  response A pointer to the buffer which will contain the response bytes
  @param  responseLength The length of the desired response

  @returns  A boolean value indicating whether the operation succeeded or not
*/
/**************************************************************************/
bool cie_Nfc_Mock::sendCommand(byte* command, byte commandLength, byte* response, byte* responseLength) {

  _attemptedCommandsCount += 1;
  if (_attemptedCommandsCount > _expectedCommandsCount) {
    Serial.println("cie_PN532 tried to execute more commands than expected");
    return false;
  }
  
  bool isSameCommand = areEqual(command, _expectedCommands[_executedCommandsCount].command, _expectedCommands[_executedCommandsCount].commandOffset, _expectedCommands[_executedCommandsCount].commandLength);
  if (isSameCommand) {
    memcpy(response, _expectedCommands[_executedCommandsCount].response, *responseLength);
    _executedCommandsCount += 1;
    return true;
  } else {
    return false;
  }
}


/**************************************************************************/
/*!
  @brief Sets the number of commands to expect from the NFC library
*/
/**************************************************************************/
void cie_Nfc_Mock::expectCommands(const byte count) {
  clear();
  _executedCommandsCount = 0;
  _attemptedCommandsCount = 0;
  _expectedCommandsCount = 0;
  _expectedCommands = new cie_Command[_expectedCommandsCount];
}


/**************************************************************************/
/*!
  @brief Sets up a command that's supposed to be sent by the NFC library
  
  @param command A pointer to the expected command (or part of it)
  @param commandOffset The starting offset of the provided command
  @param commandLength The length of the provided command
  @param response A pointer to the baked response that should be returned for this command
  @param responseLength The length of the response

*/
/**************************************************************************/
void cie_Nfc_Mock::expectCommand(byte* command, const byte commandOffset, const byte commandLength, byte* response, const byte responseLength) {
  _expectedCommands[_expectedCommandsCount].command = command;
  _expectedCommands[_expectedCommandsCount].commandLength = commandLength;
  _expectedCommands[_expectedCommandsCount].commandOffset = commandOffset;
  _expectedCommands[_expectedCommandsCount].response = response;
  _expectedCommands[_expectedCommandsCount].responseLength = responseLength;
  _expectedCommandsCount += 1;
}


/**************************************************************************/
/*!
  @brief Checks whether all expected commands were correctly sent by the NFC library
  
  @returns  A boolean value indicating whether the commands were all sent or not
*/
/**************************************************************************/
bool cie_Nfc_Mock::allExpectedCommandsExecuted() {
  return _expectedCommandsCount == _executedCommandsCount && _expectedCommandsCount == _attemptedCommandsCount;
}


/**************************************************************************/
/*!
  @brief Checks two buffers for equality, byte by byte
  
  @param originalBuffer A pointer to the first buffer
  @param comparedBuffer A pointer to the second buffer
  @param offset The starting offset in the original buffer
  @param length The number of bytes to compare

  @returns  A boolean value indicating whether the two buffers were equal or not
*/
/**************************************************************************/
bool cie_Nfc_Mock::areEqual(byte* originalBuffer, byte* comparedBuffer, const byte offset, const byte length) {
  for (byte i = 0; i < length; i++) {
    if (originalBuffer[i+offset] != comparedBuffer[i]) {
      Serial.print(F("Command at index "));
      Serial.print(_executedCommandsCount);
      Serial.print(F(" was not expected (byte "));
      Serial.print(i);
      Serial.print(F(" was different: expected "));
      printHex(comparedBuffer[i], 1);
      Serial.print(F(" but received "));
      printHex(originalBuffer[i+offset], 1);
      Serial.println(F(")"));
      return false;
    }
  }
  return true;
}


/**************************************************************************/
/*!
  @brief  Frees resources
*/
/**************************************************************************/
void cie_Nfc_Mock::clear() {
  for (byte i = 0; i < _expectedCommandsCount; i++) {
    delete [] _expectedCommands[i].command;
    delete [] _expectedCommands[i].response;
  }
  delete [] _expectedCommands;
}


/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters

    @param  data      Pointer to the byte data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void cie_Nfc_Mock::printHex(const byte * data, const byte numBytes)
{
  uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++)
  {
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      Serial.print(F("0"));
    Serial.print(data[szPos]&0xff, HEX);
    if ((numBytes > 1) && (szPos != numBytes - 1))
    {
      Serial.print(F(" "));
    }
  }
}


/**************************************************************************/
/*!
  @brief  Frees resources
*/
/**************************************************************************/
cie_Nfc_Mock::~cie_Nfc_Mock() {
  clear();
}