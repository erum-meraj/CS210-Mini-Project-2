#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define JTAG_UART_BASE ((volatile int *)0xFF201000)
#define JTAG_UART_CONTROL ((volatile int *)(0xFF201000 + 4))

#define BLUE 0x00ff
#define GREEN 0xff00
#define BLACK 0x0000
#define RED 0xf800
#define YELLOW 0xff70
#define PINK 0xf81F

int random = 1;
// wrie a color to the pixel
void writePixel(int x, int y, short color)
{
    volatile short *vga_addr = (volatile short *)(0x08000000 + (y << 10) + (x << 1));
    *vga_addr = color;
}

// returns the pixel's color
short readPixel(int x, int y)
{
    volatile short *vga_addr = (volatile short *)(0x08000000 + (y << 10) + (x << 1));
    return *vga_addr;
}

void writeChar(int x, int y, char c)
{
    // VGA character buffer
    volatile char *character_buffer = (char *)(0x09000000 + (y << 7) + x);
    *character_buffer = c;
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
// write an entire character array into the character buffer
void writeString(int x, int y, char s[])
{
    int i;
    for ( i = 0; i < strlen(s); i++)
    {
        writeChar(x, y, s[i]);
        x++;
    }
}

void clearScreen()
{
    int x, y;
    for (x = 0; x < 320; x++)
    {
        for (y = 0; y < 240; y++)
        {
            writePixel(x, y, 0x0000); // write black color to all the pixels on display
        }
    }

    for (x = 0; x < 80; x++)
    {
        for (y = 0; y < 60; y++)
        {
            writeChar(x, y, (char)0);
        }
    }
}

// utility function for the generation of the circle
void helpCircle(int xc, int yc, int x, int y, short c)
{
    writePixel(xc + x, yc + y, c);
    writePixel(xc - x, yc + y, c);
    writePixel(xc + x, yc - y, c);
    writePixel(xc - x, yc - y, c);
    writePixel(xc + y, yc + x, c);
    writePixel(xc - y, yc + x, c);
    writePixel(xc + y, yc - x, c);
    writePixel(xc - y, yc - x, c);
}
// Bresenham's Algorithm to generate a circle
void drawCircle(int xc, int yc, int r, short colour)
{
    short c = colour;
    int x = 0, y = r;
    int d = 3 - 2 * r;
    helpCircle(xc, yc, x, y, c);
    while (y >= x)
    {
        x++;
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        helpCircle(xc, yc, x, y, c);
    }
}

// Bresenham's algorithm to generate a Line in a plane between two points
void drawLine(int x1, int y1, int x2, int y2, short colour)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int two_dx = 2 * dx;
    int two_dy = 2 * dy;
    int p = two_dy - dx;
    int x = x1, y = y1;
    int ix = x1 < x2 ? 1 : -1;
    int iy = y1 < y2 ? 1 : -1;
    if (dx >= dy)
    {
        while (x != x2)
        {
            writePixel(x, y, colour);
            if (p >= 0)
            {
                y += iy;
                p -= two_dx;
            }
            p += two_dy;
            x += ix;
        }
    }
    else
    {
        while (y != y2)
        {
            writePixel(x, y, colour);
            if (p >= 0)
            {
                x += ix;
                p -= two_dy;
            }
            p += two_dx;
            y += iy;
        }
    }
}

void outerStructure(int xc, int yc, int r, short col, short col2)
{
    drawCircle(xc, yc, r, col);

    // int h = round(r * sqrt(3) / 2);
    int h = 43;
    int i = r / 2;
    drawCircle(xc - i, yc + h, 5, col2); // bottom left
    drawCircle(xc - i, yc - h, 5, col2); // top left
    drawCircle(xc - r, yc, 5, col2);     // left left
    drawCircle(xc + r, yc, 5, col2);     // right right
    drawCircle(xc + i, yc + h, 5, col2); // bottom right
    drawCircle(xc + i, yc - h, 5, col2); // top right
}

