#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ACCOUNTS 10000
#define MAX_TEXT_LENGTH 30
#define MAX_TEXT_LENGTH_D MAX_TEXT_LENGTH + 3
#define DEBT_LIMIT -1000
#define MAX_SAVINGS 99999999999999999

typedef struct
{
    char name[MAX_TEXT_LENGTH_D];
    char surname[MAX_TEXT_LENGTH_D];
    char address[MAX_TEXT_LENGTH_D];
    char pesel[MAX_TEXT_LENGTH_D];
    float balance;
    float savings;
} CustAccount;

bool does_file_exist(const char *path)
{
    if (access(path, F_OK) == -1)
    {
        return false;
    }
    return true;
}

void flush_stdin()
{
    while ((getchar()) != '\n')
        ;
}

void to_lower(char *word)
{
    for (int j = 0; word[j]; j++)
    {
        word[j] = tolower(word[j]);
    }
}

bool is_name(char *word)
{
    int i;
    bool is_letter = false;
    for (i = 0; word[i]; i++)
    {
        if ((word[i] < 'A' || (word[i] > 'Z' && word[i] < 'a') || word[i] > 'z') && word[i] != ' ')
        {
            return false;
        }
        if (word[i] != ' ')
        {
            is_letter = true;
        }
    }
    if (i < 2 || !is_letter)
    {
        return false;
    }
    return true;
}

bool is_address(char *word)
{
    int i;
    bool is_letter = false;
    for (i = 0; word[i]; i++)
    {
        if ((word[i] < '0' || (word[i] > '9' && word[i] < 'A') || (word[i] > 'Z' && word[i] < 'a') || word[i] > 'z') && word[i] != ' ')
        {
            return false;
        }
        if (word[i] != ' ')
        {
            is_letter = true;
        }
    }
    if (i < 2 || !is_letter)
    {
        return false;
    }
    return true;
}

void choice_input(char *user_choice_f)
{
    printf("\nWhat would you like to do?\n\n");
    printf("1. To create new user type: 'create'\n");
    printf("2. To list all accounts type: 'list'\n");
    printf("3. To search for an account type: 'search\n");
    printf("4. To make a deposit type: 'deposit'\n");
    printf("5. To make a withdrawal type: 'withdrawal'\n");
    printf("6. To make a money transfer between two accounts type: 'transfer'\n");
    printf("7. To make a money transfer to/from a saving account type: 'save'\n");
    printf("8. To close program type: 'close;\n");
    while (1)
    {
        printf("Input what you want: ");
        fgets(user_choice_f, MAX_TEXT_LENGTH_D, stdin);
        if (strlen(user_choice_f) <= MAX_TEXT_LENGTH)
        {
            user_choice_f[strcspn(user_choice_f, "\n")] = 0;
            break;
        }
        printf("ERROR: Input too long, try again.\n\n");
        flush_stdin();
    }
    to_lower(user_choice_f);
}

bool is_float(char *number)
{
    bool is_number = false;
    bool is_dot = false;
    bool is_number_aftdot = false;
    int number_of_decimal = 0;
    int i;
    for (i = 0; number[i]; i++)
    {
        if (number[i] != '.' && (number[i] < '0' || number[i] > '9'))
        {
            return false;
        }
        else
        {
            if (number[i] >= '0' && number[i] <= '9')
            {
                is_number = true;
                if (is_dot)
                {
                    is_number_aftdot = true;
                    number_of_decimal += 1;
                }
            }
            else if (number[i] == '.' && (!is_number || is_dot))
            {
                return false;
            }
            else if (number[i] == '.')
            {
                is_dot = true;
            }
        }
    }
    if(number_of_decimal > 2)
    {
        return false;
    }
    if (i < 1)
    {
        return false;
    }
    if (is_dot && !is_number_aftdot)
    {
        return false;
    }
    return true;
}

bool is_int(char *number)
{
    int i;
    for (i = 0; number[i]; i++)
    {
        if (number[i] <= '0' || number[i] > '9')
        {
            return false;
        }
    }
    if (i < 1)
    {
        return false;
    }
    return true;
}

bool is_pesel(char *pesel)
{
    int i;
    for (i = 0; pesel[i]; i++)
    {
        if (!isdigit(pesel[i]) || i >= 12)
        {
            return false;
        }
    }
    if (i == 11)
    {
        return true;
    }
    return false;
}

