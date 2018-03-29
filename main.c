#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdint.h>
#include <inttypes.h>
#include <dir.h>
#include <time.h>

#include <QBASIC_func_C.h>

#define USERNAME_SIZE 31
#define PASSWORD_TAG_SIZE 16
#define FOLDER_NAME_SIZE 41
#define DATE_SIZE 11

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define ESC 27
#define ENTER 13

uint8_t del_acc_flag;
unsigned int new_account_no, total_accounts;

struct position
{
    uint8_t col, row;
}top, bot;

struct user_details
{
    int account_no;
    char username[USERNAME_SIZE];
    char password[PASSWORD_TAG_SIZE];
}loged_in_as;

struct record_details
{
    unsigned int record_id;
    char date[DATE_SIZE];
    float amount;
    char type;
    short int no_of_tags;
    char **tags_list;
    short int description_size;
    char *description;
};

struct print_line_detail
{
    uint8_t lines_for_tags;
    uint8_t tags_in_line_1, tags_in_line_2;
    uint8_t lines_for_description;
}detail;

//Dynamic memory handling function
char **get_2D_char_array(const int, const int);
char **change_2D_char_array_size(char **, const int, const int, const int);

//Functions used in other functions
int getkey();
void gotoxy(const short int, const short int);
void print_box(const short int, const short int, const short int, const short int);
void print_detail(const char *);
int get_conformation(const char *);
void get_system_date(char *);
int check_date(char *);
long int read_record(const long int position, struct record_details *);
int get_description_length(char *);
void get_line_detail(struct record_details);
void display_tags_of_record(char **);
void display_half_description(char *);
void display_full_record(struct record_details);
void display_half_record(struct record_details, const short int, const short int);

//compare function
int tag_compare_ascending(const void *, const void *);
int tag_compare_descending(const void *, const void *);
int date_compare_ascending(const void *, const void *);
int date_compare_descending(const void *, const void *);
int amount_compare_ascending(const void *, const void *);
int amount_compare_descending(const void *, const void *);

//Getting data functions
void get_folder_name(char *, struct user_details);
void get_data(char *, const short int);
void get_edit_date(char *);
void get_edit_display_description(char *, short int);

//Window functions
int display_menu(char **, const int, int);
void display_tag_id_window(char *, char *, const short int);
void display_tags_list(char **, const unsigned int);
int display_record_id_window(const short int);
int diplay_a_record(const struct record_details);
void edit_record_window(struct record_details *);
int display_records_list(const long int *,const unsigned int);

void main_menu();
void login();
void login_menu();
void records_menu();
void add_record();
void list_records();
void filter_records();
void edit_record();
void delete_record();
void tags_menu();
void add_tag();
void list_tags();
void search_tags();
void edit_tag();
void delete_tag();
void settings_menu();
void change_password();
void delete_account();
void creat_account();

int main()
{
    FILE *f_index = NULL;
    struct user_details data;

    new_account_no = 0;
    total_accounts = 0;

    mkdir("Users");
    chdir("Users");

    f_index = fopen("index.dat", "rb");
    if (f_index == NULL)
    {
        f_index = fopen("index.dat", "ab");
        if (f_index == NULL)
        {
            print_detail("Can't open index file");
            exit(EXIT_FAILURE);
        }
    }

    while (fread(&data.account_no, sizeof(data.account_no), 1, f_index) == 1)
    {
        fread(data.username, sizeof(data.username), 1, f_index);
        total_accounts++;
        new_account_no = data.account_no;
        new_account_no++;
    }
    fclose(f_index);

    main_menu();

    return 0;
}

char **get_2D_char_array(const int m, const int n)
{
    short unsigned int i;
    char **data = NULL;

    data = (char **)malloc(m * sizeof(char *));
    for (i = 0; i < m; i++)
        data[i] = (char *)calloc(n, sizeof(char));

    return data;
}

char **change_2D_char_array_size(char **data, const int old_size, const int new_size, const int oneD_size)
{
    short unsigned int i;
    char **new_array = NULL;

    if ((old_size != new_size) || ((old_size > 0) && (new_size == 0)))
    {
        if ((data == NULL) || ((old_size == 0) && (new_size > 0)))
            new_array = get_2D_char_array(new_size, oneD_size);
        else if (old_size < new_size)
        {
            new_array = get_2D_char_array(new_size, oneD_size);

            for (i = 0; i < old_size; i++)
                strcpy(new_array[i], data[i]);
        }
        else if (old_size > new_size)
        {
            new_array = get_2D_char_array(new_size, oneD_size);

            for (i = 0; i < new_size; i++)
                strcpy(new_array[i], data[i]);
        }
    }

    for (i = 0; i < old_size; i++)
        free(data[i]);
    free(data);

    return new_array;
}

int getkey()
{
    int ch;

    gotoxy(24, 79);
    ch = getch();
    if (ch == 224)
    {
        ch = getch();
        return ch;
    }

    return ch;
}

void gotoxy(const short int row, const short int col)        //"row" ra "col" exchange gareko xa yo linema sajiloko lagi|
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {col,row};
    SetConsoleCursorPosition(h, position);
}

void print_box(const short int box_type, const short int line_no1, const short int line_no2,const short int line_type)
{
    struct box_type_ASCII_codes
    {
        unsigned char tl, tr, bl, br, ver, hor;
    }box;
    struct line_type_ASCII_codes
    {
        unsigned char left, right, hor;
    }line;

    short unsigned int i, j;

    switch (box_type)
    {
    case 1:     //single horizontal and single vertical
        box.tl = 218;
        box.tr = 191;
        box.bl = 192;
        box.br = 217;
        box.ver = 179;
        box.hor = 196;
        break;
    case 2:     //double horizontal and double vertical
        box.tl = 201;
        box.tr = 187;
        box.bl = 200;
        box.br = 188;
        box.ver = 186;
        box.hor = 205;
        break;
    case 3:     //double horizontal and single vertical
        box.tl = 213;
        box.tr = 184;
        box.bl = 212;
        box.br = 190;
        box.ver = 179;
        box.hor = 205;
        break;
    case 4:     //single horizontal and double vertical
        box.tl = 214;
        box.tr = 183;
        box.bl = 211;
        box.br = 189;
        box.ver = 186;
        box.hor = 196;
        break;
    default:
        box.tl = (unsigned char)box_type;
        box.tr = (unsigned char)box_type;
        box.bl = (unsigned char)box_type;
        box.br = (unsigned char)box_type;
        box.ver = (unsigned char)box_type;
        box.hor = (unsigned char)box_type;
    }

    switch (line_type)
    {
    case 1:     //single horizontal and double vertical
        line.left = 195;
        line.right = 180;
        line.hor = 196;
        break;
    case 2:     //double horizontal and double vertical
        line.left = 204;
        line.right = 185;
        line.hor = 205;
        break;
    case 3:     //double horizontal and single vertical
        line.left = 198;
        line.right = 181;
        line.hor = 205;
        break;
    case 4:     //single horizontal and double vertical
        line.left = 199;
        line.right = 182;
        line.hor = 196;
        break;
    default:
        line.left = (unsigned char)box_type;
        line.right = (unsigned char)box_type;
        line.hor = (unsigned char)box_type;
    }

    for (i = top.col ; i <= bot.col; i++)
    {
        gotoxy(top.row, i);
        printf("%c", box.hor);
        gotoxy(bot.row, i);
        printf("%c", box.hor);
    }

    for (i = top.row; i <= bot.row; i++)
    {
        gotoxy(i, top.col);
        printf("%c", box.ver);
        gotoxy(i, bot.col);
        printf("%c", box.ver);

        if ((i == (line_no1 + top.row - 1)) || (i == (line_no2 + top.row - 1)))
        {
            for (j = top.col ; j <= bot.col; j++)
            {
                gotoxy(i, j);
                printf("%c", line.hor);
            }
            gotoxy(i, top.col);
            printf("%c", line.left);
            gotoxy(i, bot.col);
            printf("%c", line.right);
        }
    }
    gotoxy(top.row, top.col);
    printf("%c", box.tl);
    gotoxy(top.row, bot.col);
    printf("%c", box.tr);
    gotoxy(bot.row, top.col);
    printf("%c", box.bl);
    gotoxy(bot.row, bot.col);
    printf("%c", box.br);
}

void print_detail(const char *data)
{
    short unsigned int n, len = strlen(data);

    n = (80 - len)/2;

    system("cls");

    top.row = 11;
    top.col = n - 2;
    bot.row = 13;
    bot.col = n + 1 + len;
    print_box(3, 0, 0, 0);

    gotoxy(12, n);
    printf("%s", data);

    gotoxy(24,79);
    getch();
}

int get_conformation(const char *data)
{
    short unsigned int len, key, choice = 0;

    while (1)
    {
        system("cls");

        top.row = 9;
        top.col = 27;
        bot.row = 15;
        bot.col = 51;
        print_box(2, 3, 0, 2);

        len = strlen(data);
        gotoxy(top.row + 1, ((79 - len)/2));
        printf("%s", data);

        top.row += 3;
        bot.row = top.row + 2;

        gotoxy(top.row + 1, top.col + 5);
        printf("Yes");
        gotoxy(top.row + 1, bot.col - 7);
        printf("No");

        if (choice == 1)
            top.col = 28;
        else if (choice == 0)
            top.col = 40;
        bot.col = top.col + 10;
        print_box(3, 0, 0, 0);

        key = getkey();
        if ((key == RIGHT) || (key == LEFT))
            if (choice == 1)
                choice = 0;
            else
                choice = 1;
        else if (key == ESC)
            return 0;
        else if (key == ENTER)
            return choice;
    }
}

void get_system_date(char *data)
{
    short unsigned int year, mon, day;

    time_t t = time(NULL);
    struct tm curent_time = *localtime(&t);

    year = curent_time.tm_year + 1900;
    mon = curent_time.tm_mon + 1;
    day = curent_time.tm_mday;

    strcpy(data, int_to_string(year));
    strcat(data, "-");
    if (mon < 10)
    {
        strcat(data, "0");
        strcat(data, int_to_string(mon));
    }
    else
        strcat(data, int_to_string(mon));
    strcat(data, "-");
    if (day < 10)
    {
        strcat(data, "0");
        strcat(data, int_to_string(day));
    }
    else
        strcat(data, int_to_string(day));
}

int check_date(char *data)
{
    short unsigned int arr[]={31,28,31,30,31,30,31,31,30,31,30,31};
    short unsigned int y, m, d;
    short unsigned int s_year, s_mon, s_day;
    char temp_y[5] = "", temp_m[3] = "", temp_d[3] = "";

    time_t t = time(NULL);
    struct tm curent_time = *localtime(&t);

    s_year = curent_time.tm_year + 1900;
    s_mon = curent_time.tm_mon + 1;
    s_day = curent_time.tm_mday;

    temp_y[0] = data[0];
    temp_y[1] = data[1];
    temp_y[2] = data[2];
    temp_y[3] = data[3];
    y = string_to_int(temp_y);

    temp_m[0] = data[5];
    temp_m[1] = data[6];
    m = string_to_int(temp_m);

    temp_d[0] = data[8];
    temp_d[1] = data[9];
    d = string_to_int(temp_d);

    if ((y > 0) && (y < s_year))
    {
        if ((m > 0) && (m <= 12))
        {
            if ((y%400==0 || (y%100!=0 && y%4==0)) && (m == 2))
            {
                if ((d > 0) && (d <= 29))
                    return 1;
            }
            else if ((d > 0) && (d <= arr[m - 1]))
                return 1;
        }
    }
    else if (y == s_year)
    {
        if ((m > 0) && (m < s_mon))
        {
            if ((y%400==0 || (y%100!=0 && y%4==0)) && (m == 2))
            {
                if ((d > 0) && (d <= 29))
                    return 1;
            }
            else if ((d > 0) && (d <= arr[m - 1]))
                return 1;
        }
        else if (m == s_mon)
            if ((d > 0) && (d <= s_day))
                return 1;
    }

    /*
    short int year, mon, day;

    year = s_year;
    if ((y > 0) && (y < s_year))
    {
        mon = 12;
        if ((y%400==0 || (y%100!=0 && y%4==0)) && (m == 2))
            day = 29;
        else
            day = arr[m];
    }
    else if (y == s_year)
    {
        mon = s_mon;
        if ((m > 0) && (m < s_mon))
            if ((y%400==0 || (y%100!=0 && y%4==0)) && (m == 2))
                day = 29;
            else
                day = arr[m];
        else if (m == s_mon)
            day = s_day;
    }

    if ((y > 0 && y <= year) && (m > 0 && m <= mon) && (d > 0 && d <= day))
        return 1;
    */

    return 0;
}

long int read_record(const long int start_position, struct record_details *record)
{
    FILE *fp;
    short int i;
    long int end_position;

    fp = fopen("records.dat", "rb");
    if (fp == NULL)
    {
        perror("PROCESS FAILED");
        return -1;
    }

    fseek(fp, start_position, SEEK_SET);

    fread(&record->record_id, sizeof(unsigned int), 1, fp);
    fread(record->date, sizeof(record->date), 1, fp);
    fread(&record->amount, sizeof(float), 1, fp);
    fread(&record->type, sizeof(char), 1, fp);

    fread(&record->no_of_tags, sizeof(short int), 1, fp);
    record->tags_list = get_2D_char_array(record->no_of_tags, PASSWORD_TAG_SIZE);
    for (i = 0; i < record->no_of_tags; i++)
        fread(record->tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, fp);

    fread(&record->description_size, sizeof(short int), 1 ,fp);
    record->description = (char *)calloc(record->description_size, sizeof(char));
    fread(record->description, record->description_size*sizeof(char), 1, fp);

    end_position = ftell(fp);

    fclose(fp);

    return end_position;
}

int get_description_length(char *data)
{
    short int i;
    int length = 0;

    for (i = 0; data[i] != 0; i++)
        if (data[i] == '\t')
            length += 8;
        else
            length++;

    return length;
}

