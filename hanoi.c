#include <stdio.h>
#include <stdlib.h>

#define RODS 3
#define DISKS 3
#define JTAG_UART_BASE ((volatile int *)0xFF201000)
#define JTAG_UART_CONTROL ((volatile int *)(0xFF201000 + 4))
#define BLUE 0x00ff
#define GREEN 0xff00
#define BLACK 0x0000
#define RED 0xf800
#define YELLOW 0xff70
#define PINK 0xf81F

#define SWITCHES ((volatile unsigned long *) 0xFF200040)

// Function prototypes
void initializeGame(int rods[RODS][DISKS], int disks);
void printRods(int rods[RODS][DISKS], int top[RODS]);
void moveDisk(int rods[RODS][DISKS], int top[RODS], int fromRod, int toRod);
void towerOfHanoi(int disks, int fromRod, int toRod, int auxRod);
int getUserInput();
void writeString(int x, int y, char s[]);
void homeScreen();
void rules();
int game() ;
void write_pixel(int x, int y, short colour);
void write_char(int x, int y, char c);
void clear_screen() ;
void draw_square(int x, int y, int size, short color) ;
char get_jtag(volatile int *JTAG_UART_ptr);
void put_jtag(volatile int *JTAG_UART_ptr, char c);
short rgbconv(int r, int g, int b);

short rgbconv(int r, int g, int b) {
    short color = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
    return color;
}


char get_jtag(volatile int *JTAG_UART_ptr)
{
    int data;
    data = *(JTAG_UART_ptr);
    if (data & 0x00008000) // check RVALID
        return ((char)data & 0xFF);
    else
        return ('\0');
}

// print user text to console
void put_jtag(volatile int *JTAG_UART_ptr, char c)
{
    int control;
    control = *(JTAG_UART_ptr + 1); // read control reg
    if (control & 0xFFFF0000)
    {
        *(JTAG_UART_ptr) = c;
    }
}

void writeString(int x, int y, char s[])
{
    int i;
    for ( i = 0; i < strlen(s); i++)
    {
        write_char(x, y, s[i]);
        x++;
    }
}

void homeScreen()
{
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    clear_screen();
    

    char h1[] = "Welcome!";
    writeString(34, 20, h1);
    char h3[] = "Press TAB to read RULES of the game";
    writeString(21, 40, h3);
    char h2[] = "Press ENTER to start";
    writeString(29, 25, h2);

    while (1)
    {
        c = get_jtag(JTAG_UART_ptr);
        if (c == 0x0a)
        {
            clear_screen();
            game();

            break;
        }
        if (c == 0x09)
        {
            clear_screen;
            rules();

            break;
        }
    }
}

void rules()
{
    clear_screen();
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    char h1[] = "The motto of this game is to stack all disks on Rod C ";
    writeString(16, 20, h1);


    char h3[] = "How to Play: ";
    writeString(31, 30, h3);

    char h4[] = "1. Move all disks from Rod A (source) to Rod C (destination).";
    writeString(10, 35, h4);

    char h5[] = "2. Slide the switch to select the source rod (A, B, or C).";
    writeString(10, 40, h5);

    char h6[] = "3. Then, slide the switch to choose the destination rod (A, B, or C) for the top disk.";
    writeString(10, 45, h6);

    char h7[] = "4. Larger disks cannot be placed on smaller disks.";
    writeString(10, 50, h7);

    while (1)
    {
        c = get_jtag(JTAG_UART_ptr);

        if (c == 0x1b)
        {
            homeScreen();
            return;
        }
    }
}

int game_over(){
   
    clear_screen();
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    char h1[] = "YOU HAVE WON!!";
    writeString(32, 30, h1);


    char h7[] = "Press Esc to go to the Home page";
    writeString(25, 35, h7);

    while (1)
    {
        c = get_jtag(JTAG_UART_ptr);

        if (c == 0x1b)
        {
            homeScreen();
            return;
        }
    }

}

int main()
{
    clear_screen();
    homeScreen();
}

