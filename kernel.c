void main() {
    // char *video_memory = (char *)0xb8000;
    // *video_memory = 'x';
    // *(video_memory + 1) = 0x0f;
    *((int*)0xb8000)=0x07690748;
}
