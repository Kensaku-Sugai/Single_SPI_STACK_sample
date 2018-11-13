#include "mbed.h"
#include "SPI_STACK.h"

Serial pc(USBTX, USBRX);
SPI_STACK spistack(D11, D12, D13, D10, D8);

void dump(uint8_t *dt, uint8_t active_die) {
    uint16_t ca  = 0;
    uint32_t end = 0;

    if(active_die==0) {
        end = 16;
    } else if(active_die==1) {
        end = (PAGE_SIZE_SPI_NAND)/16;
    }

    for(uint16_t i=0; i<end; i++) {
        pc.printf("%04X: ", ca);
        for(uint16_t j=0; j<16; j++) {
            pc.printf("%02X ", dt[i*16+j]);
        }
        pc.printf("\n");
        ca = ca + 16;
    }
}

//Block erase
void block_erase(uint32_t addr, uint8_t active_die) {
    pc.printf("==================Block Erase Start==================\n");

    if(active_die==0) {
        spistack.SPI_STACK_WriteEnable();
        spistack.SPI_NOR_Block_Erase(addr);
    } else if(active_die==1) {
        spistack.SPI_STACK_WriteEnable();
        spistack.SPI_NAND_BlockErase(addr);
    }

    pc.printf("Block Erase (D8h) : done\n");
}

// Read data
void read_data(uint32_t addr, uint8_t active_die) {
    uint8_t rdata_nor[PAGE_SIZE_SPI_NOR]={};    // Read data buffer
    uint8_t rdata_nand[PAGE_SIZE_SPI_NAND]={};  // Read data buffer
    uint16_t num;                            // Number of read data

    pc.printf("=====================Read Start======================\n");

    if(active_die==0) {
        num = spistack.SPI_NOR_Read(addr, rdata_nor);
        pc.printf("Read (03h) : num = %d\n", num);
        dump(rdata_nor, active_die);
    } else if(active_die==1) {
        num = spistack.SPI_NAND_PageDataRead(addr);
        if(num) {
            pc.printf("Page Data Read (13h) : done\n");
        }
        num = spistack.SPI_NAND_ReadData(0x00, rdata_nand);
        pc.printf("Read (03h) : num = %d\n", num);
        dump(rdata_nand, active_die);
    }
}

// Program data
void program_data(uint32_t addr, uint8_t mode, uint8_t active_die) {
    uint8_t pdata_nor[PAGE_SIZE_SPI_NOR]={};    // Program data buffer
    uint8_t pdata_nand[PAGE_SIZE_SPI_NAND]={};  // Program data buffer
    uint16_t num;                               // Number of read data

    pc.printf("====================Program Start======================\n");

    if(active_die==0) {
        if(mode==0) {    // Increment program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NOR; i++) {
                pdata_nor[i] = pdata_nor[i] + i;
            }
        } else if(mode==1) {    // AAh 55h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NOR; i++) {
                if(i%2==0) {
                    pdata_nor[i] = 0xAA;
                } else {
                    pdata_nor[i] = 0x55;
                }
            }
        } else if(mode==2) {    // FFh 00h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NOR; i++) {
                if(i%2==0) {
                    pdata_nor[i] = 0xFF;
                } else {
                    pdata_nor[i] = 0x00;
                }
            }
        } else if(mode==3) {    // 00h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NOR; i++) {
                pdata_nor[i] = 0x00;
            }
        }
        spistack.SPI_STACK_WriteEnable();
        num = spistack.SPI_NOR_Page_Program(addr, pdata_nor);
        if(num) {
            pc.printf("Page Program (02h) : num = %d\n", num);
        }
    } else if(active_die==1) {
            if(mode==0) {    // Increment program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NAND; i++) {
                pdata_nand[i] = pdata_nand[i] + i;
            }
        } else if(mode==1) {    // AAh 55h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NAND; i++) {
                if(i%2==0) {
                    pdata_nand[i] = 0xAA;
                } else {
                    pdata_nand[i] = 0x55;
                }
            }
        } else if(mode==2) {    // FFh 00h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NAND; i++) {
                if(i%2==0) {
                    pdata_nand[i] = 0xFF;
                } else {
                    pdata_nand[i] = 0x00;
                }
            }
        } else if(mode==3) {    // 00h program mode
            for(uint16_t i=0; i<PAGE_SIZE_SPI_NAND; i++) {
                pdata_nand[i] = 0x00;
            }
        }
        spistack.SPI_STACK_WriteEnable();
        num = spistack.SPI_NAND_ProgramDataLoad(0x00, pdata_nand);
        if(num) {
            pc.printf("Program Data Load (02h) : num = %d\n", num);
        }
        spistack.SPI_STACK_WriteEnable();
        num = spistack.SPI_NAND_ProgramExecute(addr);
        if(num) {
            pc.printf("Program Execute (10h) : done\n");
        }
    }
}

// Detect Initial Bad Block Marker
void detect_ibbm(void) {
    uint32_t addr=0;
    uint8_t rdata[PAGE_SIZE_SPI_NAND]={};    // Read data buffer
    uint32_t bb_num=0;

    for(uint16_t i=0; i<MAX_BLOCKS_SPI_NAND; i++) {
        spistack.SPI_NAND_PageDataRead(addr);
        spistack.SPI_NAND_ReadData2(0, rdata);
        if(rdata[0]!=0xFF) {
            pc.printf("Block %d = Initial Bad Block\n", i);
            bb_num = bb_num + 1;
        }
        addr = addr + 64;
    }
    pc.printf("Total Number of Initial Bad Blocks = %d\n", bb_num);
}