// Function to fill color into a circle with centre and radius
void colorFill(int xc, int yc, int r, int x)
{
    int i = 0;
    if (x == 1)
        for (i; i <= r; i++)
            drawCircle(xc, yc, i, 0xF800);
    else if (x == 0)
        for (i; i <= r; i++)
            drawCircle(xc, yc, i, 0x0FF0);
    else
        for (i; i <= r; i++)
            drawCircle(xc, yc, i, 0x00ff);
}

// assigning color to the dots according to the values in the array
// 1 => RED
// 0 => GREEN
void fillMyArray(int arr[])
{
    int r = 50;
    // int h = round(r * sqrt(3) / 2);
    int h = 43;
    int i = r / 2;

    // Just the lines that form a triangle (Aesthetcis :>)
    drawLine(125, 100, 125 + r, 100, 0);
    drawLine(125 - i, 100 + h, 125 + i, 100 - h, 0);
    drawLine(125, 100, 125 + i, 100 + h, 0);
    drawLine(175 - i, 100 - h, 175 + i, 100 + h, 0);
    drawLine(175, 100, 175 - i, 100 + h, 0);
    drawLine(150 - r, 143, 150 + r, 143, 0);
    drawLine(125 - i, 100 + h, 125 + r, 100, 0);
    drawLine(125 + i + 2, 100 - h, 125 + i + 2, 100 + h, 0);
    drawLine(175 + i, 100 + h, 125, 100, 0);
    colorFill(125 + i, 100 + r / sqrt(12), 2, 1);

    int xc = 125, yc = 100;
    colorFill(xc - i, yc - h, 5, arr[0]); // top 1
    colorFill(xc + i, yc - h, 5, arr[1]); // top 2
    colorFill(xc - r, yc, 5, arr[3]);     // tmid 1
    colorFill(xc + r, yc, 5, arr[5]);     // tmid 3
    colorFill(xc - i, yc + h, 5, arr[7]); // lmid 1
    colorFill(xc + i, yc + h, 5, arr[8]); // lmid 2

    xc = 175, yc = 100;
    colorFill(xc + i, yc - h, 5, arr[2]); // top 3
    colorFill(xc - r, yc, 5, arr[4]);     // tmid 2
    colorFill(xc + r, yc, 5, arr[6]);     // tmid 4
    colorFill(xc + i, yc + h, 5, arr[9]); // lmid 3

    xc = 150, yc = 143;
    colorFill(xc - i, yc + h, 5, arr[10]); // bottom 1
    colorFill(xc + i, yc + h, 5, arr[11]); // bottom 2
}

