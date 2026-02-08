extern "C" void kernel_main() {
    volatile char* vga = (char*)0xb8000;
    vga[0] = 'H'; vga[1] = 0x0F; // Affiche un 'H' blanc
    while(1);
}