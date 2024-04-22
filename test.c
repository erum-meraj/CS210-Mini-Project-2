
void write_pixel(int x, int y, short colour) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=colour;
}

/* use write_pixel to set entire screen to black (does not clear the character buffer) */
void clear_screen() {
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
	  write_pixel(x,y,0);
	}
  }
}

void draw_square(int x, int y, int size, short color) {
    int i, j;
    for (i = y; i < y + size; i++) {
        for (j = x; j < x + size; j++) {
            write_pixel(j, i, color);
        }
    }
}
 
void drawRodsAndDisks(int rods[3][3]) {
    for (int i = 0; i < 3; i++) {
        int rodX = 20 + (i * 60);
        int rodY = 0;
        
        // Draw vertical line for the rod
        for (int h = 0; h < 240; h++) {
            write_pixel(rodX, rodY + h, 0xFFFF);  // White color (vertical line)
        }
        int diskY = 40;
        for (int d = 0; d < 3; d++) {
            int diskWeight = rods[i][d];
            if (diskWeight > 0) {
                int diskWidth = diskWeight * 10;  // Calculate disk width based on weight
                int diskX = rodX - diskWidth / 2;  // Center disk on the rod
                draw_square(diskX, diskY, diskWidth, 0xF800);
                diskY += diskWidth;  
            }
        }
    }
}
int main() {
    // Clear the screen
    clear_screen();

    int rods[3][3] = {
        {1, 2, 3},  // Disks on rod 1 (from bottom to top)
        {2, 3, 1},  // Disks on rod 2 (no disks)
        {0, 0, 0}   // Disks on rod 3 (no disks)
    };
    
    // Draw rods and disks based on the current state
    drawRodsAndDisks(rods);

    return 0;
}
