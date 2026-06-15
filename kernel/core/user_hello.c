__attribute__((section(".text"))) unsigned char user_hello_bin[] = {
    0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00, /* mov $1, %rdi */
    0x48, 0x8d, 0x35, 0x0b, 0x00, 0x00, 0x00, /* lea 0x0b(%rip), %rsi */
    0x0f, 0x05,                               /* syscall */
    0x48, 0xc7, 0xc7, 0x02, 0x00, 0x00, 0x00, /* mov $2, %rdi */
    0x0f, 0x05,                               /* syscall */
    'H','a','l','o',' ','d','a','r','i',' ','R','i','n','g',' ','3','!',' ',
    'S','y','s','c','a','l','l',' ','b','e','r','h','a','s','i','l','.','\n','\0'
};
