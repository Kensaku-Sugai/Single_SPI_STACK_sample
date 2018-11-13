#include "SPI_STACK.h"

SPI_STACK::SPI_STACK(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName hold)
    :
    _spi(mosi, miso, sclk),
    _cs(cs),
    _hold(hold)
{
    SPI_STACK_Begin();
}

SPI_STACK::~SPI_STACK() {
}

void SPI_STACK::SPI_STACK_Begin() {
    _spi.format(8, 0);
    _spi.frequency(20000000);  //SPI Clock = 20MHz
    SPI_STACK_Deselect();
}

void SPI_STACK::SPI_STACK_Select() {
    _cs = 0;
}

void SPI_STACK::SPI_STACK_Deselect() {
    _cs = 1;
}

void SPI_STACK::SPI_STACK_Hold() {
    _hold = 0;
}

void SPI_STACK::SPI_STACK_Unhold() {
    _hold = 1;
}

void SPI_STACK::SPI_STACK_Die_Select_NOR() {
    SPI_STACK_Select();
    _spi.write(CMD_DIE_SELECT);
    _spi.write(0x00);
    SPI_STACK_Deselect();
}

void SPI_STACK::SPI_STACK_Die_Select_NAND() {
    SPI_STACK_Select();
    _spi.write(CMD_DIE_SELECT);
    _spi.write(0x01);
    SPI_STACK_Deselect();
}

void SPI_STACK::SPI_STACK_WriteEnable() {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_WRITE_ENABLE);
    SPI_STACK_Deselect();
}

void SPI_STACK::SPI_STACK_WriteDisable() {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_WRITE_DISABLE);
    SPI_STACK_Deselect();
}

uint8_t SPI_STACK::SPI_NOR_ReadStatusReg1() {
    uint8_t rc;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG1);
    rc = _spi.write(0xFF);
    SPI_STACK_Deselect();
    return rc;
}

void SPI_STACK::SPI_NOR_WriteStatusReg1(uint8_t rc) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_WRITE_STATUS_REG1);
    _spi.write(rc);
    SPI_STACK_Deselect();
}

bool SPI_STACK::SPI_NOR_IsBusy() {
    uint8_t r1;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG1);
    r1 = _spi.write(0xFF);
    SPI_STACK_Deselect();
    if(r1 & EWIP_MASK)
        return true;
    return false;
}

void SPI_STACK::SPI_NOR_ReadID(uint8_t* d) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_JEDEC_ID);
    for (uint8_t i=0; i<3; i++) {
        d[i] = _spi.write(0x00);
    }
    SPI_STACK_Deselect();
}

uint16_t SPI_STACK::SPI_NOR_Read(uint32_t addr, uint8_t *buf) { 
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ);
    _spi.write(addr>>16);       // A23-A16
    _spi.write(addr>>8);        // A15-A08
    _spi.write(addr & 0xFF);    // A07-A00
    uint16_t i;
    for(i=0; i<PAGE_SIZE_SPI_NOR; i++) {
        buf[i] = _spi.write(0x00);
    }
    SPI_STACK_Deselect();
    wait(0.1);
    return i;
}

void SPI_STACK::SPI_NOR_Block_Erase(uint32_t addr) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_BLOCK_ERASE);
    _spi.write(addr>>16);       // A23-A16
    _spi.write(addr>>8);        // A15-A08
    _spi.write(addr & 0xFF);    // A07-A00
    SPI_STACK_Deselect();
    wait(2);
}

uint16_t SPI_STACK::SPI_NOR_Page_Program(uint32_t addr, uint8_t *buf) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_PAGE_PROGRAM);
    _spi.write(addr>>16);       // A23-A16
    _spi.write(addr>>8);        // A15-A08
    _spi.write(addr & 0xFF);    // A07-A00
    uint16_t i;
    for(i=0; i<PAGE_SIZE_SPI_NOR; i++) {
        _spi.write(buf[i]);
    }
    SPI_STACK_Deselect();
    wait(0.1);
    return i;
}

uint8_t SPI_STACK::SPI_NAND_ReadStatusReg1() {
    uint8_t rc;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_1);
    rc = _spi.write(0xFF);
    SPI_STACK_Deselect();
    return rc;
}

uint8_t SPI_STACK::SPI_NAND_ReadStatusReg2() {
    uint8_t rc;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_2);
    rc = _spi.write(0xFF);
    SPI_STACK_Deselect();
    return rc;
}

uint8_t SPI_STACK::SPI_NAND_ReadStatusReg3() {
    uint8_t rc;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_3);
    rc = _spi.write(0xFF);
    SPI_STACK_Deselect();
    return rc;
}

void SPI_STACK::SPI_NAND_WriteStatusReg1(uint8_t rc) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_WRITE_STATUS_REG);
    _spi.write(ADR_STATUS_REG_1);
    _spi.write(rc);
    SPI_STACK_Deselect();
}

void SPI_STACK::SPI_NAND_WriteStatusReg2(uint8_t rc) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_WRITE_STATUS_REG);
    _spi.write(ADR_STATUS_REG_2);
    _spi.write(rc);
    SPI_STACK_Deselect();
}

bool SPI_STACK::SPI_NAND_IsBusy() {
    uint8_t r1;
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_3);
    r1 = _spi.write(0xFF);
    SPI_STACK_Deselect();
    if(r1 & SR3_BUSY_MASK)
        return true;
    return false;
}

void SPI_STACK::SPI_NAND_ReadID(uint8_t* d) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_JEDEC_ID);
    _spi.write(0x00);
    for (uint8_t i=0; i<3; i++) {
        d[i] = _spi.write(0x00);
    }
    SPI_STACK_Deselect();
}

bool SPI_STACK::SPI_NAND_PageDataRead(uint16_t addr) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_PAGE_DATA_READ);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_STACK_Deselect();
    wait(0.1);
    return true;
}

uint16_t SPI_STACK::SPI_NAND_ReadData(uint16_t addr, uint8_t *buf) { 
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_DATA);
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    _spi.write(0x00);           // Dummy
    uint16_t i;
    for(i=0; i<(PAGE_SIZE_SPI_NAND); i++) {
        buf[i] = _spi.write(0x00);
    }
    SPI_STACK_Deselect();
    wait(0.1);
    return i;
}

void SPI_STACK::SPI_NAND_ReadData2(uint16_t addr, uint8_t *buf) { 
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_READ_DATA);
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    _spi.write(0x00);           // Dummy
    buf[0] = _spi.write(0x00);
    SPI_STACK_Deselect();
    wait(0.1);
}

void SPI_STACK::SPI_NAND_BlockErase(uint16_t addr) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_BLOCK_ERASE);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_STACK_Deselect();
    wait(2);
}

uint16_t SPI_STACK::SPI_NAND_ProgramDataLoad(uint16_t addr, uint8_t *buf) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_PGM_DATA_LOAD);
    _spi.write(addr>>8);        // CA15-PA08
    _spi.write(addr & 0xFF);    // CA07-PA00
    uint16_t i;
    for(i=0; i<(PAGE_SIZE_SPI_NAND); i++) {
        _spi.write(buf[i]);
    }
    SPI_STACK_Deselect();
    wait(0.1);
    return i;
}

bool SPI_STACK::SPI_NAND_ProgramExecute(uint16_t addr) {
    SPI_STACK_Select();
    SPI_STACK_Unhold();
    _spi.write(CMD_PGM_EXECUTE);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_STACK_Deselect();
    wait(1);
    return true;
}