int game() {
    int rods[RODS][DISKS];  
    int top[RODS] = {DISKS, 0, 0};  // Top index of disks for each rod
    
    initializeGame(rods, DISKS);
    // printRods(rods, top);
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    
    printf("Tower of Hanoi Game\n");
    printf("Use '1', '2', '3' keys to move top disk from one rod to another.\n");
    
    int sourceRod, destRod;
    while (1) {
        printRods(rods, top);
        printf("Enter source rod (1, 2, 3): ");
        sourceRod = getUserInput() - 1;
        if (sourceRod < 0 || sourceRod >= RODS) {
            printf("Invalid input. Please enter a valid rod (1, 2, 3).\n");
            continue;
        }
        
        printf("Enter destination rod (1, 2, 3): ");
        destRod = getUserInput() - 1;
        if (destRod < 0 || destRod >= RODS) {
            printf("Invalid input. Please enter a valid rod (1, 2, 3).\n");
            continue;
        }
        
        if (top[sourceRod] == 0) {
            printf("No disk to move from rod %d.\n", sourceRod + 1);
            continue;
        }
        
        if (top[destRod] > 0 && rods[sourceRod][top[sourceRod] - 1] > rods[destRod][top[destRod] - 1]) {
            printf("Cannot place a larger disk on top of a smaller disk.\n");
            continue;
        }
        
        moveDisk(rods, top, sourceRod, destRod);
        
        // Check if all disks are moved to rod 3
        if (top[2] == DISKS) {
			printRods(rods, top);
            printf("You have won!\n");
            game_over();
        }
        
        
    }
    
    return 0;
}

void initializeGame(int rods[RODS][DISKS], int disks) {
    // Place all disks on rod 1 (index 0) in descending order (3, 2, 1)
    for (int i = 0; i < disks; i++) {
        rods[0][i] = disks - i;  // Place disk (disks - i) on rod 1 (index 0)
    }
    
    
}

void write_pixel(int x, int y, short colour) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=colour;
}

void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}

