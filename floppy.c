#include "floppy.h"

enum Registers {
    STATUS_REGISTER_A                = 0x3F0, // read-only
    STATUS_REGISTER_B                = 0x3F1, // read-only
    DOR                              = 0x3F2,
    TAPE_DRIVE_REGISTER              = 0x3F3,
    MSR                              = 0x3F4,
    DSR                              = 0x3F4, // write-only
    FIFO                             = 0x3F5,
    DIR                              = 0x3F7, // read-only
    CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum Commands {
    READ_TRACK =                 2,  // generates IRQ6
    SPECIFY =                    3,  // * set drive parameters
    SENSE_DRIVE_STATUS =         4,
    WRITE_DATA =                 5,  // * write to the disk
    READ_DATA =                  6,  // * read from the disk
    RECALIBRATE =                7,  // * seek to cylinder 0
    SENSE_INTERRUPT =            8,  // * ack IRQ6, get status of last command
    WRITE_DELETED_DATA =         9,
    READ_ID =                    10, // generates IRQ6
    READ_DELETED_DATA =          12,
    FORMAT_TRACK =               13, // *
    SEEK =                       15, // * seek both heads to cylinder X
    VERSION =                    16, // * used during initialization, once
    SCAN_EQUAL =                 17,
    PERPENDICULAR_MODE =         18, // * used during initialization, once, maybe
    CONFIGURE =                  19, // * set controller parameters
    LOCK =                       20, // * protect controller params from a reset
    VERIFY =                     22,
    SCAN_LOW_OR_EQUAL =          25,
    SCAN_HIGH_OR_EQUAL =         29
};

enum Options {
    MTBIT = 0x80,
    MFMBIT = 0x40,
    SKBIT = 0x20
};

volatile int irq6_done = 0;

void _irq6_handler() {
    irq6_done = 1;
    *(char*)(0xb800a) = 0x4C;
}

void wait_for_rqm() {
    uint8_t msr = 0;
    while (!(msr & 0x80)) {
        msr = inb(MSR);
    }
}

void sense_interrupt() {
    outb(FIFO, SENSE_INTERRUPT);
    inb(FIFO);
    inb(FIFO);
}

void specify() {
    outb(FIFO, SPECIFY);
    outb(FIFO, 8 << 4 | 0);
    outb(FIFO, 5 << 1 | 0);
}

void configure() {
    outb(FIFO, CONFIGURE);

    outb(FIFO, 0x00);
    outb(FIFO, 1 << 6 | 0 << 5 | 1 << 4 | (1 - 1));
    outb(FIFO, 0x00);
}

void recalibrate() {
    irq6_done = 0;

    outb(FIFO, RECALIBRATE);
    outb(FIFO, 0x00);

    while (!irq6_done);

    sense_interrupt();
}

void motoron() {
    outb(DOR, 0x1C);
}

void floppy_init() {
    floppy_reset();
    specify();
    configure();
    motoron();
    recalibrate();

    *(char*)(0xb800c) = 0x4C;
}

void floppy_reset() {
    irq6_done = 0;

    outb(DOR, 0x00);
    outb(DOR, 0x0C);

    while (!irq6_done);

    sense_interrupt();
    sense_interrupt();
    sense_interrupt();
    sense_interrupt();

    outb(DSR, 0x00);
}

void floppy_seek() {
    irq6_done = 0;

    outb(FIFO, SEEK);
    outb(FIFO, 0 << 2 | 0); // head << 2 | drive
    outb(FIFO, 0); // cylinder

    while (!irq6_done);

    sense_interrupt();
}

void floppy_read(uint8_t *buffer) {
    irq6_done = 0;

    outb(FIFO, MTBIT | MFMBIT | READ_DATA);

    outb(FIFO, 0 << 2 | 0); // head << 2 | drive
    outb(FIFO, 0); // cylinder
    outb(FIFO, 0); // head
    outb(FIFO, 1); // sector start
    outb(FIFO, 2); // LEAVE (512 byte sectors)
    outb(FIFO, 1); // sectors
    outb(FIFO, 0x1b); // LEAVE (GAP1??)
    outb(FIFO, 0xff); // LEAVE (also 512 byte sectors?)

    while (!irq6_done);

    uint8_t st0 = inb(FIFO);
    uint8_t st1 = inb(FIFO);
    uint8_t st2 = inb(FIFO);
    uint8_t cylinder = inb(FIFO);
    uint8_t ending_head = inb(FIFO);
    uint8_t ending_sector = inb(FIFO);
    uint8_t should_be_2 = inb(FIFO);
}