void get_line_detail(struct record_details record)
{
    short int i;
    short int tags_length[5] = {0};

    for (i = 0; i < record.no_of_tags; i++)
        tags_length[i] = strlen(record.tags_list[i]);

    if (record.no_of_tags < 4)
    {
        detail.lines_for_tags = 1;
        detail.tags_in_line_1 = record.no_of_tags;
    }
    else if (record.no_of_tags == 4)
    {
        if ((tags_length[0] + 2 + tags_length[1] + 2 + tags_length[2] + 2 + tags_length[3]) <= 50)
        {
            detail.lines_for_tags = 1;
            detail.tags_in_line_1 = 4;
        }
        else
        {
            detail.lines_for_tags = 2;
            detail.tags_in_line_1 = 3;
            detail.tags_in_line_2 = 1;
        }
    }
    else if (record.no_of_tags == 5)
    {
        if ((tags_length[0] + 2 + tags_length[1] + 2 + tags_length[2] + 2 + tags_length[3] + 2 + tags_length[4]) <= 50)
        {
            detail.lines_for_tags = 1;
            detail.tags_in_line_1 = 5;
        }
        else if ((tags_length[0] + 2 + tags_length[1] + 2 + tags_length[2] + 2 + tags_length[3] + 1) <= 50)
        {
            detail.lines_for_tags = 2;
            detail.tags_in_line_1 = 4;
            detail.tags_in_line_2 = 1;
        }
        else
        {
            detail.lines_for_tags = 2;
            detail.tags_in_line_1 = 3;
            detail.tags_in_line_2 = 2;
        }
    }

    detail.lines_for_description = ((get_description_length(record.description) - 1)/50) + 1;
}

void display_tags_of_record(char **data)
{
    short int i;

    gotoxy(top.row ,top.col);

    if (detail.lines_for_tags == 1)
    {
        printf("%-11s: ", "Tags");
        for (i = 0; i < (detail.tags_in_line_1 - 1); i++)
            printf("%s, ", data[i]);
        printf("%s", data[i]);
    }
    else
    {
        printf("%-11s: ", "Tags");
        for (i = 0; i < (detail.tags_in_line_1 - 1); i++)
            printf("%s, ", data[i]);
        printf("%s,", data[i]);

        top.row++;
        gotoxy(top.row, top.col);
        printf("%-11s: ", "Tags");
        for (i = detail.tags_in_line_1; i < (detail.tags_in_line_1 + detail.tags_in_line_2 - 1); i++)
            printf("%s, ", data[i]);
        printf("%s", data[i]);
    }

    top.row++;
}

void display_half_description(char *data)
{
    uint8_t flag = 0;
    uint8_t col = 0;
    short int i, j;

    if (get_description_length(data) > 50)
    {
        for (i = 0; data[i] != 0; i++)
        {
            if (data[i] == '\t')
            {
                for (j = 0; j < 8; j++)
                {
                    gotoxy(top.row, top.col + col);
                    putch(' ');
                    col++;

                    if (col == 47)
                    {
                        flag = 1;
                        break;
                    }
                }
            }
            else
            {
                gotoxy(top.row, top.col + col);
                putch(data[i]);
                col++;

                if (col == 47)
                {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1)
                break;
        }
        gotoxy(top.row, top.col + 47);
        printf("...");
    }
    else
        for (i = 0; data[i] != 0; i++)
            if (data[i] == '\t')
                for (j = 1; j <= 8; j++)
                    printf(" ");
            else
                putch(data[i]);
}

void display_full_record(struct record_details record)
{
    short int length;

    get_line_detail(record);
    length = 1 + 2 + detail.lines_for_tags + detail.lines_for_description + 1;

    bot.row = top.row + length - 1;
    bot.col = top.col + 67 - 1;
    print_box(3, 0, 0, 0);

    top.row++;
    top.col += 2;

    gotoxy(top.row, top.col);
    printf("%-11s: %03u", "ID", record.record_id);

    gotoxy(top.row, bot.col - 20);
    printf("Date: %s AD", record.date);
    top.row++;

    gotoxy(top.row, top.col);
    printf("%-11s: Rs. %.2f", "Amount", record.amount);

    gotoxy(top.row, bot.col - 20);
    if (record.type == 'S')
        printf("Type: Spent");
    else if (record.type == 'E')
        printf("Type: Earned");
    top.row++;

    display_tags_of_record(record.tags_list);

    gotoxy(top.row, top.col);
    printf("Description: ");
    top.col += 13;
    get_edit_display_description(record.description, 3);
    top.col -= 13;

    top.row += (detail.lines_for_description - 1 + 2);
    top.col -= 2;
}

void display_half_record(struct record_details record, const short int type, const short int no_of_lines)
{
    /*
    type:
    0 => full
    1 => top half
    2 => bottom half
    */
    short int i;

    if ((type == 1) || (type == 2))
        for (i = 0; i < no_of_lines; i++)
        {
            gotoxy(top.row + i, top.col);
            printf("%c", 179);
            gotoxy(top.row + i, top.col + 67 - 1);
            printf("%c", 179);
        }

    if ((type == 0) || (no_of_lines >= 4))
    {
        bot.row = top.row + 4 - 1;
        bot.col = top.col + 67 - 1;

        print_box(1, 0, 0, 0);

        top.row++;
        top.col += 2;

        gotoxy(top.row, top.col);
        printf("%-11s: Rs. %.2f", "Amount", record.amount);

        gotoxy(top.row, bot.col - 20);
        printf("Date: %s AD", record.date);
        top.row++;

        gotoxy(top.row, top.col);
        printf("Description: ");
        top.col += 13;
        display_half_description(record.description);
        top.col -= 13;

        top.row += 2;
        top.col -= 2;
    }
    else if (type == 1)
    {
        for (i = 0; i < 67; i++)
        {
            gotoxy(top.row, top.col + i);
            printf("%c", 196);
        }

        gotoxy(top.row, top.col);
        printf("%c", 218);
        gotoxy(top.row, top.col + 66);
        printf("%c", 191);

        top.col += 2;

        if (no_of_lines >= 2)
        {
            top.row++;

            gotoxy(top.row, top.col);
            printf("%-11s: Rs. %.2f", "Amount", record.amount);

            gotoxy(top.row, bot.col - 20);
            printf("Date: %s AD", record.date);
        }

        if (no_of_lines == 3)
        {
            top.row++;

            gotoxy(top.row, top.col);
            printf("Description: ");
            top.col += 13;
            display_half_description(record.description);
            top.col -= 13;
        }

        top.row++;
        top.col -= 2;
    }
    else if (type == 2)
    {
        for (i = 0; i < 67; i++)
        {
            gotoxy(top.row + no_of_lines - 1, top.col + i);
            printf("%c", 196);
        }

        gotoxy(top.row + no_of_lines - 1, top.col);
        printf("%c", 192);
        gotoxy(top.row + no_of_lines - 1, top.col + 67 - 1);
        printf("%c", 217);

        top.col += 2;

        if (no_of_lines == 3)
        {
            gotoxy(top.row, top.col);
            printf("%-11s: Rs. %.2f", "Amount", record.amount);

            gotoxy(top.row, bot.col - 20);
            printf("Date: %s AD", record.date);

            top.row++;
        }

        if (no_of_lines >= 2)
        {
            gotoxy(top.row, top.col);
            printf("Description: ");
            top.col += 13;
            display_half_description(record.description);
            top.col -= 13;
            top.row++;
        }

        top.row++;
        top.col -= 2;
    }
}

int tag_compare_ascending(const void *elem1, const void *elem2)
{
    char *a = *(char**)elem1;
    char *b = *(char**)elem2;

    return strcmpi(a, b);
}

int tag_compare_descending(const void *elem1, const void *elem2)
{
    char *a = *(char**)elem1;
    char *b = *(char**)elem2;

    return -strcmpi(a, b);
}

int date_compare_ascending(const void *elem1, const void *elem2)
{
    short int i;
    int ans;
    long int a = *(long int *)elem1;
    long int b = *(long int *)elem2;
    struct record_details record_a = {0};
    struct record_details record_b = {0};

    (void)read_record(a, &record_a);
    (void)read_record(b, &record_b);

    ans = strcmp(record_a.date, record_b.date);
    if (ans == 0)
        ans = record_a.record_id - record_b.record_id;

    for (i = 0; i < record_a.no_of_tags; i++)
        free(record_a.tags_list[i]);
    free(record_a.tags_list);
    free(record_a.description);

    for (i = 0; i < record_b.no_of_tags; i++)
        free(record_b.tags_list[i]);
    free(record_b.tags_list);
    free(record_b.description);

    return ans;
}

int date_compare_descending(const void *elem1, const void *elem2)
{
    short int i;
    int ans;
    long int a = *(long int *)elem1;
    long int b = *(long int *)elem2;
    struct record_details record_a = {0};
    struct record_details record_b = {0};

    (void)read_record(a, &record_a);
    (void)read_record(b, &record_b);

    ans = strcmp(record_b.date, record_a.date);
    if (ans == 0)
        ans = record_b.record_id - record_a.record_id;

    for (i = 0; i < record_a.no_of_tags; i++)
        free(record_a.tags_list[i]);
    free(record_a.tags_list);
    free(record_a.description);

    for (i = 0; i < record_b.no_of_tags; i++)
        free(record_b.tags_list[i]);
    free(record_b.tags_list);
    free(record_b.description);

    return ans;
}

int amount_compare_ascending(const void *elem1, const void *elem2)
{
    short int i;
    int ans;
    long int a = *(long int *)elem1;
    long int b = *(long int *)elem2;
    struct record_details record_a = {0};
    struct record_details record_b = {0};

    (void)read_record(a, &record_a);
    (void)read_record(b, &record_b);

    ans = record_a.amount - record_b.amount;
    if (ans == 0)
        ans = record_a.record_id - record_b.record_id;

    for (i = 0; i < record_a.no_of_tags; i++)
        free(record_a.tags_list[i]);
    free(record_a.tags_list);
    free(record_a.description);

    for (i = 0; i < record_b.no_of_tags; i++)
        free(record_b.tags_list[i]);
    free(record_b.tags_list);
    free(record_b.description);

    return ans;
}

int amount_compare_descending(const void *elem1, const void *elem2)
{
    short int i;
    int ans;
    long int a = *(long int *)elem1;
    long int b = *(long int *)elem2;
    struct record_details record_a = {0};
    struct record_details record_b = {0};

    (void)read_record(a, &record_a);
    (void)read_record(b, &record_b);

    ans = record_b.amount - record_a.amount;
    if (ans == 0)
        ans = record_b.record_id - record_a.record_id;

    for (i = 0; i < record_a.no_of_tags; i++)
        free(record_a.tags_list[i]);
    free(record_a.tags_list);
    free(record_a.description);

    for (i = 0; i < record_b.no_of_tags; i++)
        free(record_b.tags_list[i]);
    free(record_b.tags_list);
    free(record_b.description);

    return ans;
}

void get_folder_name(char *folder_name, struct user_details data)
{
    strcpy(folder_name, int_to_string(data.account_no));
    strcat(folder_name, data.username);
}

void get_data(char *data, const short int type)
{
    /*
    0 = username
    1 = password
    2 = tag
    3 = tag (for record)
    4 = record type
    5 = record id
    */
    short unsigned int count = 0, min_length, max_length;
    char ch;

    if (type == 0)
    {
        min_length = 5;
        max_length = USERNAME_SIZE - 1;
    }
    else if (type == 1)
    {
        min_length = 5;
        max_length = PASSWORD_TAG_SIZE - 1;
    }
    else if (type == 2)
    {
        min_length = 1;
        max_length = PASSWORD_TAG_SIZE - 1;
    }
    else if (type == 3)
    {
        min_length = 0;
        max_length = PASSWORD_TAG_SIZE - 1;
    }
    else if (type == 4)
    {
        min_length = max_length = 1;
    }
    else if (type == 5)
    {
        min_length = 1;
        max_length = 10;
    }

    while(1)
    {
        fseek(stdin, 0, SEEK_END);
        ch = getch();
        if (ch == '\b' && count != 0)
        {
            printf("\b \b");
            data[count] = 0;
            count--;
        }
        else if (ch == '\b' && count == 0)
            continue;
        else if (count < min_length && ch =='\r')
            continue;
        else if (ch == '\r')
        {
            data[count] = 0;
            break;
        }
        else if (count == max_length)
            continue;
        else
        {
            if (type == 1)
                putchar('*');
            else if (type == 5)
            {
                if ((ch >= '0') && (ch <= '9'))
                    putchar(ch);
                else
                    continue;
            }
            else
                putchar(ch);

            data[count] = ch;
            count++;
        }
    }
    printf("\n");
}

void get_edit_date(char *data)
{
    short unsigned int pos;
    char ch;

    if (strcmp(data, "") == 0)
        pos = 0;
    else
    {
        printf("%s", data);
        pos = 10;
    }

    while(1)
    {
        ch = getch();
        if (ch == '\b')
        {
            if (pos == 5 || pos == 8)
            {
                printf("\b\b  \b\b");
                data[pos] = 0;
                pos--;
                data[pos] = 0;
                pos--;
            }
            else if (pos == 0)
                continue;
            else
            {
                printf("\b \b");
                data[pos] = 0;
                pos--;
            }
        }
        else if (pos < 10)
        {
            if (ch >= '0' && ch <= '9')
            {
                if (pos == 3 || pos == 6)
                {
                    printf("%c-", ch);
                    data[pos] = ch;
                    pos++;
                    data[pos] = '-';
                    pos++;
                }
                else
                {
                    putch(ch);
                    data[pos] = ch;
                    pos++;
                }
            }
        }
        else if (pos == 10 && ch == 13)
        {
            data[pos] = 0;
            if (check_date(data) == 1)
                return;
        }
        continue;
    }
}

void get_edit_display_description(char *data, short int type)
{
    /*
    type:
    1 => new
    2 => edit
    3 => display
    */
    short unsigned int i = 0;
    short int row = 0, col = 0;
    char ch;

    if ((type == 2) || (type == 3))
    {
        while (data[i] != 0)
        {
            gotoxy(top.row + row, top.col + col);
            if (data[i] == '\t')
                col += 8;
            else
            {
                putch(data[i]);
                col++;
            }

            if (col > 49 && row < 2)
            {
                row++;
                col -= 50;
            }

            i++;
        }
    }

    if ((type == 1) || (type == 2))
        while (1)
        {
            fseek(stdin, 0, SEEK_END);
            gotoxy(top.row + row, top.col + col);
            ch = getch();
            if (ch == 13)
            {
                data[i]= 0;
                break;
            }
            else if (ch == '\b')
            {
                if (col == 0 && row == 0)
                    continue;

                if (data[i - 1] == '\t')
                    col -= 8;
                else
                    col--;

                if (col < 0)
                {
                    row--;
                    col = 50 - (-col);
                }

                gotoxy(top.row + row, top.col + col);
                printf(" ");
                gotoxy(top.row + row, top.col + col);

                i--;
                data[i] = 0;
                continue;
            }
            else if (ch == '\t')
            {
                if ((row < 2) || ((row == 2) && ((col + 8) <= 50)))
                {
                    col += 8;

                    if (col > 49 && row < 2)
                    {
                        row++;
                        col -= 50;
                    }

                    data[i] = ch;
                    i++;
                    gotoxy(top.row + row, top.col + col);
                }
            }
            else
            {
                if ((row < 2) || ((row == 2) && ((col + 1) <= 50)))
                {
                    putch(ch);
                    col++;

                    if (col > 49 && row < 2)
                    {
                        row++;
                        col -= 50;
                    }

                    data[i] = ch;
                    i++;
                    gotoxy(top.row + row, top.col + col);
                }
            }
            continue;
        }
}

int display_menu(char **menu_items, const int n, int choice)
{
    short unsigned int i, len, length, key;

    length = n + 2 + 2 +1;

    while (1)
    {
        system("cls");

        top.row = (25 - length)/2;
        top.col = 25;
        bot.row = top.row + length - 1;
        bot.col = 54;
        print_box(2, 3, 0, 2);

        len = strlen(menu_items[0]);
        gotoxy(top.row + 1, ((79 - len)/2));
        printf("%s", menu_items[0]);

        top.row += 3;
        top.col += 1;
        bot.row = top.row + 2;
        bot.col -= 1;

        for (i = 1; i < n; i++)
        {
            if (choice == i)
            {
                print_box(3, 0, 0, 0);

                len = strlen(menu_items[i]);
                gotoxy(top.row + 1, ((79 - len)/2));
                printf("%s", menu_items[i]);

                top.row += 3;
            }
            else
            {
                gotoxy(top.row, top.col + 2);
                printf("%s", menu_items[i]);

                top.row += 1;
            }
            bot.row = top.row + 2;
        }

        key = getkey();
        switch (key)
        {
        case UP:
            if (choice > 1)
                choice--;
            else
                choice = n - 1;
            break;
        case DOWN:
            if (choice < (n - 1))
                choice++;
            else
                choice = 1;
            break;
        case ESC:
            choice = n - 1;
            if (strcmp(menu_items[choice], "Exit") != 0)
                return choice;
            break;
        case ENTER:
            return choice;
        }
    }
}

void display_tag_id_window(char *data_1, char *data_2, const short int type)
{
    /*
    0 = add
    1 = search
    2 = edit
    3 = delete
    */
    short unsigned int length, breadth, word_size = 3;
    char heading[11] = "", word[8] = "Tag";

    if (type == 0)
    {
        length = 5;
        strcpy(heading, "ADD TAG");
    }
    else if (type == 1)
    {
        length = 5;
        strcpy(heading, "SEARCH TAG");
        word_size = 7;
        strcpy(word, "Keyword");
    }
    else if (type == 2)
    {
        length = 6;
        strcpy(heading, "EDIT TAG");
    }
    else if (type == 3)
    {
        length = 5;
        strcpy(heading, "DELETE TAG");
    }
    breadth = 2 + word_size + 2 + 15 + 2;

    system("cls");

    top.row = (25 - length)/2;
    top.col = (80 - breadth)/2;
    bot.row = top.row + length - 1;
    bot.col = top.col + breadth - 1;
    print_box(2, 3, 0, 4);

    gotoxy(top.row + 1, (80 - strlen(heading))/2);
    printf("%s", heading);

    if ((type == 0) || (type == 1) || (type == 3))
    {
        gotoxy(bot.row - 1, top.col + 2);
        printf("%s: ", word);

        get_data(data_1, 2);
    }
    else if (type == 2)
    {
        gotoxy(bot.row - 2, top.col + 2);
        printf("Old: ");
        gotoxy(bot.row - 1, top.col + 2);
        printf("New: ");

        gotoxy(bot.row - 2, top.col + 7);
        get_data(data_1, 2);
        gotoxy(bot.row - 1, top.col + 7);
        get_data(data_2, 2);
    }
}

void display_tags_list(char **tags_list, const unsigned int total_tags)
{
    short unsigned int i, key, total_pages, type, count, choice, page, option, length, tags_to_print, line_no;

    if (total_tags % 5 == 0)
        total_pages = total_tags/5;
    else
        total_pages = total_tags/5 + 1;

    page = 1;
    count = 0;
    option = 1;

    while (1)
    {
        system("cls");

        length = 2 + 2 + 2 + 1;
        if ((count + 4) < total_tags)
            tags_to_print = 5;
        else
            tags_to_print = total_tags - count;
        length += tags_to_print;

        if (total_pages == 1)
        {
            type = 0;       //Back
            length += 1;
            //line_no = length - 3 - 1;
        }
        else if (page == 1)
        {
            type = 1;       //Next & Back
            length += 2;
            //line_no = length - 3 - 2;
        }
        else if (page == total_pages)
        {
            type = 2;       //Previous & Back
            length += type;
            //line_no = length - 3 - type;
        }
        else
        {
            type = 3;       //Next, Previous & Back
            length += type;
            //line_no = length - 3 - type;
        }
        line_no = 3 + tags_to_print + 1;

        top.row = (25 - length)/2;
        top.col = (80 - 21)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 21 - 1;
        print_box(2, 3, line_no, 2);
        gotoxy(top.row + 1, 34);
        printf("List of Tags");

        top.row += 3;
        for (i = 0; i < tags_to_print; i++)
        {
            gotoxy(top.row , top.col + 2);
            printf("%s", tags_list[count + i]);
            top.row++;
        }

        top.row += 1;
        top.col += 1;
        bot.row = top.row + 2;
        bot.col -= 1;
        if (type == 0)      //Back
        {
            print_box(3, 0, 0, 0);

            gotoxy(top.row + 1, 37);
            printf("Back");

            key = getkey();
            if (key == 13)
                choice = 3;     //Back
        }
        else if (type == 1)     //Next & Back
        {
            if (option == 1)
            {
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 35);
                printf("Next Page");

                gotoxy(top.row + 3, top.col + 2);
                printf("Back");
            }
            else if (option == 2)
            {
                gotoxy(top.row, top.col + 2);
                printf("Next Page");

                top.row++;
                bot.row = top.row + 2;
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 37);
                printf("Back");
            }

            key = getkey();
            if ((key == UP) || (key == DOWN))
            {
                if (option == 1)
                    option = 2;
                else if (option == 2)
                    option = 1;
            }
            else if (key == ESC)
                choice = 3;        //Back
            else if (key == ENTER)
            {
                if (option == 1)
                    choice = 1;     //Next
                else if (option == 2)
                    choice = 3;     //Back
            }
        }
        else if (type == 2)     //Previous & Back
        {
            if (option == 1)
            {
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 33);
                printf("Previous Page");

                gotoxy(top.row + 3, top.col + 2);
                printf("Back");
            }
            else if (option == 2)
            {
                gotoxy(top.row, top.col + 2);
                printf("Previous Page");

                top.row++;
                bot.row = top.row + 2;
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 37);
                printf("Back");
            }

            key = getkey();
            if ((key == UP) || (key == DOWN))
            {
                if (option == 1)
                    option = 2;
                else if (option == 2)
                    option = 1;
            }
            else if (key == ESC)
                choice = 3;        //Back
            else if (key == ENTER)
            {
                if (option == 1)
                    choice = 2;     //Previous
                else if (option == 2)
                    choice = 3;     //Back
            }
        }
        else if (type == 3)
        {
            if (option == 1)
            {
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 35);
                printf("Next Page");

                gotoxy(top.row + 3, top.col + 2);
                printf("Previous Page");
                gotoxy(top.row + 4, top.col + 2);
                printf("Back");
            }
            else if (option == 2)
            {
                gotoxy(top.row, top.col + 2);
                printf("Next Page");

                top.row++;
                bot.row = top.row + 2;
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 33);
                printf("Previous Page");

                gotoxy(top.row + 3, top.col + 2);
                printf("Back");
            }
            else if (option == 3)
            {
                gotoxy(top.row, top.col + 2);
                printf("Next Page");

                gotoxy(top.row + 1, top.col + 2);
                printf("Previous Page");

                top.row += 2;
                bot.row = top.row + 2;
                print_box(3, 0, 0, 0);
                gotoxy(top.row + 1, 37);
                printf("Back");
            }

            key = getkey();
            switch (key)
            {
            case UP:
                if (option == 1)
                    option = 3;
                else
                    option--;
                break;
            case DOWN:
                if (option == 3)
                    option = 1;
                else
                    option++;
                break;
            case ESC:
                choice = 3;
                break;
            case ENTER:
                choice = option;
                break;
            }
        }

        if (choice == 1)        //Next
        {
            page++;
            count += 5;

            choice = 0;
            option = 1;
        }
        else if (choice == 2)       //Previous
        {
            page--;
            count -= 5;

            choice = 0;
            option = 1;
        }
        else if (choice == 3)       //Back
            return;
    }
}

