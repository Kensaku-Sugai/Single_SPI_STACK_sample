#include <mbed.h>

#define CMD_DIE_SELECT            0xC2
#define PAGE_SIZE_SPI_NOR         256
#define CMD_READ_STATUS_REG1      0x05
#define CMD_WRITE_STATUS_REG1     0x01
#define CMD_WRITE_ENABLE          0x06
#define CMD_WRITE_DISABLE         0x04
#define CMD_READ                  0x03
#define CMD_PAGE_PROGRAM          0x02
#define CMD_BLOCK_ERASE           0xD8
#define CMD_READ_JEDEC_ID         0x9F
#define EWIP_MASK                 0x01

#define PAGE_SIZE_SPI_NAND        2048
#define SPARE_SIZE_SPI_NAND       64
#define MAX_PAGES_SPI_NAND        65536
#define MAX_BLOCKS_SPI_NAND       1024
#define CMD_DEVICE_RESET          0xFF
#define CMD_READ_STATUS_REG       0x05
#define CMD_WRITE_STATUS_REG      0x01
#define CMD_BB_MANAGEMENT         0xA1  // not implemented yet
#define CMD_READ_BBM_LUT          0xA5  // not implemented yet
#define CMD_LAST_ECC_FPA          0xA9  // not implemented yet
#define CMD_PGM_DATA_LOAD         0x02
#define CMD_RPGM_DATA_LOAD        0x84  // not implemented yet
#define CMD_QUAD_DATA_LOAD        0x32  // not implemented yet
#define CMD_RQUAD_DATA_LOAD       0x34  // not implemented yet
#define CMD_PGM_EXECUTE           0x10
#define CMD_PAGE_DATA_READ        0x13
#define CMD_READ_DATA             0x03
#define CMD_FAST_READ             0x0B
#define CMD_FAST_READ_4B          0x0C  // not implemented yet
#define CMD_FAST_READ_DUAL        0x3B  // not implemented yet
#define CMD_FAST_READ_DUAL_4B     0x3C  // not implemented yet
#define CMD_FAST_READ_QUAD        0x6B  // not implemented yet
#define CMD_FAST_READ_QUAD_4B     0x6C  // not implemented yet
#define CMD_FAST_READ_DUAL_IO     0xBB  // not implemented yet
#define CMD_FAST_READ_DUAL_IO_4B  0xBC  // not implemented yet
#define CMD_FAST_READ_QUAD_IO     0xEB  // not implemented yet
#define CMD_FAST_READ_QUAD_IO_4B  0xEC  // not implemented yet
#define ADR_STATUS_REG_1          0xAF
#define ADR_STATUS_REG_2          0xBF
#define ADR_STATUS_REG_3          0xCF
#define SR3_BUSY_MASK             0x01

class SPI_STACK
{
public:
    // Create an SPI_STACK instance
    SPI_STACK(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName hold);

    // Destructor of SPI_STACK
    virtual ~SPI_STACK();

    void SPI_STACK_Begin();
    void SPI_STACK_Select();
    void SPI_STACK_Deselect();
    void SPI_STACK_Hold();
    void SPI_STACK_Unhold();
    void SPI_STACK_Die_Select_NOR();
    void SPI_STACK_Die_Select_NAND();
    void SPI_STACK_WriteEnable();
    void SPI_STACK_WriteDisable();

    uint8_t SPI_NOR_ReadStatusReg1();
    void SPI_NOR_WriteStatusReg1(uint8_t rc);
    bool SPI_NOR_IsBusy();
    void SPI_NOR_ReadID(uint8_t* d);
    uint16_t SPI_NOR_Read(uint32_t addr, uint8_t *buf);
    void SPI_NOR_Block_Erase(uint32_t addr);
    uint16_t SPI_NOR_Page_Program(uint32_t addr, uint8_t *buf);

    uint8_t SPI_NAND_ReadStatusReg1();
    uint8_t SPI_NAND_ReadStatusReg2();
    uint8_t SPI_NAND_ReadStatusReg3();
    void SPI_NAND_WriteStatusReg1(uint8_t rc);
    void SPI_NAND_WriteStatusReg2(uint8_t rc);
    bool SPI_NAND_IsBusy();
    void SPI_NAND_ReadID(uint8_t* d);
    bool SPI_NAND_PageDataRead(uint16_t addr);
    uint16_t SPI_NAND_ReadData(uint16_t addr, uint8_t *buf);
    void SPI_NAND_ReadData2(uint16_t addr, uint8_t *buf);
    void SPI_NAND_BlockErase(uint16_t addr);
    uint16_t SPI_NAND_ProgramDataLoad(uint16_t addr, uint8_t *buf);
    bool SPI_NAND_ProgramExecute(uint16_t addr);

private:
    SPI _spi;
    DigitalOut _cs;
    DigitalOut _hold;
};
