void resetChirp() {
  writeI2CRegister8bit(0x20, 6); //reset  
}

unsigned int readCapacitance(){
  return readI2CRegister16bit(0x20, 0);
}

unsigned int readChirpTemperature() {
  return readI2CRegister16bit(0x20, 5);
}

unsigned int readLight(){
   writeI2CRegister8bit(0x20, 3);
   return readI2CRegister16bit(0x20, 4);
}

void writeI2CRegister8bit(int addr, int value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

unsigned int readI2CRegister16bit(int addr, int reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  delay(20);
  Wire.requestFrom(addr, 2);
  unsigned int t = Wire.read() << 8;
  t = t | Wire.read();
  return t;
}
