#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FOOT_CONFIF_FILEPATH "/home/adi/.config/foot/foot.ini"
#define BUFLEN 256
#define TEMP_FILEPATH "/home/adi/.theme_temp"

ssize_t buffered_read(int fd, char *buf, ssize_t len);

typedef struct
{
    char *family;
    int size;
    char *weight;
    double letter_spacing;
} Font;

Font select_font();
Font global_fonts[] = {
    {"CaskaydiaCove NF", 15, "SemiBold", -0.4},
    {"JetBrainsMono NFM", 17, "Medium", -0.4},
    {"Agave Nerd Font", 17, "Regular", -0.3},
    {"SF Mono", 17, "Regular", -0.3},
};

int main(void)
{

    if (unlink(TEMP_FILEPATH) == 0)
    {
        printf("Removed existing theme temporary file\n");
    }

    Font font = select_font();

    int file_fd = open(FOOT_CONFIF_FILEPATH, O_RDONLY);
    if (file_fd < 0)
    {
        perror("File opening");
    }

    int temp_file_fd = open(TEMP_FILEPATH, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (temp_file_fd < 0)
    {
        perror("Temp File opening");
    }

    char buffer[BUFLEN];
    ssize_t read_n;

    while ((read_n = buffered_read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        if (strncmp(buffer, "font=", 5) == 0)
        {
            int n =
                snprintf(buffer, sizeof(buffer), "font=%s:size=%d:style=%s\n",
                         font.family, font.size, font.weight);
            write(temp_file_fd, buffer, n);
            continue;
        }
        if (strncmp(buffer, "letter-spacing=", 15) == 0)
        {
            int n = snprintf(buffer, sizeof(buffer), "letter-spacing=%.1f\n",
                             font.letter_spacing);
            write(temp_file_fd, buffer, n);
            continue;
        }
        write(temp_file_fd, buffer, read_n);
    }

    close(file_fd);
    close(temp_file_fd);

    printf("\n");

    if (rename(TEMP_FILEPATH, FOOT_CONFIF_FILEPATH) != 0)
    {
        perror("Rename");
    }
    else
    {
        printf("Moving temporary file to config file...\n");
    }

    return 0;
}

ssize_t buffered_read(int fd, char *buf, ssize_t len)
{
    memset(buf, 0, len);

    int i = 0;
    char c;
    while (i < len - 1)
    {
        ssize_t n = read(fd, &c, 1);
        if (n < 0)
        {
            return -1;
        }
        if (n == 0)
        {
            break;
        }
        buf[i++] = c;
        if (c == '\n')
        {
            break;
        }
    }
    buf[i] = '\0';

    return i;
}

Font select_font()
{

    printf("\n");
    for (int i = 0; i < sizeof(global_fonts) / sizeof(global_fonts[0]); i++)
    {
        printf("  %d. %s\n", i + 1, global_fonts[i].family);
    }

    int choice = -1;

    printf("  → ");
    do
    {
        scanf("%d", &choice);
    } while (choice < 1 ||
             choice > sizeof(global_fonts) / sizeof(global_fonts[0]));

    return global_fonts[choice - 1];
}