int display_record_id_window(const short int type)
{
    /*
    type
    1 = edit record
    2 = delete record
    */
    unsigned short int len;
    char data[11] = "";
    char heading[15] = "";

    if (type == 1)
        strcpy(heading, "EDIT RECORD");
    else if (type == 2)
        strcpy(heading, "DELETE RECORD");

    len = strlen(heading);

    top.row = (25 - 5)/2;
    top.col = (80 - 20)/2;
    bot.row = top.row + 5 - 1;
    bot.col = top.col + 18 - 1;

    system("cls");
    print_box(2, 3, 0, 4);

    top.row++;
    gotoxy(top.row, (80 - len)/2);
    printf("%s", heading);

    top.row += 2;
    top.col += 2;

    gotoxy(top.row, top.col);
    printf("ID: ");

    top.col += 4;
    gotoxy(top.row, top.col);

    get_data(data, 5);

    return ((unsigned int)string_to_int(data));
}

int diplay_a_record(const struct record_details data)
{
    uint8_t temp_col;
    short int length, choice = 2;
    int key;

    get_line_detail(data);

    length = 3 + 4 + detail.lines_for_tags + detail.lines_for_description + 1 + 3 + 1;

    while (1)
    {
        system("cls");

        top.row = (25 - length)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 67 - 1;

        print_box(2, 3, length - 4, 2);

        top.row++;
        gotoxy(top.row, (80 - strlen("RECORD DETAIL"))/2);
        printf("RECORD DETAIL");
        top.row += 2;
        top.col += 2;

        gotoxy(top.row, top.col);
        printf("%-11s: %03u", "ID", data.record_id);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: %s", "Date", data.date);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: Rs. %.2f", "Amount", data.amount);
        top.row++;

        gotoxy(top.row, top.col);
        if (data.type == 'S')
            printf("%-11s: Spent", "Type");
        else if (data.type == 'E')
            printf("%-11s: Earned", "Type");
        top.row++;

        display_tags_of_record(data.tags_list);

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Description");
        temp_col = top.col;
        top.col += 13;
        get_edit_display_description(data.description, 3);
        top.col = temp_col;

        top.row = bot.row - 3;
        bot.row--;

        gotoxy(top.row + 1, 18);
        printf("%s", "Continue");
        gotoxy(top.row + 1, 53);
        printf("%s", "Cancel");

        if (choice == 1)
            top.col = 7;
        else if (choice == 2)
            top.col = 40;
        bot.col = top.col + 31;

        print_box(3, 0, 0, 0);

        gotoxy(24, 79);
        key = getkey();

        if ((key == LEFT) || (key == RIGHT))
        {
            if (choice == 1)
                choice = 2;
            else if (choice == 2)
                choice = 1;
        }
        else if (key == ESC)
            return 2;
        else if (key == ENTER)
            return choice;
    }
}