bool strings_compare(char *word_1, char *word_2)
{
    if (word_1[0] == 0)
    {
        return false;
    }
    if (strlen(word_1) != strlen(word_2))
    {
        return false;
    }
    for (int j = 0; word_1[j]; j++)
    {
        if (word_1[j] == '\n' || word_1[j] == '\0')
        {
            if (word_2[j] != '\n' || word_2[j] != '\0')
            {
                return false;
            }
            return true;
        }
        if (word_2[j] == '\n' || word_2[j] == '\0')
        {
            if (word_1[j] != '\n' || word_1[j] != '\0')
            {
                return false;
            }
            return true;
        }
        if (tolower(word_1[j]) != tolower(word_2[j]))
        {
            return false;
        }
    }
    return true;
}

bool confirmation()
{
    char user_choice_f[MAX_TEXT_LENGTH_D];
    while (1)
    {
        while (1)
        {
            printf("Would you like to confirm the action? (yes/no)\n");
            fgets(user_choice_f, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(user_choice_f) <= MAX_TEXT_LENGTH)
            {
                user_choice_f[strcspn(user_choice_f, "\n")] = 0;
                break;
            }
            printf("ERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        to_lower(user_choice_f);
        if (strings_compare(user_choice_f, "yes"))
        {
            return true;
        }
        else if (strings_compare(user_choice_f, "no"))
        {
            return false;
        }
        printf("\nWrong input, try again\n");
    }
}

void gett_accNo(int *accNof, FILE *fptr)
{
    CustAccount custf;
    rewind(fptr);
    for (int j = 1; j < ACCOUNTS; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (is_name(custf.name))
        {
            *accNof += 1;
        }
    }
    rewind(fptr);
}

void print_cust(int numb, CustAccount custf)
{
    printf("\nNumber: %d | Name: %s | Surname: %s | Address: %s | Pesel: %s | Balance: %.2f | Savings: %.2f\n", numb, custf.name, custf.surname, custf.address, custf.pesel, custf.balance, custf.savings);
}

void get_name(char *namef)
{
    while (1)
    {
        while (1)
        {
            printf("Input name: ");
            fgets(namef, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(namef) <= MAX_TEXT_LENGTH)
            {
                namef[strcspn(namef, "\n")] = 0;
                break;
            }
            printf("ERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_name(namef))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            break;
        }
    }
}

void get_surname(char *surnamef)
{
    while (1)
    {
        while (1)
        {
            printf("Input surname: ");
            fgets(surnamef, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(surnamef) <= MAX_TEXT_LENGTH)
            {
                surnamef[strcspn(surnamef, "\n")] = 0;
                break;
            }
            printf("ERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_name(surnamef))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            break;
        }
    }
}

void get_address(char *addressf)
{
    while (1)
    {
        while (1)
        {
            printf("Input address: ");
            fgets(addressf, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(addressf) <= MAX_TEXT_LENGTH)
            {
                addressf[strcspn(addressf, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_address(addressf))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            break;
        }
    }
}

void get_pesel(char *peself)
{
    while (1)
    {
        while (1)
        {
            printf("Input pesel: ");
            fgets(peself, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(peself) <= MAX_TEXT_LENGTH)
            {
                peself[strcspn(peself, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_pesel(peself))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            break;
        }
    }
}

void get_balance(char *balancef)
{
    while (1)
    {
        while (1)
        {
            printf("Input balance: ");
            fgets(balancef, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(balancef) <= MAX_TEXT_LENGTH)
            {
                balancef[strcspn(balancef, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_float(balancef))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            if (strtof(balancef, NULL) > MAX_SAVINGS)
            {
                printf("Wrong value! - Number is too big.\n");
            }
            else
            {
                break;
            }
        }
    }
}

void get_savings(char *savingsf)
{
    while (1)
    {
        while (1)
        {
            printf("Input savings: ");
            fgets(savingsf, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(savingsf) <= MAX_TEXT_LENGTH)
            {
                savingsf[strcspn(savingsf, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_float(savingsf))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            if (strtof(savingsf, NULL) > MAX_SAVINGS)
            {
                printf("Wrong value! - Number is too big.\n");
            }
            else
            {
                break;
            }
        }
    }
}

void get_value(char *valuef)
{
    while (1)
    {
        while (1)
        {
            printf("Input value: ");
            fgets(valuef, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(valuef) <= MAX_TEXT_LENGTH)
            {
                valuef[strcspn(valuef, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_float(valuef))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            if (strtof(valuef, NULL) > MAX_SAVINGS)
            {
                printf("Wrong value! - Number is too big.\n");
            }
            else
            {
                break;
            }
        }
    }
}

void get_accnr(char *accnrf, int accNof)
{
    while (1)
    {
        while (1)
        {
            printf("Input account number: ");
            fgets(accnrf, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(accnrf) <= MAX_TEXT_LENGTH)
            {
                accnrf[strcspn(accnrf, "\n")] = 0;
                break;
            }
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        if (!is_int(accnrf))
        {
            printf("\nIncorrect input, try again.\n");
        }
        else
        {
            if (strtod(accnrf, NULL) > accNof - 1)
            {
                printf("Incorrect input, account number too big.\n");
            }
            else
            {
                break;
            }
        }
    }
}

void create_acc(int *accNof, FILE *fptr)
{
    char namef[MAX_TEXT_LENGTH_D], surnamef[MAX_TEXT_LENGTH_D], addressf[MAX_TEXT_LENGTH_D], peself[MAX_TEXT_LENGTH_D], balancef[MAX_TEXT_LENGTH_D], savingsf[MAX_TEXT_LENGTH_D];
    get_name(namef);
    get_surname(surnamef);
    get_address(addressf);
    get_pesel(peself);
    get_balance(balancef);
    get_savings(savingsf);
    CustAccount custf;
    memset(&custf, 0, sizeof(CustAccount));
    strcpy(custf.name, namef);
    strcpy(custf.surname, surnamef);
    strcpy(custf.address, addressf);
    strcpy(custf.pesel, peself);
    custf.balance = strtof(balancef, NULL);
    custf.savings = strtof(savingsf, NULL);
    if (fseek(fptr, (*accNof - 1) * sizeof(CustAccount), SEEK_SET))
    {
        perror("Error seeking in clients.dat");
        exit(3);
    }
    *accNof += 1;
    if (fwrite(&custf, sizeof(CustAccount), 1, fptr) != 1)
    {
        perror("Error during initialization of clients.dat");
        exit(2);
    }
    rewind(fptr);
}

void list(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to print accounts as none are yet saved.\n\n");
    }
    else
    {
        CustAccount custf;
        rewind(fptr);
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            print_cust(j, custf);
        }
        rewind(fptr);
    }
}

void search_acc_nr(int accNof, FILE *fptr)
{
    char searched_acc_nr[MAX_TEXT_LENGTH];
    CustAccount custf;
    rewind(fptr);
    get_accnr(searched_acc_nr, accNof);
    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (j == strtod(searched_acc_nr, NULL))
        {
            print_cust(j, custf);
            break;
        }
    }
    rewind(fptr);
}

void search_name(int accNof, FILE *fptr)
{
    char searched_name[MAX_TEXT_LENGTH];
    int number_found = 0;
    CustAccount custf;
    rewind(fptr);
    get_name(searched_name);
    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (strings_compare(custf.name, searched_name))
        {
            print_cust(j, custf);
            number_found += 1;
        }
    }
    if (number_found == 0)
    {
        printf("\n\nNo records were found.\n\n");
    }
    else
    {
        printf("%d records were found.", number_found);
    }
    rewind(fptr);
}

void search_surname(int accNof, FILE *fptr)
{
    char searched_surname[MAX_TEXT_LENGTH];
    int number_found = 0;
    CustAccount custf;
    get_name(searched_surname);
    rewind(fptr);
    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (strings_compare(custf.surname, searched_surname))
        {
            print_cust(j, custf);
            number_found += 1;
        }
    }
    if (number_found == 0)
    {
        printf("\n\nNo records were found.\n\n");
    }
    else
    {
        printf("%d records were found.", number_found);
    }
    rewind(fptr);
}

void search_address(int accNof, FILE *fptr)
{
    char searched_address[MAX_TEXT_LENGTH];
    int number_found = 0;
    CustAccount custf;
    get_address(searched_address);
    rewind(fptr);
    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (strings_compare(custf.address, searched_address))
        {
            print_cust(j, custf);
            number_found += 1;
        }
    }
    if (number_found == 0)
    {
        printf("\n\nNo records were found.\n\n");
    }
    else
    {
        printf("%d records were found.", number_found);
    }
    rewind(fptr);
}

void search_pesel(int accNof, FILE *fptr)
{
    char searched_pesel[MAX_TEXT_LENGTH];
    int number_found = 0;
    get_pesel(searched_pesel);
    CustAccount custf;
    rewind(fptr);
    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }
        if (strings_compare(custf.pesel, searched_pesel))
        {
            print_cust(j, custf);
            number_found += 1;
        }
    }
    if (number_found == 0)
    {
        printf("\n\nNo records were found.\n\n");
    }
    else
    {
        printf("%d records were found.", number_found);
    }
    rewind(fptr);
}

void search_user_input(char *user_inputf, int accNof, FILE *fptr)
{
    while (1)
    {
        printf("On what field would you like to search? account.number/name/surname/address/pesel\n");
        while (1)
        {
            printf("Input field: ");
            fgets(user_inputf, MAX_TEXT_LENGTH_D, stdin);
            if (strlen(user_inputf) <= MAX_TEXT_LENGTH)
            {
                user_inputf[strcspn(user_inputf, "\n")] = 0;
                break;
            }
            printf("ERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
        to_lower(user_inputf);
        if (strings_compare(user_inputf, "account.number") || strings_compare(user_inputf, "name") || strings_compare(user_inputf, "surname") || strings_compare(user_inputf, "address") | strings_compare(user_inputf, "pesel"))
        {
            break;
        }
        printf("\nWrong input! - try again.\n\n");
    }
}

void search(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n\n");
    }
    else
    {
        char user_inputf[MAX_TEXT_LENGTH];
        search_user_input(user_inputf, accNof, fptr);
        if (strings_compare(user_inputf, "account.number"))
        {
            search_acc_nr(accNof, fptr);
        }
        else if (strings_compare(user_inputf, "name"))
        {
            search_name(accNof, fptr);
        }
        else if (strings_compare(user_inputf, "surname"))
        {
            search_surname(accNof, fptr);
        }
        else if (strings_compare(user_inputf, "address"))
        {
            search_address(accNof, fptr);
        }
        else if (strings_compare(user_inputf, "pesel"))
        {
            search_pesel(accNof, fptr);
        }
        else
        {
            printf("\n\nERROR WITH INPUT");
            exit(1);
        }
    }
}

void deposit(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n\n");
    }
    else
    {
        char searched_acc_nr[MAX_TEXT_LENGTH_D];
        char dep_value[MAX_TEXT_LENGTH_D];
        CustAccount custf;
        rewind(fptr);
        get_accnr(searched_acc_nr, accNof);
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            if (j == strtod(searched_acc_nr, NULL))
            {
                print_cust(j, custf);
                break;
            }
        }
        while (1)
        {
            printf("\n\nHow much money would you like to deposit: \n");
            get_value(dep_value);
            if ((custf.balance - strtof(dep_value, NULL)) >= MAX_SAVINGS)
            {
                printf("\nUnable to deposit money, too much funds on the account.\n");
                break;
            }
            else
            {
                if (confirmation())
                {
                    custf.balance = custf.balance + strtof(dep_value, NULL);
                    if (fseek(fptr, (strtod(searched_acc_nr, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                    {
                        perror("Error seeking in clients.dat");
                        exit(3);
                    }

                    if (fwrite(&custf, sizeof(CustAccount), 1, fptr) != 1)
                    {
                        perror("Error reading from clients.dat");
                        exit(4);
                    }
                }
                break;
            }
        }
    }
}

void withdrawal(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n\n");
    }
    else
    {
        char searched_acc_nr[MAX_TEXT_LENGTH_D];
        char with_value[MAX_TEXT_LENGTH_D];
        CustAccount custf;
        rewind(fptr);
        get_accnr(searched_acc_nr, accNof);
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            if (j == strtod(searched_acc_nr, NULL))
            {
                print_cust(j, custf);
                break;
            }
        }
        while (1)
        {
            printf("\n\nHow much money would you like to withdrawal: \n");
            get_value(with_value);
            if ((custf.balance - strtof(with_value, NULL)) < DEBT_LIMIT)
            {
                printf("\nUnable to withdrawal money, not enough funds on the account.\n");
                break;
            }
            else
            {
                if (confirmation())
                {
                    custf.balance = custf.balance - strtof(with_value, NULL);
                    if (fseek(fptr, (strtod(searched_acc_nr, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                    {
                        perror("Error seeking in clients.dat");
                        exit(3);
                    }

                    if (fwrite(&custf, sizeof(CustAccount), 1, fptr) != 1)
                    {
                        perror("Error reading from clients.dat");
                        exit(4);
                    }
                }
                break;
            }
        }
    }
}

void transfer(int accNof, FILE *fptr)
{
    if (accNof <= 2)
    {
        printf("\nUnable to transfer as less then 2 accounts are created.\n\n");
    }
    else
    {
        char acc_from[MAX_TEXT_LENGTH_D];
        char acc_to[MAX_TEXT_LENGTH_D];
        char trans_value[MAX_TEXT_LENGTH_D];
        CustAccount custfrom;
        CustAccount custto;
        rewind(fptr);
        printf("\nInput the account number from which you want to transfer.\n");
        get_accnr(acc_from, accNof);
        printf("\nInput the account number to which you want to transfer.\n");
        get_accnr(acc_to, accNof);
        if (strings_compare(acc_from, acc_to))
        {
            printf("\nError, wrong input. 2 accounts are the same.\n");
            return;
        }
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custfrom, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            if (j == strtod(acc_from, NULL))
            {
                break;
            }
        }
        rewind(fptr);
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custto, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            if (j == strtod(acc_to, NULL))
            {
                break;
            }
        }
        while (1)
        {
            printf("\n\nHow much money would you like to transfer: \n");
            get_value(trans_value);
            if (custfrom.balance - strtof(trans_value, NULL) < DEBT_LIMIT)
            {
                printf("\nUnable to transfer money, too little funds on the from account.\n");
                break;
            }
            else if (custto.balance + strtof(trans_value, NULL) > MAX_SAVINGS)
            {
                printf("\nUnable to transfer money, too much funds on the to account.\n");
                break;
            }
            else
            {
                if (confirmation())
                {
                    custfrom.balance = custfrom.balance - strtof(trans_value, NULL);
                    if (fseek(fptr, (strtod(acc_from, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                    {
                        perror("Error seeking in clients.dat");
                        exit(3);
                    }
                    if (fwrite(&custfrom, sizeof(CustAccount), 1, fptr) != 1)
                    {
                        perror("Error reading from clients.dat");
                        exit(4);
                    }
                    custto.balance = custto.balance + strtof(trans_value, NULL);
                    if (fseek(fptr, (strtod(acc_to, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                    {
                        perror("Error seeking in clients.dat");
                        exit(3);
                    }
                    if (fwrite(&custto, sizeof(CustAccount), 1, fptr) != 1)
                    {
                        perror("Error reading from clients.dat");
                        exit(4);
                    }
                }
                break;
            }
        }
    }
}

void save(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n\n");
    }
    else
    {
        char to_from[MAX_TEXT_LENGTH_D];
        char searched_acc_nr[MAX_TEXT_LENGTH_D];
        char save_value[MAX_TEXT_LENGTH_D];
        CustAccount custf;
        rewind(fptr);
        get_accnr(searched_acc_nr, accNof);
        for (int j = 1; j < accNof; j++)
        {
            if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error reading from clients.dat");
                exit(2);
            }
            if (j == strtod(searched_acc_nr, NULL))
            {
                print_cust(j, custf);
                break;
            }
        }
        printf("\nWould you like to deposit to the savings account or withdrawal from the savings account? (to/from)\n");
        while (1)
        {
            while (1)
            {
                printf("Input what you want: ");
                fgets(to_from, MAX_TEXT_LENGTH_D, stdin);
                if (strlen(to_from) <= MAX_TEXT_LENGTH)
                {
                    to_from[strcspn(to_from, "\n")] = 0;
                    break;
                }
                printf("ERROR: Input too long, try again.\n\n");
                flush_stdin();
            }
            to_lower(to_from);
            if (strings_compare(to_from, "to") || strings_compare(to_from, "from"))
            {
                break;
            }
            printf("\nWrong input! - try again.\n");
        }
        if (strings_compare(to_from, "to"))
        {
            while (1)
            {
                printf("\n\nHow much money would you like to save: \n");
                get_value(save_value);
                if (custf.balance - strtof(save_value, NULL) < DEBT_LIMIT)
                {
                    printf("\nUnable to deposit money into savings, not enough funds on the account\n");
                    break;
                }
                else if (custf.savings + strtof(save_value, NULL) > MAX_SAVINGS)
                {
                    printf("\nUnable to deposit money into savings, too much money on the savings account\n");
                    break;
                }
                else
                {
                    if (confirmation())
                    {
                        custf.balance = custf.balance - strtof(save_value, NULL);
                        custf.savings = custf.savings + strtof(save_value, NULL);
                        if (fseek(fptr, (strtod(searched_acc_nr, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                        {
                            perror("Error seeking in clients.dat");
                            exit(3);
                        }

                        if (fwrite(&custf, sizeof(CustAccount), 1, fptr) != 1)
                        {
                            perror("Error reading from clients.dat");
                            exit(4);
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            while (1)
            {
                printf("\n\nHow much money would you like to withdrawal from savings: \n");
                get_value(save_value);
                if (custf.savings - strtof(save_value, NULL) < 0)
                {
                    printf("\nUnable to withdrawal money from savings, not enough funds on the savings account\n");
                    break;
                }
                else if (custf.balance + strtof(save_value, NULL) > MAX_SAVINGS)
                {
                    printf("\nUnable to withdrawal money from savings, too much money on the account\n");
                    break;
                }
                else
                {
                    if (confirmation())
                    {
                        custf.balance = custf.balance + strtof(save_value, NULL);
                        custf.savings = custf.savings - strtof(save_value, NULL);
                        if (fseek(fptr, (strtod(searched_acc_nr, NULL) - 1) * sizeof(CustAccount), SEEK_SET))
                        {
                            perror("Error seeking in clients.dat");
                            exit(3);
                        }

                        if (fwrite(&custf, sizeof(CustAccount), 1, fptr) != 1)
                        {
                            perror("Error reading from clients.dat");
                            exit(4);
                        }
                    }
                    break;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *fptr;
    // O accNo trzeba pomyslec to bylp wczesniej w pierwszym ifie
    int accNo = 1;
    if (!does_file_exist("clients.dat"))
    {
        CustAccount cust;
        memset(&cust, 0 , sizeof(CustAccount));
        if ((fptr = fopen("clients.dat", "w+b")) == NULL)
        {
            perror("Error opening clients.dat");
            exit(1);
        }
        for (int i = 0; i < ACCOUNTS; i++)
        {
            if (fwrite(&cust, sizeof(CustAccount), 1, fptr) != 1)
            {
                perror("Error during initialization of clients.dat");
                exit(2);
            }
        }
    }
    else
    {
        if ((fptr = fopen("clients.dat", "r+b")) == NULL)
        {
            perror("Error opening clients.dat");
            exit(3);
        }
        gett_accNo(&accNo, fptr);
    }
    char user_choice[21];
    printf("Hello user! Welcome to our bank :)\n\n");
    while (1)
    {
        choice_input(user_choice);
        if (strings_compare(user_choice, "close"))
        {
            break;
        }
        else if (strings_compare(user_choice, "create"))
        {
            create_acc(&accNo, fptr);
        }
        else if (strings_compare(user_choice, "list"))
        {
            list(accNo, fptr);
        }
        else if (strings_compare(user_choice, "search"))
        {
            search(accNo, fptr);
        }
        else if (strings_compare(user_choice, "deposit"))
        {
            deposit(accNo, fptr);
        }
        else if (strings_compare(user_choice, "withdrawal"))
        {
            withdrawal(accNo, fptr);
        }
        else if (strings_compare(user_choice, "transfer"))
        {
            transfer(accNo, fptr);
        }
        else if (strings_compare(user_choice, "save"))
        {
            save(accNo, fptr);
        }
        else
        {
            printf("\nWrong input, try again.\n");
        }
    }
    fclose(fptr);
    return 0;
}