// boundary of the puzzle screen
void boundary()
{
    int x, y;
    for (x = 0; x < 320; x++)
    {
        for (y = 0; y < 240; y++)
        {
            writePixel(x, y, 0xffff); // write black color to all the pixels on display
        }
    }

    for (x = 0; x < 280; x++)
    {
        writePixel(x + 20, 20, 0x0);
        writePixel(x + 20, 220, 0x0);
    }
    for (y = 0; y < 200; y++)
    {
        writePixel(20, y + 20, 0x0);
        writePixel(300, y + 20, 0x0);
    }
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize(int arr[], int n)
{
    random += 100; // changing seed value after every sucssesfull run will generate a random ordered
    srand(random);
    int i;
    for ( i = n - 1; i > 0; i--)
    {

        int j = rand() % (i + 1);

        swap(&arr[i], &arr[j]);
    }
}

// rotate LEFT gear
void rotate1(int a[12], int b[12])
{
    int xc = 125;
    int yc = 100;
    int r = 50;
    // int h = round(r * sqrt(3) / 2);
    int h = 43.30;
    int i = r / 2;
    b[0] = a[3];
    colorFill(xc - i, yc - h, 5, b[0]); // top left

    b[1] = a[0];
    colorFill(xc + i, yc - h, 5, b[1]); // top right

    b[3] = a[7];
    colorFill(xc - r, yc, 5, b[3]); // left left

    b[5] = a[1];
    colorFill(xc + r, yc, 5, b[5]); // right right

    b[7] = a[8];
    colorFill(xc - i, yc + h, 5, b[7]); // bottom left

    b[8] = a[5];
    colorFill(xc + i, yc + h, 5, b[8]); // bottom right
}

// rotate RIGHT gear
void rotate2(int a[12], int b[12])
{
    int xc = 175;
    int yc = 100;
    int r = 50;
    // int h = round(r * sqrt(3) / 2);
    int h = 43.30;
    int i = r / 2;

    b[1] = a[4];
    colorFill(xc - i, yc - h, 5, b[1]); // top left

    b[2] = a[1];
    colorFill(xc + i, yc - h, 5, b[2]); // top right

    b[6] = a[2];
    colorFill(xc + r, yc, 5, b[6]); // right right

    b[9] = a[6];
    colorFill(xc + i, yc + h, 5, b[9]); // bottom right

    b[8] = a[9];
    colorFill(xc - i, yc + h, 5, b[8]); // bottom left

    b[4] = a[8];
    colorFill(xc - r, yc, 5, b[4]); // left left
}

// rotate BOTTOM gear
void rotate3(int a[12], int b[12])
{

    int xc = 150;
    int yc = 143;
    int r = 50;
    // int h = round(r * sqrt(3) / 2);
    int h = 43.30;
    int i = r / 2;

    b[4] = a[7];
    colorFill(xc - i, yc - h, 5, b[4]); // top left

    b[5] = a[4];
    colorFill(xc + i, yc - h, 5, b[5]); // top right

    b[9] = a[5];
    colorFill(xc + r, yc, 5, b[9]); // right right

    b[11] = a[9];
    colorFill(xc + i, yc + h, 5, b[11]); // bottom right

    b[10] = a[11];
    colorFill(xc - i, yc + h, 5, b[10]); // bottom left

    b[7] = a[10];
    colorFill(xc - r, yc, 5, b[7]); // left left
}

// background of puzzle screen below the main structure wala block
void charblock()
{
    int x, y;
    for (x = 0; x < 200; x++)
    {
        for (y = 0; y < 20; y++)
        {
            writePixel(x + 60, y + 197, 0x7dff);
        }
    }
}

// background of the press ESC key to exit
void charblock2()
{
    int x, y;
    for (x = 0; x < 90; x++)
    {
        for (y = 0; y < 14; y++)
        {
            writePixel(x, y, 0x7dff);
        }
    }
}

// // just a while to expect a delay in blinking but didn't find much difference :(
void ssleep(int time)
{
    int x;
    for (x = 0; x < time; x++)
    {
        
    }
}

// To cause the blinking effect when the game is cleared its hardcoded :>
void blink()
{
    int xc = 125, yc = 100, r = 50;
    // int h = round(r * sqrt(3) / 2);
    int h = 43.30;
    int i = r / 2;
    colorFill(xc - i, yc - h, 5, 0); // top 1
    colorFill(xc + i, yc - h, 5, 1); // top 2
    colorFill(xc - r, yc, 5, 0);     // tmid 1
    colorFill(xc + r, yc, 5, 1);     // tmid 3
    colorFill(xc - i, yc + h, 5, 1); // lmid 1
    colorFill(xc + i, yc + h, 5, 1); // lmid 2

    xc = 175, yc = 100;
    colorFill(xc + i, yc - h, 5, 0); // top 3
    colorFill(xc - r, yc, 5, 1);     // tmid 2
    colorFill(xc + r, yc, 5, 0);     // tmid 4
    colorFill(xc + i, yc + h, 5, 1); // lmid 3

    xc = 150, yc = 143;
    colorFill(xc - i, yc + h, 5, 0); // bottom 1
    colorFill(xc + i, yc + h, 5, 0); // bottom 2

    ssleep(1000000);

    xc = 125;
    yc = 100;
    colorFill(xc - i, yc - h, 5, 1); // top 1
    colorFill(xc + i, yc - h, 5, 0); // top 2
    colorFill(xc - r, yc, 5, 1);     // tmid 1
    colorFill(xc + r, yc, 5, 0);     // tmid 3
    colorFill(xc - i, yc + h, 5, 0); // lmid 1
    colorFill(xc + i, yc + h, 5, 0); // lmid 2

    xc = 175, yc = 100;
    colorFill(xc + i, yc - h, 5, 1); // top 3
    colorFill(xc - r, yc, 5, 0);     // tmid 2
    colorFill(xc + r, yc, 5, 1);     // tmid 4
    colorFill(xc + i, yc + h, 5, 0); // lmid 3

    xc = 150, yc = 143;
    colorFill(xc - i, yc + h, 5, 1); // bottom 1
    colorFill(xc + i, yc + h, 5, 1); // bottom 2
    ssleep(1000000);
}

// main puzzle
void ringPuzzle()
{
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    int i;
    char c;

    clearScreen();
    boundary();

    outerStructure(125, 100, 50, 0x0ff0, 0xf800);
    outerStructure(175, 100, 50, 0x0ff0, 0xf800);
    outerStructure(150, 143, 50, 0x0ff0, 0xf800);

    int allDots[12] = {0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0};
    randomize(allDots, 12);

    /*  hardcoded to demonstrate results
    int allDots[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    allDots[0] = 1;
    allDots[1] = 1;
    allDots[5] = 1;
    allDots[4] = 1;
    allDots[8] = 1;
    allDots[9] = 1;
     */

    fillMyArray(allDots);

    int brr[12];
    for (i = 0; i < 12; i++)
        brr[i] = allDots[i];

    charblock2();
    char h[] = "Press Esc to exit.";
    writeString(2, 1, h);

    while (1)
    {
        // int flag;
        c = get_jtag(JTAG_UART_ptr);
        if (c == 0x67 || c == 0x47)
        {
            // flag = 0;
            rotate1(allDots, brr);
        }

        if (c == 0x68 || c == 0x48)
        {
            // flag = 0;
            rotate2(allDots, brr);
        }

        if (c == 0x62 || c == 0x42)
        {
            // flag = 0;
            rotate3(allDots, brr);
        }
        if (c == 0x1b)
        {
            // flag = 0;
            homeScreen();
        }

        // flag = 1;

        for (i = 0; i < 12; i++)
            allDots[i] = brr[i];

        if (brr[1] && brr[4] && brr[5] && brr[7] && brr[8] && brr[9])
        {
            charblock();
            char h1[] = "Congratulations, You Win";
            writeString(28, 50, h1);

            char h2[] = "Press 'ENTER' to go to home screen";
            writeString(23, 52, h2);

            while (1)
            {
                blink();

                c = get_jtag(JTAG_UART_ptr);
                if (c == 0x0a || c == 0x1b)
                {
                    homeScreen();
                    return;
                }
            }
        }
    }
}

void homeScreen()
{
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    clearScreen();
    int i;
    for (i = 20; i < 320; i++)
        writePixel(i, 30, YELLOW);
    for (i = 0; i < 300; i++)
        writePixel(i, 240 - 30, YELLOW);
    for (i = 0; i < 210; i++)
        writePixel(20, i, YELLOW);
    for (i = 30; i < 240; i++)
        writePixel(320 - 20, i, YELLOW);

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
            ringPuzzle();

            break;
        }
        if (c == 0x09)
        {
            rules();

            break;
        }
    }
}

void rules()
{
    clearScreen();
    volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;
    char c;
    int i;
    for (i = 20; i < 320; i++)
        writePixel(i, 30, YELLOW);
    for (i = 0; i < 300; i++)
        writePixel(i, 240 - 30, YELLOW);
    for (i = 0; i < 210; i++)
        writePixel(20, i, YELLOW);
    for (i = 30; i < 240; i++)
        writePixel(320 - 20, i, YELLOW);

    char h1[] = "The motto of this game is to rotate the 3 gears and place all ";
    writeString(7, 15, h1);
    char h2[] = "the red dots on the triangular figure at the centre";
    writeString(9, 20, h2);

    char h3[] = "How to Play: ";
    writeString(29, 30, h3);

    char h4[] = "1. Use the keys G to rotate LEFT circle";
    writeString(19, 35, h4);

    char h5[] = "2. Use the keys H to rotate RIGHT circle";
    writeString(19, 40, h5);

    char h6[] = "3. Use the keys B to rotate BOTTOM circle";
    writeString(19, 45, h6);

    char h7[] = "3. Press Esc. to exit";
    writeString(19, 50, h7);

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
    clearScreen();
    homeScreen();
}