void clear_screen() {
  int x, y;

  // Fill the screen with blue color (0x001F for a blue pixel)
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
      write_pixel(x, y, 0); // Blue color
    }
  }

  // Draw a yellow border with a thickness of 3 pixels and a gap of 7 pixels from the edge
  int borderThickness = 3;
  int gapSize = 7;

  // Draw top border
  for (x = gapSize; x < 320 - gapSize; x++) {
    for (y = 0; y < borderThickness; y++) {
      write_pixel(x, y, 0xFFE0); // Yellow color for the border
    }
  }

  // Draw bottom border
  for (x = gapSize; x < 320 - gapSize; x++) {
    for (y = 240 - borderThickness; y < 240; y++) {
      write_pixel(x, y, 0xFFE0); // Yellow color for the border
    }
  }

  // Draw left border (excluding corners covered by top and bottom borders)
  for (y = gapSize; y < 240 - gapSize; y++) {
    for (x = 0; x < borderThickness; x++) {
      write_pixel(x, y, 0xFFE0); // Yellow color for the border
    }
  }

  // Draw right border (excluding corners covered by top and bottom borders)
  for (y = gapSize; y < 240 - gapSize; y++) {
    for (x = 320 - borderThickness; x < 320; x++) {
      write_pixel(x, y, 0xFFE0); // Yellow color for the border
    }
  }
  for (x = 0; x < 80; x++)
    {
        for (y = 0; y < 60; y++)
        {
            write_char(x, y, (char)0);
        }
    }

  char *text = "Tower of Hanoi";
    int text_length = 15; // Length of "Tower of Hanoi"

    // Calculate starting X-coordinate for centering
    int start_x = (320 ) / 2;

    // Print the text at the calculated starting position
    for (int i = 0; i < text_length; i++) {
        write_char(start_x + i, 5, text[i]); // Y-coordinate is 12 (center vertically)
    }

    char *text2 = "Erum Meraj";
    int text2_length = 11; // Length of "Tower of Hanoi"

   
    start_x = start_x + 2;
    // Print the text2 at the calculated starting position
    for (int i = 0; i < text2_length; i++) {
        write_char(start_x + i, 10, text2[i]); // Y-coordinate is 12 (center vertically)
    }

    char *text3 = "2201CS24";
    int text3_length = 8; // Length of "Tower of Hanoi"
    start_x = start_x + 1;
    // Print the text3 at the calculated starting position
    for (int i = 0; i < text3_length; i++) {
        write_char(start_x + i, 12, text3[i]); // Y-coordinate is 12 (center vertically)
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
 
void drawRodsAndDisks(int rods[3][3], int top[3]) {
    clear_screen();
    for (int h = 0; h < 270; h++) {
            write_pixel(30 + h, 200, 0xFFFF);  // White color (vertical line)
        }
    for (int i = 0; i < 3; i++) {
        int rodX = 60 + (i * 100);
        int rodY = 0;

        // Draw vertical line for the rod
        for (int h = 100; h < 200; h++) {
            write_pixel(rodX, rodY + h, 0xFFFF);  // White color (vertical line)
        }
        
        // Calculate starting Y position for the bottom disk on this rod
        int diskY = 195;  // Adjust this value based on the rod height and disk sizes

        // Iterate through disks from bottom to top (from largest to smallest)
        for (int d = 0; d < top[i]; d++) {
            int diskWeight = rods[i][d];
            if (diskWeight > 0) {
                int diskWidth = diskWeight * 10;  // Calculate disk width based on weight
                int diskX = rodX - diskWidth / 2;  // Center disk on the rod
                draw_square(diskX, diskY - diskWidth, diskWidth, 0xF800);
                diskY -= diskWidth + 5;  // Move up for the next disk (next smaller size)
            }
        }
    }
}



void printRods(int rods[RODS][DISKS], int top[RODS]) {
    
    printf("Rod 1: ");
    for (int i = 0; i < top[0]; i++) {
        printf("%d ", rods[0][i]);
    }
    printf("\nRod 2: ");
    for (int i = 0; i < top[1]; i++) {
        printf("%d ", rods[1][i]);
    }
    printf("\nRod 3: ");
    for (int i = 0; i < top[2]; i++) {
        printf("%d ", rods[2][i]);
    }
    printf("\n");
    drawRodsAndDisks(rods, top);
}

void moveDisk(int rods[RODS][DISKS], int top[RODS], int fromRod, int toRod) {
    if (top[fromRod] == 0) {
        printf("No disk to move from rod %d.\n", fromRod + 1);
        return;
    }

    int disk = rods[fromRod][top[fromRod] - 1];

    if (top[toRod] > 0 && rods[toRod][top[toRod] - 1] < disk) {
        printf("Cannot place a larger disk on top of a smaller disk.\n");
        return;
    }

    top[fromRod]--;
    rods[toRod][top[toRod]] = disk;
    top[toRod]++;
}


int getUserInput() {
    unsigned long currentSwval;
    unsigned long previousSwval = *SWITCHES;
    
    // Wait until all switches are off
    while (previousSwval != 0) {
        currentSwval = *SWITCHES;
        
        // Check if any switch is pressed (bits 0 to 5)
        if (currentSwval == 0 && previousSwval != 0) {
            // All switches are off now
            break;
        }
        
        previousSwval = currentSwval;
    }
    
    int option = -1;
    
    // Wait for a switch to be pressed
    while (1) {
        currentSwval = *SWITCHES;
        
        // Check if any switch is pressed (bits 0 to 5)
        for (int i = 0; i < 6; i++) {
            unsigned long mask = (1 << i);
            if ((currentSwval & mask) && !(previousSwval & mask)) {
                option = i;  // Set option to the index of the pressed switch
                break;
            }
        }
        
        // Update the previous switch state
        previousSwval = currentSwval;
        
        // Display the selected option
        if (option != -1) {
            // Wait until all switches are off again
            while (currentSwval != 0) {
                currentSwval = *SWITCHES;
            }
            break;  // Exit the loop once an option is selected
        }
    }
    
    return option;
}


// void towerOfHanoi(int disks, int fromRod, int toRod, int auxRod) {
//     if (disks == 1) {
//         printf("Move disk from rod %d to rod %d\n", fromRod + 1, toRod + 1);
//         return;
//     }
//     towerOfHanoi(disks - 1, fromRod, auxRod, toRod);
//     printf("Move disk from rod %d to rod %d\n", fromRod + 1, toRod + 1);
//     towerOfHanoi(disks - 1, auxRod, toRod, fromRod);
// }