int main() {
    uint8_t temp[32];    // Temporary data buffer
    uint8_t menu;
    uint32_t address;
    uint8_t active_die;

    pc.printf("\n");

    while(1) {
        pc.printf("=====================================================\n");
        pc.printf("=           mbed SpiStack Sample Program            =\n");
        pc.printf("=          Winbond Electronics Corporation          =\n");
        pc.printf("=====================================================\n");

        // Get Manufacture and Device ID
        spistack.SPI_NOR_ReadID(temp);
        if(temp[1]==0x40 && temp[2]==0x15) {
            active_die = 0;
        } else {
            spistack.SPI_NAND_ReadID(temp);
            active_die = 1;
        }
        
        pc.printf("MFR ID : ");
        for(uint8_t i=0; i<1; i++) {
            pc.printf("%02X", temp[i]);
            pc.printf(" ");
        }
        pc.printf("\n");
        pc.printf("DEV ID : ");
        for(uint8_t i=1; i<3; i++) {
            pc.printf("%02X", temp[i]);
            pc.printf(" ");
        }
        pc.printf("\n");

        if(active_die==0) {
            pc.printf("=====================================================\n");
            pc.printf("Active Die : Serial NOR\n");
            pc.printf("=====================================================\n");
            pc.printf("Menu :\n");
            pc.printf("  0. Die Select to Serial NAND\n");
            pc.printf("  1. Read\n");
            pc.printf("  2. Block Erase\n");
            pc.printf("  3. Program Increment Data\n");
            pc.printf("  4. Program AAh 55h\n");
            pc.printf("  5. Program FFh 00h\n");
            pc.printf("  6. Program 00h\n");
            pc.printf("Please input menu number: ");
            pc.scanf("%d", &menu);
        
            switch(menu) {
                case 0:
                    pc.printf(">Die Select to Serial NAND\n");
                    spistack.SPI_STACK_Die_Select_NAND();
                    break;
                case 1:
                    pc.printf(">Read\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    read_data(address, active_die);
                    break;
                case 2:
                    pc.printf(">Block Erase\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    block_erase(address, active_die);
                    break;
                case 3:
                    pc.printf(">Program Increment Data\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 0, active_die);
                    break;
                case 4:
                    pc.printf(">Program AAh 55h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 1, active_die);
                    break;
                case 5:
                    pc.printf(">Program FFh 00h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 2, active_die);
                    break;
                case 6:
                    pc.printf(">Program 00h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 3, active_die);
                    break;
                default:
                    pc.printf("Invalid menu number\n");
                    break;
            }
        } else if(active_die==1) {
            // Set Block Protect Bits to 0
            spistack.SPI_NAND_WriteStatusReg1(0x00);
            
            // Get ECC-E and BUF of Status Register-2
            temp[0] = spistack.SPI_NAND_ReadStatusReg2();
            if(temp[0] & 0x10) {
                pc.printf("ECC-E  : 1\n");
            } else {
                pc.printf("ECC-E  : 0\n");
            }
            if(temp[0] & 0x08) {
                pc.printf("BUF    : 1\n");
            } else {
                pc.printf("BUF    : 0\n");
            }

            // Get P-FAIL, E-FAIL and WEL of Status Register-3
            temp[0] = spistack.SPI_NAND_ReadStatusReg3();
            if(temp[0] & 0x08) {
                pc.printf("P-FAIL : 1\n");
            } else {
                pc.printf("P-FAIL : 0\n");
            }
            if(temp[0] & 0x04) {
                pc.printf("E-FAIL : 1\n");
            } else {
                pc.printf("E-FAIL : 0\n");
            }
            pc.printf("=====================================================\n");
            pc.printf("Active Die : Serial NAND\n");
            pc.printf("=====================================================\n");
            pc.printf("Menu :\n");
            pc.printf("  0. Die Select to Serial NOR\n");
            pc.printf("  1. Read\n");
            pc.printf("  2. Block Erase\n");
            pc.printf("  3. Program Increment Data\n");
            pc.printf("  4. Program AAh 55h\n");
            pc.printf("  5. Program FFh 00h\n");
            pc.printf("  6. Program 00h\n");
            pc.printf("  7. Detect Initial Bad Block Marker\n");
            pc.printf("       -> Please execute \"7\" before the first Erase/Program.\n");
            pc.printf("Please input menu number: ");
            pc.scanf("%d", &menu);

            switch(menu) {
                case 0:
                    pc.printf(">Die Select to Serial NOR\n");
                    spistack.SPI_STACK_Die_Select_NOR();
                    break;
                case 1:
                    pc.printf(">Read\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    read_data(address, active_die);
                    break;
                case 2:
                    pc.printf(">Block Erase\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    block_erase(address, active_die);
                    break;
                case 3:
                    pc.printf(">Program Increment Data\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 0, active_die);
                    break;
                case 4:
                    pc.printf(">Program AAh 55h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 1, active_die);
                    break;
                case 5:
                    pc.printf(">Program FFh 00h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 2, active_die);
                    break;
                case 6:
                    pc.printf(">Program 00h\n");
                    pc.printf(">Please input address: ");
                    pc.scanf("%x", &address);
                    program_data(address, 3, active_die);
                    break;
                case 7:
                    pc.printf(">Detect Bad Block Marker\n");
                    detect_ibbm();
                    break;
               default:
                    pc.printf("Invalid menu number\n");
                    break;
            }
        }
    }
}