void edit_record_window(struct record_details *record)
{
    enum turn_detail
    {
        date, amount, type, tags, description
    } turn = date;

    FILE *f_tags = NULL;
    uint8_t flag_search_tag;
    short int i;
    short int length;
    struct position temp = {0};
    struct record_details dummy = {0};
    char tag[PASSWORD_TAG_SIZE] = "";
    char dummy_tag[PASSWORD_TAG_SIZE] = "";
    char dummy_description[151] = "";
    char temp_tags_list[5][PASSWORD_TAG_SIZE] = {""};

    f_tags = fopen("tags.dat", "rb");
    if (f_tags == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    dummy.record_id = record->record_id;
    strcpy(dummy.date, record->date);
    dummy.amount = record->amount;
    dummy.type = record->type;
    dummy.no_of_tags = record->no_of_tags;
    dummy.tags_list = get_2D_char_array(dummy.no_of_tags, PASSWORD_TAG_SIZE);
    for (i = 0; i < dummy.no_of_tags; i++)
        strcpy(dummy.tags_list[i], record->tags_list[i]);
    dummy.description_size = record->description_size;
    dummy.description = (char *)calloc(dummy.description_size, sizeof(char));
    strcpy(dummy.description, record->description);

    for(i = 0; i < record->no_of_tags; i++)
        free(record->tags_list[i]);
    free(record->tags_list);
    free(record->description);

    get_line_detail(dummy);

    length = 3 + 4 + detail.lines_for_tags + detail.lines_for_description + 1;


    while (turn != tags)
    {
        top.row = (25 - length)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 67 - 1;

        system("cls");
        print_box(2, 3, 0, 4);

        gotoxy(top.row + 1, (80 - strlen("EDIT RECORD"))/2);
        printf("EDIT RECORD");
        top.row += 3;
        top.col += 2;

        gotoxy(top.row, top.col);
        printf("%-11s: %03u", "ID", dummy.record_id);
        top.row++;

        temp.row = top.row;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Date");
        if (turn != date)
            printf("%s AD", dummy.date);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Amount");
        if (turn != amount)
            printf("Rs. %.2f", dummy.amount);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Type");
        if (turn != type)
        {
            if (dummy.type == 'S')
                printf("%s", "Spent");
            else if (dummy.type == 'E')
                printf("%s", "Earned");
        }
        top.row++;

        display_tags_of_record(dummy.tags_list);

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Description");
        temp.col = top.col;
        top.col += 13;
        get_edit_display_description(dummy.description, 3);

        top.row = temp.row;

        if (turn == date)
        {
            gotoxy(top.row, top.col);
            get_edit_date(dummy.date);

            turn = amount;
        }
        else if (turn == amount)
        {
            gotoxy(top.row + 1, top.col);
            printf("Rs. ");
            dummy.amount = -1;
            do
            {
                for (i = 0; i < 46; i++)
                {
                    gotoxy(top.row + 1, top.col + 4 + i);
                    printf(" ");
                }
                gotoxy(top.row + 1, top.col + 4);
                scanf("%f", &dummy.amount);
            }while(dummy.amount < 0);

            turn = type;
        }
        else if (turn == type)
        {
            gotoxy(top.row + 2, top.col);
            while (1)
            {
                get_data(&dummy.type, 4);
                dummy.type = char_upper(dummy.type);

                gotoxy(top.row + 2, top.col + 1);
                if (dummy.type == 'S')
                    printf("\bSpent");          //printf("pent");
                else if (dummy.type == 'E')
                    printf("\bEarned");         //printf("arned");

                if (dummy.type != 'S' && dummy.type != 'E')
                    printf("\b \b");
                else
                    break;
            }

            turn = tags;
        }

        top.col = temp.col;
    }

    for (i = 0; i < dummy.no_of_tags; i++)
        free(dummy.tags_list[i]);
    free(dummy.tags_list);
    dummy.no_of_tags = 0;

    length = 3 + 4 + 1 + detail.lines_for_description + 1;

    while (turn != description)
    {
        system("cls");

        top.row = (25 - length)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 67 - 1;
        print_box(2, 3, 0, 4);

        gotoxy(top.row + 1, (80 - strlen("EDIT RECORD"))/2);
        printf("EDIT RECORD");

        top.row += 3;
        top.col += 2;

        gotoxy(top.row, top.col);
        printf("%-11s: %03u", "ID", dummy.record_id);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: %s", "Date", dummy.date);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: Rs. %.2f", "Amount", dummy.amount);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Type");
        if (dummy.type == 'S')
            printf("%s", "Spent");
        else if (dummy.type == 'E')
            printf("%s", "Earned");
        top.row++;

        temp.row = top.row;

        for (i = 0; (i <= dummy.no_of_tags) && (i < 5); i++)
        {
            gotoxy(top.row, top.col);
            printf("%-11s: ", "Tag");
            top.row++;
        }

        gotoxy(top.row, top.col);
        printf("Description: ");
        top.col += 13;
        get_edit_display_description(dummy.description, 3);

        top.row = temp.row;

        for (i = 0; i < dummy.no_of_tags; i++)
        {
            gotoxy(top.row, top.col);
            printf("%s", temp_tags_list[i]);
            top.row++;
        }
        if (turn == tags)
        {
            gotoxy(top.row, top.col);
            fseek(stdin, 0, SEEK_END);
            get_data(dummy_tag, 3);
            if (strlen(dummy_tag) > 0)     //"enter" key matra enter nagareko OR non-empty tag input gareko
            {
                flag_search_tag = 0;
                fseek(f_tags, 0, SEEK_SET);
                while (fread(tag, sizeof(tag), 1, f_tags) == 1)
                    if (strcmpi(dummy_tag, tag) == 0)
                    {
                        flag_search_tag = 1;
                        break;
                    }
                for (i = 0; i < dummy.no_of_tags; i++)
                    if (strcmpi(temp_tags_list[i], dummy_tag) == 0)
                    {
                        flag_search_tag = 0;
                        break;
                    }

                if (flag_search_tag == 1)
                {
                    dummy.no_of_tags++;
                    strcpy(temp_tags_list[dummy.no_of_tags - 1], tag);
                    gotoxy(top.row, top.col);
                    printf("%s", tag);

                    if (dummy.no_of_tags < 5)
                    {
                        length++;
                        continue;
                    }
                    else
                        turn = description;
                }
                else
                    continue;
            }
            else if ((strlen(dummy_tag) <= 0) && (dummy.no_of_tags == 0))
            {
                dummy.no_of_tags++;
                strcpy(temp_tags_list[dummy.no_of_tags - 1], "Others");
                gotoxy(top.row, top.col);
                printf("Others");
                turn = description;
            }
            else
                turn = description;
        }
        dummy.tags_list = get_2D_char_array(dummy.no_of_tags, PASSWORD_TAG_SIZE);
        for (i = 0; i < dummy.no_of_tags; i++)
            strcpy(dummy.tags_list[i], temp_tags_list[i]);
        top.row++;
    }

    strcpy(dummy_description, dummy.description);
    free(dummy.description);
    dummy.description_size = 0;

    get_line_detail(dummy);

    length = 3 + 4 + detail.lines_for_tags + 3 + 1;

    system("cls");

    top.row = (25 - length)/2;
    top.col = (80 - 67)/2;
    bot.row = top.row + length - 1;
    bot.col = top.col + 67 - 1;
    print_box(2, 3, 0, 4);

    gotoxy(top.row + 1, (80 - strlen("EDIT RECORD"))/2);
    printf("EDIT RECORD");

    top.row += 3;
    top.col += 2;

    gotoxy(top.row, top.col);
    printf("%-11s: %03u", "ID", dummy.record_id);
    top.row++;

    gotoxy(top.row, top.col);
    printf("%-11s: %s", "Date", dummy.date);
    top.row++;

    gotoxy(top.row, top.col);
    printf("%-11s: Rs. %.2f", "Amount", dummy.amount);
    top.row++;

    gotoxy(top.row, top.col);
    printf("%-11s: ", "Type");
    if (dummy.type == 'S')
        printf("%s", "Spent");
    else if (dummy.type == 'E')
        printf("%s", "Earned");
    top.row++;

    display_tags_of_record(dummy.tags_list);

    gotoxy(top.row, top.col);
    printf("%-11s: ", "Description");
    top.col += 13;
    get_edit_display_description(dummy_description, 2);

    dummy.description_size = strlen(dummy_description) + 1;
    dummy.description = (char *)calloc(dummy.description_size, sizeof(char));
    strcpy(dummy.description, dummy_description);

    record->record_id = dummy.record_id;
    strcpy(record->date, dummy.date);
    record->amount = dummy.amount;
    record->type = dummy.type;
    record->no_of_tags = dummy.no_of_tags;
    record->tags_list = get_2D_char_array(record->no_of_tags, PASSWORD_TAG_SIZE);
    for (i = 0; i < dummy.no_of_tags; i++)
        strcpy(record->tags_list[i], dummy.tags_list[i]);
    record->description_size = dummy.description_size;
    record->description = (char *)calloc(record->description_size, sizeof(char));
    strcpy(record->description, dummy.description);

    fclose(f_tags);
}

int display_records_list(const long int *data, const unsigned int no_of_records)
{
    unsigned short int i, type, length;
    unsigned int record_to_display;
    int key;
    long int temp;
    struct record_details record[3] = {{0}};

    if (no_of_records == 0)
    {
        top.row = (25 - 18)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + 18 - 1;
        bot.col = top.col + 67 - 1;

        system("cls");
        print_box(2, 3, 0, 2);

        gotoxy(top.row + 1, (80 - strlen("REDORD LIST"))/2);
        printf("RECORD LIST");

        top.row += 3;

        gotoxy(top.row + 7 - 1, (80 - strlen("No records to display"))/2);
        printf("No records to display");

        while (getkey() != ESC);
    }
    else if (no_of_records == 1)
    {
        temp = read_record(data[0], &record[0]);
        if (temp == -1)
        {
            print_detail("PROCESS FAILED");
            return 0;
        }

        top.row = (25 - 18)/2;
        top.col = (80 - 69)/2;
        bot.row = top.row + 18 - 1;
        bot.col = top.col + 69 - 1;

        system("cls");
        print_box(2, 3, 0, 2);

        gotoxy(top.row + 1, (80 - strlen("REDORD LIST"))/2);
        printf("RECORD LIST");

        top.row += 3;
        top.col = (80 - 67)/2;

        display_full_record(record[0]);

        while (getkey() != ESC);

        for (i = 0; i < record[0].no_of_tags; i++)
            free(record[0].tags_list[i]);
        free(record[0].tags_list);
        free(record[0].description);
    }
    else if (no_of_records == 2)
    {
        temp = read_record(data[0], &record[0]);
        if (temp == -1)
        {
            print_detail("PROCESS FAILED");
            return 0;
        }

        temp = read_record(data[1], &record[1]);
        if (temp == -1)
        {
            print_detail("PROCESS FAILED");
            return 0;
        }

        type = 0;
        while (1)
        {
            top.row = (25 - 18)/2;
            top.col = (80 - 69)/2;
            bot.row = top.row + 18 - 1;
            bot.col = top.col + 69 - 1;

            system("cls");
            print_box(2, 3, 0, 2);

            gotoxy(top.row + 1, (80 - strlen("REDORD LIST"))/2);
            printf("RECORD LIST");

            top.row += 3;
            top.col = (80 - 67)/2;

            if (type == 0)
            {
                display_full_record(record[0]);
                display_half_record(record[1], 0, 0);
            }
            else if (type == 1)
            {
                display_half_record(record[0], 0, 0);
                display_full_record(record[1]);
            }

            do
            {
                key = getkey();
                if ((key == UP) || (key == DOWN))
                {
                    if (type == 1)
                        type = 0;
                    else if (type == 0)
                        type = 1;

                    break;
                }
            }while (key != ESC);

            if (key == ESC)
                break;
        }
    }
    else if (no_of_records >= 3)
    {
        record_to_display = 0;

        while (1)
        {
            if (record_to_display == 0)
            {
                type = 1;

                for (i = 0; i < 3; i++)
                {
                    temp = read_record(data[record_to_display + i], &record[i]);
                    if (temp == -1)
                    {
                        print_detail("PROCESS FAILED");
                        return 0;
                    }
                }
            }
            else if (record_to_display == (no_of_records - 1))
            {
                type = 3;

                for (i = 0; i < 3; i++)
                {
                    temp = read_record(data[record_to_display + i - 2], &record[i]);
                    if (temp == -1)
                    {
                        print_detail("PROCESS FAILED");
                        return 0;
                    }
                }
            }
            else
            {
                type = 2;

                for (i = 0; i < 3; i++)
                {
                    temp = read_record(data[record_to_display + i - 1], &record[i]);
                    if (temp == -1)
                    {
                        print_detail("PROCESS FAILED");
                        return 0;
                    }
                }
            }

            top.row = (25 - 18)/2;
            top.col = (80 - 69)/2;
            bot.row = top.row + 18 - 1;
            bot.col = top.col + 69 - 1;

            system("cls");
            print_box(2, 3, 0, 2);

            gotoxy(top.row + 1, (80 - strlen("REDORD LIST"))/2);
            printf("RECORD LIST");

            top.row += 3;
            top.col = (80 - 67)/2;

            if (type == 1)
            {
                get_line_detail(record[0]);
                length = 1 + 2 + detail.lines_for_tags + detail.lines_for_description + 1;
                display_full_record(record[0]);

                display_half_record(record[1], 0, 0);
                length += 4;

                display_half_record(record[2], 1, 14 - length);
            }
            else if (type == 2)
            {
                get_line_detail(record[1]);
                length = 1 + 2 + detail.lines_for_tags + detail.lines_for_description + 1;

                if (length == 6)
                {
                    display_half_record(record[0], 0, 0);
                    display_full_record(record[1]);
                    display_half_record(record[2], 0, 0);
                }
                else if (length == 7)
                {
                    display_half_record(record[0], 2, 3);
                    display_full_record(record[1]);
                    display_half_record(record[2], 0, 0);
                }
                else if (length == 8)
                {
                    display_half_record(record[0], 2, 3);
                    display_full_record(record[1]);
                    display_half_record(record[2], 1, 3);
                }
                else if (length == 9)
                {
                    display_half_record(record[0], 2, 2);
                    display_full_record(record[1]);
                    display_half_record(record[2], 1, 3);
                }
            }
            else if (type == 3)
            {
                get_line_detail(record[2]);
                length = 1 + 2 + detail.lines_for_tags + detail.lines_for_description + 1;

                display_half_record(record[0], 2, 14 - (length + 4));
                display_half_record(record[1], 0, 0);
                display_full_record(record[2]);
            }

            do
            {
                key = getkey();
                if (key == UP)
                {
                    if (record_to_display == 0)
                        record_to_display = no_of_records - 1;
                    else
                        record_to_display--;

                    break;
                }
                else if (key == DOWN)
                {
                    if (record_to_display == (no_of_records - 1))
                        record_to_display = 0;
                    else
                        record_to_display++;

                    break;
                }
            }while (key != ESC);

            if (key == ESC)
                break;
        }
    }

    return 1;
}

