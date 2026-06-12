#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define COLOR_RED "\e[0;31m"
#define COLOR_GREEN "\e[0;32m"
#define COLOR_BLUE "\e[0;34m"
#define COLOR_RESET "\e[0m"

#define SLEEP 600000

int direction = 1;
int level = 0;

char *sprites = "/\\_";
size_t sprites_len = 3;

typedef struct
{
    char sprite;
    char *color;
} Sprite;

typedef struct
{
    size_t width;
    size_t height;
    Sprite **state;
} Window;

Window *NewWindow(size_t width, size_t height)
{
    Window *window = (Window *)malloc(sizeof(Window));

    window->width = width;
    window->height = height;

    window->state = (Sprite **)malloc(sizeof(Sprite *) * height);
    for (size_t row = 0; row < window->height; row++)
    {
        window->state[row] = (Sprite *)malloc(sizeof(Sprite) * window->width);
    }

    for (size_t row = 0; row < window->height; row++)
    {
        for (size_t col = 0; col < window->width; col++)
        {
            window->state[row][col].sprite = ' ';
            window->state[row][col].color = COLOR_RESET;
        }
    }

    return window;
}

void DrawTriangle(Window *window, size_t row, size_t col, char *color)
{
    window->state[row + 0][col + 1].sprite = sprites[0];
    window->state[row + 0][col + 1].color = color;

    window->state[row + 0][col + 2].sprite = sprites[1];
    window->state[row + 0][col + 2].color = color;

    window->state[row + 1][col + 0].sprite = sprites[0];
    window->state[row + 1][col + 0].color = color;
    window->state[row + 1][col + 1].sprite = sprites[2];
    window->state[row + 1][col + 1].color = color;
    window->state[row + 1][col + 2].sprite = sprites[2];
    window->state[row + 1][col + 2].color = color;
    window->state[row + 1][col + 3].sprite = sprites[1];
    window->state[row + 1][col + 3].color = color;
}

void GenerateTriangle(Window *window, int level, size_t row, size_t col, char *top_color, char *left_color,
                      char *right_color)
{
    if (level == 0)
    {
        DrawTriangle(window, row, col, top_color);
        return;
    }

    int offset = 1 << level;

    GenerateTriangle(window, level - 1, row, col + offset, top_color, top_color, top_color);
    GenerateTriangle(window, level - 1, row + offset, col, left_color, left_color, left_color);
    GenerateTriangle(window, level - 1, row + offset, col + (offset * 2), right_color, right_color, right_color);
}

void Clear(Window *window)
{
    for (size_t row = 0; row < window->height; row++)
    {
        for (size_t col = 0; col < window->width; col++)
        {
            window->state[row][col].sprite = ' ';
            window->state[row][col].color = COLOR_RESET;
        }
    }
}

void Draw(Window *window)
{
    int height = 1 << (level + 1);
    int width = height * 2;

    int start_row = (window->height - height) / 2;
    int start_col = (window->width - width) / 2;

    GenerateTriangle(window, level, start_row, start_col, COLOR_RED, COLOR_GREEN, COLOR_BLUE);

    for (size_t row = 0; row < window->height; row++)
    {
        for (size_t col = 0; col < window->width; col++)
        {
            printf("%s%c", window->state[row][col].color, window->state[row][col].sprite);
        }
        printf(COLOR_RESET "\n");
    }
}

void Update(Window *window)
{
    if (level >= 4)
    {
        direction = -1;
    }
    else if (level <= 1)
    {
        direction = 1;
    }

    level += direction;
}

void Loop(Window *window)
{
    while (1)
    {
        Update(window);
        Clear(window);
        Draw(window);
        usleep(SLEEP);
    }
}

int main()
{
    struct winsize win;
    ioctl(0, TIOCGWINSZ, &win);

    Window *window = NewWindow(win.ws_col, win.ws_row - 1);
    Loop(window);

    return 0;
}
