__attribute__((naked)) void test_write() {
    short *video_memory = (short*)0xb8000;
    video_memory[0] = 0x0748;
    video_memory[1] = 0x0765;
    video_memory[2] = 0x076C;
    video_memory[3] = 0x076C;
    video_memory[4] = 0x076F;
}