void main_menu()
{
    short unsigned int i, choice;
    char **main_menu_items = NULL;

    main_menu_items = get_2D_char_array(4, 20);
    strcpy(main_menu_items[0], "MAIN MENU");
    strcpy(main_menu_items[1], "Login");
    strcpy(main_menu_items[2], "Create Account");
    strcpy(main_menu_items[3], "Exit");

    choice = 1;
    while (1)
    {
        choice = display_menu(main_menu_items, 4, choice);

        switch (choice)
        {
        case 1:
            login();
            del_acc_flag = 0;
            break;
        case 2:
            creat_account();
            break;
        case 3:
            for (i = 0; i < 4; i++)
                free(main_menu_items[i]);
            free(main_menu_items);
            print_detail("PROGRAM CLOSED");

            gotoxy(25, 0);
            exit(EXIT_SUCCESS);
        }
    }
}

void login()
{
    FILE *f_index = NULL, *fp = NULL;
    short unsigned int flag_username, flag_password;
    char folder_name[FOLDER_NAME_SIZE] = {0};
    struct user_details data = {0};

    if (total_accounts == 0)
        print_detail("NO ACCOUNT FOUND");
    else
    {
        f_index = fopen("index.dat", "rb");
        if (f_index == NULL)
        {
            print_detail("PROCESS FAILED");
            return;
        }

        top.row = 7;
        top.col = 18;
        bot.row = 12;
        bot.col = 61;
        system("cls");
        print_box(2, 3, 0, 4);
        gotoxy(8, 37);
        printf("LOGIN");

        gotoxy(10, 20);
        printf("Username: ");
        gotoxy(11, 20);
        printf("Password: ");

        gotoxy(10, 30);
        get_data(data.username, 0);
        gotoxy(11, 30);
        get_data(data.password, 1);

        flag_username = 0;
        flag_password = 0;
        fseek(f_index, 0, SEEK_SET);
        while (fread(&loged_in_as.account_no, sizeof(loged_in_as.account_no), 1, f_index) == 1)
        {
            fread(loged_in_as.username, sizeof(loged_in_as.username), 1, f_index);
            if (strcmp(data.username, loged_in_as.username) == 0)
            {
                flag_username = 1;

                get_folder_name(folder_name, loged_in_as);
                chdir(folder_name);     //personal folderma gayeko

                fp = fopen("setting.dat", "rb");
                fread(loged_in_as.password,sizeof(loged_in_as.password), 1, fp);
                fclose(fp);

                if (strcmp(data.password, loged_in_as.password) == 0)
                {
                    flag_password = 1;

                    print_detail("LOGIN SUCESSFULL");

                    fclose(f_index);

                    login_menu();

                    if (del_acc_flag == 0)
                        chdir("..");        //"Users" folderma aayeko

                    break;
                }
            }
        }
        if (flag_username == 0)
        {
            print_detail("INTRUDER");
            fclose(f_index);
        }
        else if (flag_username == 1 && flag_password == 0)
        {
            chdir("..");        //"Users" folderma aayeko
            print_detail("INTRUDER");
            fclose(f_index);
        }
    }
}

void login_menu()           //login_menu kholda jasle login garyo tesko folder vitra hunxa
{
    uint8_t flag_logout = 0;
    short unsigned int i, choice;
    char **login_menu_items = NULL;

    login_menu_items = get_2D_char_array(5, 20);
    strcpy(login_menu_items[0], "MENU");
    strcpy(login_menu_items[1], "Records");
    strcpy(login_menu_items[2], "Tags");
    strcpy(login_menu_items[3], "Settings");
    strcpy(login_menu_items[4], "Logout");

    choice = 1;
    do
    {
        choice = display_menu(login_menu_items, 5, choice);

        switch (choice)
        {
        case 1:
            records_menu();
            break;
        case 2:
            tags_menu();
            break;
        case 3:
            settings_menu();
            if (del_acc_flag == 1)
            {
                for (i = 0; i < 5; i++)
                    free(login_menu_items[i]);
                free(login_menu_items);
                flag_logout = 1;
            }
            break;
        case 4:
            if (get_conformation("CONFORM LOGOUT") == 1)
            {
                for (i = 0; i < 5; i++)
                    free(login_menu_items[i]);
                free(login_menu_items);
                flag_logout = 1;
                print_detail("LOGED OUT");
            }
            else
                print_detail("LOGOUT CANCELLED");
            break;
        }
    }while (flag_logout == 0);
}

void records_menu()
{
    short unsigned int i, choice;
    char **records_menu_items = NULL;

    records_menu_items = get_2D_char_array(7, 15);
    strcpy(records_menu_items[0], "RECORDS");
    strcpy(records_menu_items[1], "Add Records");
    strcpy(records_menu_items[2], "List Records");
    strcpy(records_menu_items[3], "Filter Records");
    strcpy(records_menu_items[4], "Edit Records");
    strcpy(records_menu_items[5], "Delete Records");
    strcpy(records_menu_items[6], "Back");

    choice = 1;
    do
    {
        choice = display_menu(records_menu_items, 7, choice);

        switch (choice)
        {
        case 1:
            add_record();
            break;
        case 2:
            list_records();
            break;
        case 3:
            filter_records();
            break;
        case 4:
            edit_record();
            break;
        case 5:
            delete_record();
            break;
        case 6:
            for (i = 0; i < 7; i++)
                free(records_menu_items[i]);
            free(records_menu_items);
            break;
        }
    }while (choice != 6);
}

void add_record()
{
    FILE *f_old, *f_new, *f_tags;
    uint8_t flag_search_tag;
    uint8_t flag_record_id = 0, flag_date = 0, flag_amount = 0, flag_type = 0, flag_tag = 0;
    short unsigned int i, length, temp;
    unsigned int new_record_id = 1;
    char dummy_tag[PASSWORD_TAG_SIZE], tag[PASSWORD_TAG_SIZE];
    char dummy_description[151] = "";
    char temp_tags_list[5][PASSWORD_TAG_SIZE] = {""};
    struct record_details record = {0};
    struct record_details dummy = {0};

    f_old = fopen("records.dat", "rb");
    if (f_old == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    f_tags = fopen("tags.dat", "rb");
    if (f_tags == NULL)
    {
        print_detail("PROCESS FAILED");
        fclose(f_old);
        return;
    }

    while (fread(&dummy.record_id, sizeof(unsigned int), 1, f_old) == 1)
    {
        if (new_record_id <= dummy.record_id)
            new_record_id = dummy.record_id + 1;

        fread(dummy.date, sizeof(dummy.date), 1, f_old);
        fread(&dummy.amount, sizeof(float), 1, f_old);
        fread(&dummy.type, sizeof(char), 1, f_old);

        fread(&dummy.no_of_tags, sizeof(short int), 1, f_old);
        dummy.tags_list = get_2D_char_array(dummy.no_of_tags, PASSWORD_TAG_SIZE);
        for (i = 0; i < dummy.no_of_tags; i++)
            fread(dummy.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_old);

        fread(&dummy.description_size, sizeof(short int), 1, f_old);
        dummy.description = malloc(dummy.description_size*sizeof(char));
        fread(dummy.description, dummy.description_size*sizeof(char), 1, f_old);

        for (i = 0; i < dummy.no_of_tags; i++)
            free(dummy.tags_list[i]);
        free(dummy.tags_list);
        free(dummy.description);
    }

    length = 12;
    record.no_of_tags = 0;
    while (1)
    {
        system("cls");

        top.row = (25 - length)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 67 - 1;
        print_box(2, 3, 0, 2);

        gotoxy(top.row + 1, (80 - 10)/2);
        printf("Add Record");

        top.row += 3;
        top.col += 2;
        temp = top.row;

        gotoxy(top.row, top.col);
        printf("%-11s:", "ID");
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Date");
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: Rs. ", "Amount");
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Type [S/E]");
        top.row++;

        for (i = 1; (i <= record.no_of_tags + 1) && (i <= 5); i++)
        {
            gotoxy(top.row, top.col);
            printf("%-11s: ", "Tag");
            top.row++;
        }

        gotoxy(top.row, top.col);
        printf("Description: ");

        top.row = temp;
        top.col += 13;

        gotoxy(top.row, top.col);
        if (flag_record_id == 0)
        {
            record.record_id = new_record_id;
            flag_record_id = 1;
        }
        printf("%03u", record.record_id);
        top.row++;

        gotoxy(top.row, top.col);
        if (flag_date == 1)
            printf("%s AD", record.date);
        else
        {
            get_system_date(record.date);
            get_edit_date(record.date);
            flag_date = 1;
        }
        top.row++;

        gotoxy(top.row, top.col + 4);
        if (flag_amount == 1)
            printf("%.2f", record.amount);
        else
        {
            record.amount = -1;
            do
            {
                for (i = 0; i < 46; i++)
                {
                    gotoxy(top.row, top.col + 4 + i);
                    printf(" ");
                }
                gotoxy(top.row, top.col + 4);
                scanf("%f", &record.amount);
            }while(record.amount < 0);
            flag_amount = 1;
        }
        top.row++;

        gotoxy(top.row, top.col);
        if (flag_type == 1)
        {
            if (record.type == 'S')
                printf("Spent");
            else if (record.type == 'E')
                printf("Earned");
        }
        else
        {
            while (1)
            {
                get_data(&record.type, 4);
                record.type = char_upper(record.type);

                gotoxy(top.row, top.col + 1);
                if (record.type == 'S')
                    printf("\bSpent");      //printf("pent");
                else if (record.type == 'E')
                    printf("\bEarned");     //printf("arned");

                if (record.type != 'S' && record.type != 'E')
                    printf("\b \b");
                else
                    break;
            }
            flag_type = 1;
        }
        top.row++;

        for (i = 0; i < record.no_of_tags; i++)
        {
            gotoxy(top.row, top.col);
            printf("%s", temp_tags_list[i]);
            top.row++;
        }
        if (flag_tag == 0)
        {
            gotoxy(top.row, top.col);
            fseek(stdin, 0, SEEK_END);
            get_data(dummy_tag, 3);
            if (strlen(dummy_tag) > 0)     //"enter" key matra enter nagareko OR non-empty tag input gareko
            {
                flag_search_tag = 0;
                fseek(f_tags, 0, SEEK_SET);
                while (fread(tag, sizeof(tag), 1, f_tags) == 1)
                    if (strcmpi(dummy_tag, tag) == 0)
                    {
                        flag_search_tag = 1;
                        break;
                    }
                for (i = 0; i < record.no_of_tags; i++)
                    if (strcmpi(temp_tags_list[i], dummy_tag) == 0)
                    {
                        flag_search_tag = 0;
                        break;
                    }

                if (flag_search_tag == 1)
                {
                    record.no_of_tags++;
                    strcpy(temp_tags_list[record.no_of_tags - 1], tag);
                    gotoxy(top.row, top.col);
                    printf("%s", tag);

                    if (record.no_of_tags < 5)
                    {
                        length++;
                        continue;
                    }
                    else
                        flag_tag = 1;
                }
                else
                    continue;
            }
            else if ((strlen(dummy_tag) <= 0) && (record.no_of_tags == 0))
            {
                record.no_of_tags++;
                strcpy(temp_tags_list[record.no_of_tags - 1], "Others");
                gotoxy(top.row, top.col);
                printf("Others");
                flag_tag = 1;
            }
            else
                flag_tag = 1;
        }
        record.tags_list = get_2D_char_array(record.no_of_tags, PASSWORD_TAG_SIZE);
        for (i = 0; i < record.no_of_tags; i++)
            strcpy(record.tags_list[i], temp_tags_list[i]);
        top.row++;

        gotoxy(top.row, top.col);
        get_edit_display_description(dummy_description, 1);
        record.description_size = strlen(dummy_description) + 1;
        record.description = (char *)calloc(record.description_size, sizeof(char));
        strcpy(record.description, dummy_description);

        break;
    }

    f_new = fopen("temp.dat", "wb");
    if (f_new == NULL)
    {
        print_detail("PROCESS FAILED");
        fclose(f_old);
        fclose(f_tags);
        return;
    }

    fwrite(&record.record_id, sizeof(unsigned int), 1, f_new);
    fwrite(record.date, sizeof(record.date), 1, f_new);
    fwrite(&record.amount, sizeof(float), 1, f_new);
    fwrite(&record.type, sizeof(char), 1, f_new);
    fwrite(&record.no_of_tags, sizeof(short int), 1, f_new);
    for (i = 0; i < record.no_of_tags; i++)
        fwrite(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_new);
    fwrite(&record.description_size, sizeof(short int), 1, f_new);
    fwrite(record.description, record.description_size*sizeof(char), 1, f_new);

    for (i = 0; i < record.no_of_tags; i++)
        free(record.tags_list[i]);
    free(record.tags_list);
    free(record.description);

    fseek(f_old, 0, SEEK_SET);
    while (fread(&dummy.record_id, sizeof(unsigned int), 1, f_old) == 1)
    {
        fread(dummy.date, sizeof(dummy.date), 1, f_old);
        fread(&dummy.amount, sizeof(float), 1, f_old);
        fread(&dummy.type, sizeof(char), 1, f_old);

        fread(&dummy.no_of_tags, sizeof(short int), 1, f_old);
        dummy.tags_list = get_2D_char_array(dummy.no_of_tags, PASSWORD_TAG_SIZE);
        for (i = 0; i < dummy.no_of_tags; i++)
            fread(dummy.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_old);

        fread(&dummy.description_size, sizeof(short int), 1, f_old);
        dummy.description = malloc(dummy.description_size*sizeof(char));
        fread(dummy.description, dummy.description_size*sizeof(char), 1, f_old);

        fwrite(&dummy.record_id, sizeof(unsigned int), 1, f_new);
        fwrite(dummy.date, sizeof(dummy.date), 1, f_new);
        fwrite(&dummy.amount, sizeof(float), 1, f_new);
        fwrite(&dummy.type, sizeof(char), 1, f_new);
        fwrite(&dummy.no_of_tags, sizeof(short int), 1, f_new);
        for (i = 0; i < dummy.no_of_tags; i++)
            fwrite(dummy.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_new);
        fwrite(&dummy.description_size, sizeof(short int), 1, f_new);
        fwrite(dummy.description, dummy.description_size*sizeof(char), 1, f_new);

        for (i = 0; i < dummy.no_of_tags; i++)
            free(dummy.tags_list[i]);
        free(dummy.tags_list);
        free(dummy.description);
    }

    fclose(f_old);
    fclose(f_new);

    remove("records.dat");
    rename("temp.dat", "records.dat");

    print_detail("RECORD ADDED");

    fclose(f_tags);
}

void list_records()
{
    FILE *fp;
    short int choice;
    unsigned int i, total_records;
    long int *pos_list = NULL;
    long int *temp_pos_list = NULL;
    long int temp_pos;
    char **records_list_menu_items = NULL;
    struct record_details record = {0};

    fp = fopen("records.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    total_records = 0;
    temp_pos = ftell(fp);
    while (fread(&record.record_id, sizeof(unsigned int), 1, fp) == 1)
    {
        total_records++;
        i = total_records - 1;
        pos_list = (long int *)realloc(pos_list, total_records*sizeof(long int));
        pos_list[i] = temp_pos;

        fread(record.date, sizeof(record.date), 1, fp);
        fread(&record.amount, sizeof(float), 1, fp);
        fread(&record.type, sizeof(char), 1, fp);

        fread(&record.no_of_tags, sizeof(short int), 1, fp);
        record.tags_list = get_2D_char_array(record.no_of_tags, PASSWORD_TAG_SIZE);
        for (i = 0; i < record.no_of_tags; i++)
            fread(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, fp);

        fread(&record.description_size, sizeof(short int), 1, fp);
        record.description = (char *)calloc(record.description_size, sizeof(char));
        fread(record.description, record.description_size*sizeof(char), 1, fp);

        temp_pos = ftell(fp);

        for (i = 0; i < record.no_of_tags; i++)
            free(record.tags_list[i]);
        free(record.tags_list);
        free(record.description);
    }

    if (total_records == 0)
    {
        print_detail("NO RECORDS FOUND");
        fclose(fp);
        return;
    }

    records_list_menu_items = get_2D_char_array(8, 20);
    strcpy(records_list_menu_items[0], "LIST RECORDS");
    strcpy(records_list_menu_items[1], "ID, Descending");
    strcpy(records_list_menu_items[2], "ID, Ascending");
    strcpy(records_list_menu_items[3], "Date, Descending");
    strcpy(records_list_menu_items[4], "Date, Ascending");
    strcpy(records_list_menu_items[5], "Amount, Descending");
    strcpy(records_list_menu_items[6], "Amount, Ascending");
    strcpy(records_list_menu_items[7], "Back");

    choice = 1;
    temp_pos_list = (long int *)calloc(total_records, sizeof(long int));
    do
    {
        for (i = 0; i < total_records; i++)
            temp_pos_list[i] = pos_list[i];

        choice = display_menu(records_list_menu_items, 8, choice);

        switch (choice)
        {
        case 1:
            if (display_records_list(pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");
            break;
        case 2:
            for (i = 0; i < total_records; i++)
                temp_pos_list[i] = pos_list[total_records - 1 - i];

            if (display_records_list(temp_pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");

            break;
        case 3:
            qsort(temp_pos_list, total_records, sizeof(long int), date_compare_descending);

            if (display_records_list(temp_pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");

            break;
        case 4:
            qsort(temp_pos_list, total_records, sizeof(long int), date_compare_ascending);

            if (display_records_list(temp_pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");

            break;
        case 5:
            qsort(temp_pos_list, total_records, sizeof(long int), amount_compare_descending);

            if (display_records_list(temp_pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");

            break;
        case 6:
            qsort(temp_pos_list, total_records, sizeof(long int), amount_compare_ascending);

            if (display_records_list(temp_pos_list, total_records) == 0)
                print_detail("PROCESS FAILED");

            break;
        case 7:
            free(pos_list);
            free(temp_pos_list);

            for (i = 0; i < 8; i++)
                free(records_list_menu_items[i]);
            free(records_list_menu_items);
            break;
        }
    }while (choice != 7);

    fclose(fp);
}

void filter_records()
{
    enum filter_type_detail
    {
        any, range, more_than, less_than, spent, earned, any_among, all_of, keyword
    }id_type = any, date_type = any, amount_type = any, type_type = any, tags_type = any, description_type = any;

    FILE *fp, *f_tags;
    short int i;
    int filter_menu_choice, detail_menu_choice;
    char temp_id[11] = "";
    char **filter_menu_detial = NULL;
    char **set_detail_menu = NULL;
    struct record_details record = {0};

    uint8_t flag_tag;
    unsigned int start_id = 0, end_id = 0;
    char start_date[DATE_SIZE] = "", end_date[DATE_SIZE] = "";
    float start_amount = 0, end_amount = 0;
    short int no_of_tags = 0;
    char temp_tag[PASSWORD_TAG_SIZE] = "", dummy[PASSWORD_TAG_SIZE] = "";
    char tags_list[5][PASSWORD_TAG_SIZE] = {""};
    char description[151] = "";

    fp = fopen("records.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    f_tags = fopen("tags.dat", "rb");
    if (f_tags == NULL)
    {
        print_detail("PROCESS FAILED");
        fclose(fp);
        return;
    }

    if (fread(&record.record_id, sizeof(unsigned int), 1, fp) != 1)
        print_detail("NO RECORDS FOUND");
    fseek(fp, 0, SEEK_SET);

    filter_menu_detial = get_2D_char_array(9, 15);
    strcpy(filter_menu_detial[0], "FILTER RECORDS");
    strcpy(filter_menu_detial[1], "ID");
    strcpy(filter_menu_detial[2], "Date");
    strcpy(filter_menu_detial[3], "Amount");
    strcpy(filter_menu_detial[4], "Type");
    strcpy(filter_menu_detial[5], "Tags");
    strcpy(filter_menu_detial[6], "Description");
    strcpy(filter_menu_detial[7], "Continue");
    strcpy(filter_menu_detial[8], "Back");

    set_detail_menu = get_2D_char_array(6, 25);

    filter_menu_choice = 1;
    do
    {
        filter_menu_choice = display_menu(filter_menu_detial, 9, filter_menu_choice);

        switch (filter_menu_choice)
        {
        case 1:
            strcpy(set_detail_menu[0], "Set Detail, ID");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Set Range");
            strcpy(set_detail_menu[3], "More Than ...");
            strcpy(set_detail_menu[4], "Less Than ...");
            strcpy(set_detail_menu[5], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 6, 1);

            switch(detail_menu_choice)
            {
            case 1:
                id_type = any;
                break;
            case 2:
                id_type = range;
                do
                {
                    top.row = (25 - 4)/2;
                    top.col = (80 - 20)/2;
                    bot.row = top.row + 4 - 1;
                    bot.col = top.col + 20 - 1;
                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    gotoxy(top.row, top.col);
                    printf("%-4s: ", "From");
                    top.row++;

                    gotoxy(top.row, top.col);
                    printf("%-4s: ", "To");

                    top.row--;
                    top.col += 6;

                    gotoxy(top.row, top.col);
                    get_data(temp_id, 5);
                    start_id = string_to_int(temp_id);
                    top.row++;

                    gotoxy(top.row, top.col);
                    get_data(temp_id, 5);
                    end_id = string_to_int(temp_id);
                }while ((start_id == 0) || (end_id == 0) || (start_id > end_id));
                break;
            case 3:
                id_type = more_than;
                do
                {
                    top.row = (25 - 3)/2;
                    top.col = (80 - 20)/2;
                    bot.row = top.row + 3 - 1;
                    bot.col = top.col + 20 - 1;

                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    gotoxy(top.row, top.col);
                    printf("From: ");
                    top.col += 6;

                    gotoxy(top.row, top.col);
                    get_data(temp_id, 5);
                    start_id = string_to_int(temp_id);
                }while (start_id == 0);
                break;
            case 4:
                id_type = less_than;
                do
                {
                    top.row = (25 - 3)/2;
                    top.col = (80 - 18)/2;
                    bot.row = top.row + 3 - 1;
                    bot.col = top.col + 18 - 1;

                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    gotoxy(top.row, top.col);
                    printf("To: ");
                    top.col += 4;

                    get_data(temp_id, 5);
                    end_id = string_to_int(temp_id);
                }while (end_id == 0);
                break;
            case 5:
                //don't change anything
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");
            break;
        case 2:
            strcpy(set_detail_menu[0], "Set Detail, Date");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Set Range");
            strcpy(set_detail_menu[3], "Since ...");
            strcpy(set_detail_menu[4], "Till ...");
            strcpy(set_detail_menu[5], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 6, 1);

            switch (detail_menu_choice)
            {
            case 1:
                date_type = any;
                break;
            case 2:
                date_type = range;
                do
                {
                    top.row = (25 - 4)/2;
                    top.col = (80 - 20)/2;
                    bot.row = top.row + 4 - 1;
                    bot.col = top.col + 20 - 1;

                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    gotoxy(top.row, top.col);
                    printf("%-4s: ", "From");
                    top.row++;

                    gotoxy(top.row, top.col);
                    printf("%-4s: ", "To");

                    top.row--;
                    top.col += 6;

                    gotoxy(top.row, top.col);
                    get_edit_date(start_date);
                    top.row++;

                    gotoxy(top.row, top.col);
                    get_edit_date(end_date);
                }while (strcmpi(start_date, end_date) > 0);
                break;
            case 3:
                date_type = more_than;

                top.row = (25 - 3)/2;
                top.col = (80 - 21)/2;
                bot.row = top.row + 3 - 1;
                bot.col = top.col + 21 - 1;

                system("cls");
                print_box(2, 0, 0, 0);

                top.row++;
                top.col += 2;

                gotoxy(top.row, top.col);
                printf("Since: ");
                get_edit_date(start_date);
                break;
            case 4:
                date_type = less_than;

                top.row = (25 - 3)/2;
                top.col = (80 - 20)/2;
                bot.row = top.row + 3 - 1;
                bot.col = top.col + 20 - 1;

                system("cls");
                print_box(2, 0, 0, 0);

                top.row++;
                top.col += 2;

                gotoxy(top.row, top.col);
                printf("Till: ");
                get_edit_date(end_date);
                break;
            case 5:
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");
            break;
        case 3:
            strcpy(set_detail_menu[0], "Set Detail, Amount");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Set Range");
            strcpy(set_detail_menu[3], "Above ...");
            strcpy(set_detail_menu[4], "Below ...");
            strcpy(set_detail_menu[5], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 6, 1);

            switch (detail_menu_choice)
            {
            case 1:
                amount_type = any;
                break;
            case 2:
                amount_type = range;
                do
                {
                    top.row = (25 - 4)/2;
                    top.col = (80 - 24)/2;
                    bot.row = top.row + 4 - 1;
                    bot.col = top.col + 24 - 1;

                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    gotoxy(top.row, top.col);
                    printf("%-4s: Rs.", "From");
                    top.row++;

                    gotoxy(top.row, top.col);;
                    printf("%-4s: Rs.", "To");

                    top.row--;
                    top.col += 6;

                    start_amount = -1;
                    do
                    {
                        for (i = 0; i < 6; i++)
                        {
                            gotoxy(top.row, top.col + 4 + i);
                            printf(" ");
                        }
                        gotoxy(top.row, top.col + 4);
                        scanf("%f", &start_amount);
                    }while(start_amount < 0);
                    top.row++;

                    end_amount = -1;
                    do
                    {
                        for (i = 0; i < 6; i++)
                        {
                            gotoxy(top.row, top.col + 4 + i);
                            printf(" ");
                        }
                        gotoxy(top.row, top.col + 4);
                        scanf("%f", &end_amount);
                    }while(end_amount < 0);
                }while (start_amount > end_amount);
                break;
            case 3:
                amount_type = more_than;

                top.row = (25 - 3)/2;
                top.col = (80 - 25)/2;
                bot.row = top.row + 3 - 1;
                bot.col = top.col + 25 - 1;

                system("cls");
                print_box(2, 0, 0, 0);

                top.row++;
                top.col += 2;

                gotoxy(top.row, top.col);
                printf("Above: Rs. ");

                top.col += 7;

                start_amount = -1;
                do
                {
                    for (i = 0; i < 6; i++)
                    {
                        gotoxy(top.row, top.col + 4 + i);
                        printf(" ");
                    }
                    gotoxy(top.row, top.col + 4);
                    scanf("%f", &start_amount);
                }while(start_amount < 0);
                break;
            case 4:
                amount_type = less_than;

                top.row = (25 - 3)/2;
                top.col = (80 - 25)/2;
                bot.row = top.row + 3 - 1;
                bot.col = top.col + 25 - 1;

                system("cls");
                print_box(2, 0, 0, 0);

                top.row++;
                top.col += 2;

                gotoxy(top.row, top.col);
                printf("Below: Rs. ");

                top.col += 7;

                end_amount = -1;
                do
                {
                    for (i = 0; i < 6; i++)
                    {
                        gotoxy(top.row, top.col + 4 + i);
                        printf(" ");
                    }
                    gotoxy(top.row, top.col + 4);
                    scanf("%f", &end_amount);
                }while(end_amount < 0);
                break;
            case 5:
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");
            break;
        case 4:
            strcpy(set_detail_menu[0], "Set Detail, Type");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Expenses");
            strcpy(set_detail_menu[3], "Earnings");
            strcpy(set_detail_menu[4], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 5, 1);

            switch (detail_menu_choice)
            {
            case 1:
                type_type = any;
                break;
            case 2:
                type_type = spent;
                break;
            case 3:
                type_type = earned;
                break;
            case 4:
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");
            break;
        case 5:
            strcpy(set_detail_menu[0], "Set Detail, Tags");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Any among ...");
            strcpy(set_detail_menu[3], "All of ...");
            strcpy(set_detail_menu[4], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 5, 1);

            switch (detail_menu_choice)
            {
            case 1:
                tags_type = any;
                break;
            case 2:
                tags_type = any_among;
                break;
            case 3:
                tags_type = all_of;
                break;
            case 4:
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");

            if ((detail_menu_choice == 2) || (detail_menu_choice == 3))
            {
                no_of_tags = 0;
                for (i = 0; i , no_of_tags; i++)
                    strcpy(tags_list[i], "");

                do
                {
                    top.row = (25 - 7)/2;
                    top.col = (80 - 24)/2;
                    bot.row = top.row + 7 - 1;
                    bot.col = top.col + 24 - 1;

                    system("cls");
                    print_box(2, 0, 0, 0);

                    top.row++;
                    top.col += 2;

                    for (i = 0; i < 5; i++)
                    {
                        gotoxy(top.row + i, top.col);
                        printf("Tag: ");
                    }
                    top.col += 5;

                    for (i = 0; i < no_of_tags; i++)
                    {
                        gotoxy(top.row, top.col);
                        printf("%s", tags_list[i]);
                        top.row++;
                    }

                    if (no_of_tags == 5)
                    {
                        (void)getkey();
                        break;
                    }

                    gotoxy(top.row, top.col);
                    get_data(temp_tag, 3);

                    if (strlen(temp_tag) > 0)
                    {
                        flag_tag = 0;
                        fseek(f_tags, 0, SEEK_SET);
                        while (fread(dummy, PASSWORD_TAG_SIZE*sizeof(char), 1, f_tags) == 1)
                        {
                            if (strcmpi(temp_tag, dummy) == 0)
                            {
                                flag_tag = 1;

                                for (i = 0; i < no_of_tags; i++)
                                    if (strcmpi(tags_list[i], dummy) == 0)
                                    {
                                        flag_tag = 0;
                                        break;
                                    }
                                break;
                            }
                        }
                        if (flag_tag == 1)
                        {
                            no_of_tags++;
                            strcpy(tags_list[no_of_tags - 1], dummy);

                            gotoxy(top.row, top.col);
                            printf("%s", dummy);
                        }
                    }
                    else if (strlen(temp_tag) == 0)
                    {
                        if (no_of_tags == 0)
                            tags_type = any;
                        break;
                    }
                }while (no_of_tags < 5);
            }
            break;
        case 6:
            strcpy(set_detail_menu[0], "Set Detail, Description");
            strcpy(set_detail_menu[1], "Any");
            strcpy(set_detail_menu[2], "Enter keyword");
            strcpy(set_detail_menu[3], "Back");

            detail_menu_choice = display_menu(set_detail_menu, 4, 1);

            switch (detail_menu_choice)
            {
            case 1:
                description_type = any;
                break;
            case 2:
                description_type = keyword;

                top.row = (25 - 5)/2;
                top.col = (80 - 63)/2;
                bot.row = top.row + 5 - 1;
                bot.col = top.col + 63 - 1;

                system("cls");
                print_box(2, 0, 0, 0);

                top.row++;
                top.col += 2;

                gotoxy(top.row, top.col);
                printf("Keyword: ");
                top.col += 9;
                get_edit_display_description(description, 2);
                break;
            case 3:
                break;
            }
            for (i = 0; i < 6; i++)
                strcpy(set_detail_menu[i], "");
            break;
        case 7:
            //FILTER
            break;
        case 8:
            for (i = 0; i < 9; i++)
                free(filter_menu_detial[i]);
            free(filter_menu_detial);

            for (i = 0; i < 6; i++)
                free(set_detail_menu[i]);
            free(set_detail_menu);

            fclose(fp);
            fclose(f_tags);
            break;
        }
    }while (filter_menu_choice != 8);

    /*
    length = 10;
    turn = id;
    while (1)
    {
        top.row = (25 - length)/2;
        top.col = (80 - 67)/2;
        bot.row = top.row + length - 1;
        bot.col = top.col + 67 - 1;

        system("cls");
        print_box(2, 3, 0, 2);

        gotoxy(top.row + 1, (80 - strlen("FILTER RECORDS"))/2)
        printf("FILTER RECORDS");

        top.row += 3;
        top.col += 2;
        temp.row = top.row;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "ID");
        if (id_type == any)
            printf("Any");
        else if (id_type == range)
            printf("%03u to %03u", start_id, end_id);
        else if (id_type == more)
            printf("From %03u", start_id);
        else if (d_type == less)
            printf("Up to %03u", end_id);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Date");
        if (date_type == set)
            printf("Set detail");
        else if (date_type == any)
            printf("Any");
        else if (date_type == range)
            printf("%s to %s", start_date, end_date);
        else if (date_type == more)
            printf("Since %s", start_date);
        else if (date_type == less)
            printf("TIll %s", end_date);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Amount");
        if (amount_type == set)
            printf("Set deatail");
        else if (amount_type == any)
            printf("Any");
        else if (amount_type == range)
            printf("Rs. %.2f to Rs. %.2f", start_amount, end_amount);
        else if (amount_type == more)
            printf("More than Rs. %.2f", start_amount);
        else if (amount_type == less)
            printf("Less than Rs. %.2f", end_amount);
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Type");
        if (type_type == set)
            printf("Set detail");
        else if (type_type == any)
            printf("Any");
        else if (type_type == spent)
            printf("Spent");
        else if (type_type == earned)
            printf("Earned");
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s: ", "Tag");
        if (tag_type == set)
            printf("Set Detail");
        else if (tag_type == any)
            printf("Any");
        else
        {

        }
        top.row++;

        gotoxy(top.row, top.col);
        printf("%-11s:", "Description");
        if (description_type == set)
            printf("Set detail");
        else if (description_type == any)
            printf("Any");
        else
        {
            top.col += 13;
            get_edit_display_description(description, 3);
        }

        top.row = temp.row;
    }
    */
}

void edit_record()
{
    FILE *fp = NULL, *f_temp = NULL;
    uint8_t flag_record;
    int choice;
    short int i;
    unsigned int dummy_id;
    struct record_details record = {0};

    fp = fopen("records.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    if (fread(&record.record_id, sizeof(unsigned int), 1, fp) != 1)
    {
        print_detail("NO RECORDS FOUND");
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_SET);

    dummy_id = display_record_id_window(1);

    if (dummy_id > record.record_id)
    {
        print_detail("RECORD NOT FOUND");
        fclose(fp);
        return;
    }
    else
    {
        f_temp = fopen("temp.dat", "wb");
        if (f_temp == NULL)
        {
            print_detail("PROCESS FAILED");
            fclose(fp);
            return;
        }

        while (fread(&record.record_id, sizeof(unsigned int), 1, fp) == 1)
        {
            fread(record.date, sizeof(record.date), 1, fp);
            fread(&record.amount, sizeof(float), 1, fp);
            fread(&record.type, sizeof(char), 1, fp);

            fread(&record.no_of_tags, sizeof(short int), 1, fp);
            record.tags_list = get_2D_char_array(record.no_of_tags, PASSWORD_TAG_SIZE);
            for (i = 0; i < record.no_of_tags; i++)
                fread(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, fp);

            fread(&record.description_size, sizeof(short int), 1, fp);
            record.description = malloc(record.description_size*sizeof(char));
            fread(record.description, record.description_size*sizeof(char), 1, fp);

            if (dummy_id == record.record_id)
            {
                flag_record = 1;

                choice = diplay_a_record(record);

                if (choice == 1)
                {
                    edit_record_window(&record);

                    print_detail("RECORD EDITED");
                }
                else
                    print_detail("CANCELLED");
            }

            fwrite(&record.record_id, sizeof(unsigned int), 1, f_temp);
            fwrite(record.date, sizeof(record.date), 1, f_temp);
            fwrite(&record.amount, sizeof(float), 1, f_temp);
            fwrite(&record.type, sizeof(char), 1, f_temp);
            fwrite(&record.no_of_tags, sizeof(short int), 1, f_temp);
            for (i = 0; i < record.no_of_tags; i++)
                fwrite(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_temp);
            fwrite(&record.description_size, sizeof(short int), 1, f_temp);
            fwrite(record.description, record.description_size*sizeof(char), 1, f_temp);

            for (i = 0; i < record.no_of_tags; i++)
                free(record.tags_list[i]);
            free(record.tags_list);
            free(record.description);
        }
        if (flag_record == 0)
            print_detail("RECORD NOT FOUND");

        fclose(fp);
        fclose(f_temp);

        remove("records.dat");
        rename("temp.dat", "records.dat");
    }
}

void delete_record()
{
    FILE *fp = NULL, *f_temp = NULL;
    uint8_t flag_record, delete_data;
    int choice;
    short int i;
    unsigned int dummy_id;
    struct record_details record = {0};

    fp = fopen("records.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    if (fread(&record.record_id, sizeof(unsigned int), 1, fp) != 1)
    {
        print_detail("NO RECORDS FOUND");
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_SET);

    dummy_id = display_record_id_window(2);

    if (dummy_id > record.record_id)
    {
        print_detail("RECORD NOT FOUND");
        fclose(fp);
        return;
    }
    else
    {
        f_temp = fopen("temp.dat", "wb");
        if (f_temp == NULL)
        {
            print_detail("PROCESS FAILED");
            fclose(fp);
            return;
        }

        flag_record = 0;
        while (fread(&record.record_id, sizeof(unsigned int), 1, fp) == 1)
        {
            fread(record.date, sizeof(record.date), 1, fp);
            fread(&record.amount, sizeof(float), 1, fp);
            fread(&record.type, sizeof(char), 1, fp);

            fread(&record.no_of_tags, sizeof(short int), 1, fp);
            record.tags_list = get_2D_char_array(record.no_of_tags, PASSWORD_TAG_SIZE);
            for (i = 0; i < record.no_of_tags; i++)
                fread(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, fp);

            fread(&record.description_size, sizeof(short int), 1, fp);
            record.description = malloc(record.description_size*sizeof(char));
            fread(record.description, record.description_size*sizeof(char), 1, fp);

            delete_data = 0;

            if (dummy_id == record.record_id)
            {
                flag_record = 1;

                choice = diplay_a_record(record);

                if (choice == 1)
                {
                    delete_data = 1;
                    print_detail("RECORD DELETED");
                }
                else
                    print_detail("CANCELLED");
            }

            if (delete_data == 0)
            {
                fwrite(&record.record_id, sizeof(unsigned int), 1, f_temp);
                fwrite(record.date, sizeof(record.date), 1, f_temp);
                fwrite(&record.amount, sizeof(float), 1, f_temp);
                fwrite(&record.type, sizeof(char), 1, f_temp);
                fwrite(&record.no_of_tags, sizeof(short int), 1, f_temp);
                for (i = 0; i < record.no_of_tags; i++)
                    fwrite(record.tags_list[i], PASSWORD_TAG_SIZE*sizeof(char), 1, f_temp);
                fwrite(&record.description_size, sizeof(short int), 1, f_temp);
                fwrite(record.description, record.description_size*sizeof(char), 1, f_temp);

                for (i = 0; i < record.no_of_tags; i++)
                    free(record.tags_list[i]);
                free(record.tags_list);
                free(record.description);
            }
        }
        if (flag_record == 0)
            print_detail("RECORD NOT FOUND");

        fclose(fp);
        fclose(f_temp);

        remove("records.dat");
        rename("temp.dat", "records.dat");
    }
}

void tags_menu()
{
    short unsigned int i, choice;
    char **tags_menu_items = NULL;

    tags_menu_items = get_2D_char_array(7, 15);
    strcpy(tags_menu_items[0], "TAGS");
    strcpy(tags_menu_items[1], "Add Tag");
    strcpy(tags_menu_items[2], "List Tags");
    strcpy(tags_menu_items[3], "Search Tags");
    strcpy(tags_menu_items[4], "Edit Tag");
    strcpy(tags_menu_items[5], "Delete Tag");
    strcpy(tags_menu_items[6], "Back");

    choice = 1;
    do
    {
        choice = display_menu(tags_menu_items, 7, choice);

        switch (choice)
        {
        case 1:
            add_tag();
            break;
        case 2:
            list_tags();
            break;
        case 3:
            search_tags();
            break;
        case 4:
            edit_tag();
            break;
        case 5:
            delete_tag();
            break;
        case 6:
            for (i = 0; i < 6; i++)
                free(tags_menu_items[i]);
            free(tags_menu_items);
            break;
        }
    }while (choice != 6);
}

void add_tag()
{
    FILE *fp;
    uint8_t flag;
    //unsigned short int total_tags = 0;
    char tag[PASSWORD_TAG_SIZE] = "", dummy[PASSWORD_TAG_SIZE] = "";

    fp = fopen("tags.dat", "rb+");
    if (fp == NULL)
    {
        fp = fopen("tags.dat", "ab");
        if (fp == NULL)
        {
            print_detail("PROCESS FAILED");
            return;
        }
    }

    display_tag_id_window(tag, NULL, 0);

    if (strcmpi(tag, "Others") == 0)
    {
        print_detail("ALREADY EXIST");
        fclose(fp);
        return;
    }

    flag = 0;
    fseek(fp, 0, SEEK_SET);
    while (fread(dummy, sizeof(dummy), 1, fp) == 1)
        if (strcmpi(dummy, tag) == 0)
        {
            flag = 1;
            break;
        }

    fseek(fp, 0, SEEK_END);
    if (flag == 0)
    {
        fwrite(tag, sizeof(tag), 1, fp);

        print_detail("TAG ADDED");
    }
    else if (flag == 1)
        print_detail("ALREADY EXIST");

    fclose(fp);
}

void list_tags()
{
    FILE *fp;
    short unsigned int i, choice, total_tags = 0;
    char **list_tags_menu_items = NULL;
    char **tags_list = NULL, **temp_tags_list = NULL;
    char dummy[PASSWORD_TAG_SIZE] = "";

    fp = fopen("tags.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    while (fread(dummy, sizeof(dummy), 1, fp) == 1)
        total_tags++;

    list_tags_menu_items = get_2D_char_array(6, 15);
    strcpy(list_tags_menu_items[0], "LIST TAGS");
    strcpy(list_tags_menu_items[1], "Oldest");
    strcpy(list_tags_menu_items[2], "Latest");
    strcpy(list_tags_menu_items[3], "Ascending");
    strcpy(list_tags_menu_items[4], "Descending");
    strcpy(list_tags_menu_items[5], "Back");

    tags_list = get_2D_char_array(total_tags, PASSWORD_TAG_SIZE);

    fseek(fp, 0, SEEK_SET);
    for (i = 0; fread(dummy, sizeof(dummy), 1, fp) == 1; i++)
        strcpy(tags_list[i], dummy);

    choice = 1;
    do
    {
        choice = display_menu(list_tags_menu_items, 6, choice);

        switch (choice)
        {
        case 1:
            display_tags_list(tags_list, total_tags);
            break;
        case 2:
            temp_tags_list = get_2D_char_array(total_tags, PASSWORD_TAG_SIZE);

            for (i = 0; i < total_tags; i++)
                strcpy(temp_tags_list[i], tags_list[total_tags - 1 - i]);

            display_tags_list(temp_tags_list, total_tags);

            for (i = 0; i < total_tags; i++)
                free(temp_tags_list[i]);
            free(temp_tags_list);

            break;
        case 3:
            temp_tags_list = get_2D_char_array(total_tags, PASSWORD_TAG_SIZE);

            for (i = 0; i < total_tags; i++)
                strcpy(temp_tags_list[i], tags_list[i]);

            qsort(temp_tags_list, total_tags, sizeof(char *), tag_compare_ascending);
            display_tags_list(temp_tags_list, total_tags);

            for (i = 0; i < total_tags; i++)
                free(temp_tags_list[i]);
            free(temp_tags_list);

            break;
        case 4:
            temp_tags_list = get_2D_char_array(total_tags, PASSWORD_TAG_SIZE);

            for (i = 0; i < total_tags; i++)
                strcpy(temp_tags_list[i], tags_list[i]);

            qsort(temp_tags_list, total_tags, sizeof(char *), tag_compare_descending);
            display_tags_list(temp_tags_list, total_tags);

            for (i = 0; i < total_tags; i++)
                free(temp_tags_list[i]);
            free(temp_tags_list);

            break;
        case 5:
            for (i = 0; i < total_tags; i++)
                free(tags_list[i]);
            free(tags_list);

            for (i = 0; i < 6; i++)
                free(list_tags_menu_items[i]);
            free(list_tags_menu_items);

            fclose(fp);
            break;
        }
    }while (choice != 5);
}

void search_tags()
{
    FILE *fp;
    short unsigned int i, total_tags = 0;
    char tag[PASSWORD_TAG_SIZE] = "", dummy[PASSWORD_TAG_SIZE] = "";
    char **tags_list = NULL;

    fp = fopen("tags.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    display_tag_id_window(tag, NULL, 1);

    while (fread(dummy, sizeof(dummy), 1, fp) == 1)
    {
        if (strstr(dummy, tag) != NULL)
        {
            tags_list = change_2D_char_array_size(tags_list, total_tags, total_tags + 1, PASSWORD_TAG_SIZE);
            strcpy(tags_list[total_tags], dummy);
            total_tags++;
        }
    }

    if (total_tags == 0)
        print_detail("TAGS NOT FOUND");
    else
    {
        display_tags_list(tags_list, total_tags);

        for (i = 0; i < total_tags; i++)
            free(tags_list[i]);
        free(tags_list);
    }

    fclose(fp);
}

void edit_tag()
{
    FILE *fp, *temp;
    uint8_t flag;
    char old_tag[PASSWORD_TAG_SIZE] = "", new_tag[PASSWORD_TAG_SIZE] = "", dummy[PASSWORD_TAG_SIZE] = "";

    fp = fopen("tags.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    display_tag_id_window(old_tag, new_tag, 2);

    if (strcmpi(old_tag, "Others") == 0)
    {
        print_detail("CANT CHANGE THIS TAG");
        fclose(fp);
        return;
    }

    do
        if (strcmp(dummy, new_tag) == 0)
        {
            print_detail("ALREADY EXIST");
            fclose(fp);
            return;
        }
    while (fread(dummy, sizeof(dummy), 1, fp) == 1);

    temp = fopen("temp.dat", "wb");
    if (temp == NULL)
    {
        fclose(fp);

        print_detail("PROCESS FAILED");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    flag = 0;
    while (fread(dummy, sizeof(dummy), 1, fp) == 1)
        if (strcmp(dummy, old_tag) == 0)
        {
            if (get_conformation("CONFORM CHANGE") == 1)
            {
                flag = 1;
                fwrite(new_tag, sizeof(new_tag), 1, temp);
                print_detail("TAG EDITED");
            }
            else
            {
                flag = 1;
                fwrite(dummy, sizeof(dummy), 1, temp);
                print_detail("CANCELLED");
            }
        }
        else
            fwrite(dummy, sizeof(dummy), 1, temp);

    if (flag == 0)
        print_detail("TAG NOT FOUND");

    fclose(fp);
    fclose(temp);

    remove("tags.dat");
    rename("temp.dat", "tags.dat");
}

void delete_tag()
{
    FILE *temp, *fp;
    uint8_t flag;
    char tag[PASSWORD_TAG_SIZE] = "", dummy[PASSWORD_TAG_SIZE] = "";

    fp = fopen("tags.dat", "rb");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    display_tag_id_window(tag, NULL, 3);

    if (strcmpi(tag, "Others") == 0)
    {
        print_detail("CANT DELETE THIS TAG");
        fclose(fp);
        return;
    }

    temp = fopen("temp.dat", "wb");
    if (temp == NULL)
    {
        fclose(fp);

        print_detail("PROCESS FAILED");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    flag = 0;
    while (fread(dummy, sizeof(dummy), 1, fp) == 1)
        if (strcmpi(dummy, tag) == 0)
        {
            if (get_conformation("CONFORM DELETE") == 1)
            {
                flag = 1;
                print_detail("TAG DELETED");
            }
            else
            {
                flag = 1;
                fwrite(dummy, sizeof(dummy), 1, temp);
                print_detail("CANCELLED");
            }
        }
        else
            fwrite(dummy, sizeof(dummy), 1, temp);

    if (flag == 0)
        print_detail("TAG NOT FOUND");

    fclose(fp);
    fclose(temp);

    remove("tags.dat");
    rename("temp.dat", "tags.dat");
}

void settings_menu()
{
    short unsigned int i, choice;
    char **settings_menu_items = NULL;

    settings_menu_items = get_2D_char_array(4, 20);
    strcpy(settings_menu_items[0], "SETTINGS");
    strcpy(settings_menu_items[1], "Change Password");
    strcpy(settings_menu_items[2], "Delete Account");
    strcpy(settings_menu_items[3], "Back");

    choice = 1;
    do
    {
        choice = display_menu(settings_menu_items, 4, choice);

        switch (choice)
        {
        case 1:
            change_password();
            break;
        case 2:
            del_acc_flag = 0;
            delete_account();
            if (del_acc_flag == 1)
            {
                choice = 3;
                for (i = 0; i < 4; i++)
                    free(settings_menu_items[i]);
                free(settings_menu_items);
            }
            break;
        case 3:
            for (i = 0; i < 4; i++)
                free(settings_menu_items[i]);
            free(settings_menu_items);
            break;
        }
    }while (choice != 3);
}

void change_password()
{
    FILE *fp = NULL;
    char old_password[PASSWORD_TAG_SIZE] = "", new_password[PASSWORD_TAG_SIZE] = "", confirm_password[PASSWORD_TAG_SIZE] = "";

    fp = fopen("setting.dat", "rb+");
    if (fp == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }
    fclose(fp);

    system("cls");

    top.row = 8;
    top.col = 14;
    bot.row = 15;
    bot.col = 66;
    print_box(2, 3, 0, 4);
    gotoxy(9, 32);
    printf("CHANGE PASSWORD");

    gotoxy(11, 16);
    printf("%-16s: %s", "Username", loged_in_as.username);
    gotoxy(12, 16);
    printf("%-16s: ", "Old Password");
    gotoxy(13, 16);
    printf("%-16s: ", "New Password");
    gotoxy(14, 16);
    printf("%-16s: ", "Confirm Password");

    gotoxy(12, 34);
    get_data(old_password, 1);
    gotoxy(13, 34);
    get_data(new_password, 1);
    gotoxy(14, 34);
    get_data(confirm_password, 1);


    if ((strcmp(old_password, loged_in_as.password) == 0) && (strcmp(new_password, confirm_password) == 0))
    {
        if (get_conformation("CONFORM CHANGE") == 1)
        {
            strcpy(loged_in_as.password, new_password);

            remove("setting.dat");
            fp = fopen("setting.dat", "wb");
            fwrite(new_password, sizeof(new_password), 1, fp);
            fclose(fp);

            print_detail("PASSWORD CHANGED");
        }
        else
            print_detail("PROCESS CANCELLED");
    }
    else
        print_detail("ERROR!!");
}

void delete_account()
{
    FILE *f_index = NULL, *temp = NULL;
    char folder_name[FOLDER_NAME_SIZE] = "";
    struct user_details data = {0};

    chdir("..");        //"Users" folderma gayeko

    f_index = fopen("index.dat", "rb");
    if (f_index == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    get_folder_name(folder_name, loged_in_as);

    system("cls");

    top.row = 7;
    top.col = 18;
    bot.row = 12;
    bot.col = 61;
    print_box(2, 3, 0, 4);
    gotoxy(8, 32);
    printf("DELETE ACCOUNT");

    gotoxy(10, 20);
    printf("Username: %s", loged_in_as.username);
    gotoxy(11, 20);
    printf("Password: ");
    get_data(data.password, 1);

    if (strcmp(data.password, loged_in_as.password) == 0)
    {
        if (get_conformation("CONFORM DELETE") == 1)
        {
            total_accounts--;

            temp = fopen("temp.dat", "wb");
            if (temp == NULL)
            {
                print_detail("PROCESS FAILED");
                fclose(f_index);
                return;
            }

            fseek(f_index, 0, SEEK_SET);

            while(fread(&data.account_no, sizeof(data.account_no), 1, f_index) == 1)
            {
                fread(data.username, sizeof(data.username), 1, f_index);
                if (strcmp(data.username, loged_in_as.username) != 0)
                {
                    fwrite(&data.account_no, sizeof(data.account_no), 1, temp);
                    fwrite(data.username, sizeof(data.username), 1, temp);

                    new_account_no = data.account_no;
                }
            }
            new_account_no++;

            chdir(folder_name);     //personal folderma gayeko

            remove("setting.dat");
            remove("records.dat");
            remove("tags.dat");
            //system("del *.* /s /q");

            chdir("..");            //"users" folderma aayeko
            rmdir(folder_name);

            fclose(temp);
            fclose(f_index);

            remove("index.dat");
            rename("temp.dat", "index.dat");
            //system("del index.dat /s /q");
            //system("ren temp.dat index.dat");

            print_detail("ACCOUNT DELETED");

            del_acc_flag = 1;
            return;
        }
        else
            print_detail("PROCESS CANCELLED");
    }
    else
        print_detail("ERROR!!");

    chdir(folder_name);

    fclose(f_index);
}

void creat_account()
{
    FILE *f_index = NULL, *fp = NULL;
    char confirm_password[PASSWORD_TAG_SIZE] = {0};
    char folder_name[FOLDER_NAME_SIZE] = {0};
    char new_tag[PASSWORD_TAG_SIZE] = "Others";
    struct user_details data = {0};
    struct user_details dummy = {0};

    f_index = fopen("index.dat", "rb+");
    if (f_index == NULL)
    {
        print_detail("PROCESS FAILED");
        return;
    }

    system("cls");

    top.row = 6;
    top.col = 14;
    bot.row = 12;
    bot.col = 66;
    print_box(2, 3, 0, 4);
    gotoxy(7, 30);
    printf("CREATE NEW ACCOUNT");

    gotoxy(9, 16);
    printf("%-16s: ", "Enter Username");
    gotoxy(10, 16);
    printf("%-16s: ", "Enter Password");
    gotoxy(11, 16);
    printf("%-16s: ", "Confirm Password");

    data.account_no = new_account_no;
    gotoxy(9, 34);
    get_data(data.username, 0);
    gotoxy(10, 34);
    get_data(data.password, 1);
    gotoxy(11, 34);
    get_data(confirm_password, 1);

    while (fread(&dummy.account_no, sizeof(dummy.account_no), 1, f_index) == 1)
    {
        fread(dummy.username, sizeof(dummy.username), 1, f_index);

        if (strcmp(dummy.username, data.username) == 0)
        {
            print_detail("USERNAME ALREADY EXIST");
            fclose(f_index);
            return;
        }
    }

    if (strcmp(confirm_password, data.password) == 0)
    {
        fseek(f_index, 0, SEEK_END);
        fwrite(&data.account_no, sizeof(data.account_no), 1, f_index);
        fwrite(data.username, sizeof(data.username), 1, f_index);

        total_accounts++;
        new_account_no++;

        get_folder_name(folder_name, data);
        //folder banako
        mkdir(folder_name);
        //folder vitra gayeko
        chdir(folder_name);

        fp = fopen("setting.dat", "wb");
        fwrite(data.password, sizeof(data.password), 1, fp);
        fclose(fp);
        fp = fopen("records.dat", "wb");
        fclose(fp);
        fp = fopen("tags.dat", "wb");
        fwrite(new_tag, sizeof(new_tag), 1, fp);
        fclose(fp);

        //folder bata bahira aayeko...."User" folders ma aayeko
        chdir("..");

        print_detail("ACCOUNT CREATED");
    }
    else
        print_detail("ERROR!!");

    fclose(f_index);
}
