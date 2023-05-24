#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
    int balance_dolars;
    int balance_cents;
    int savings_dolars;
    int savings_cents;
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
}

bool is_valid_string(char *word, const char *allowed_chars)
{
    int i;
    bool is_letter = false;
    for (i = 0; word[i]; i++)
    {
        if (!strchr(allowed_chars, word[i]) && word[i] != ' ')
        {
            return false;
        }
        if (isalpha(word[i]))
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

bool is_name(char *word)
{
    return is_valid_string(word, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

bool is_address(char *word)
{
    return is_valid_string(word, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/");
}

bool is_float(char *number)
{
    bool is_number = false;
    bool is_dot = false;
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
    if (number_of_decimal > 2 || i < 1 || (is_dot && number_of_decimal == 0))
    {
        return false;
    }
    return true;
}

bool is_int(char *number)
{
    for (int i = 0; number[i]; i++)
    {
        if (!isdigit(number[i]))
        {
            return false;
        }
    }

    return (number[0] != '\0');
}

bool is_pesel(char *pesel)
{
    int i;
    for (i = 0; i < 11; i++)
    {
        if (!isdigit(pesel[i]))
        {
            return false;
        }
    }

    return (i == 11 && pesel[i] == '\0');
}

void divide(char *numb, int *dollars, int *cents)
{
    char *dotPosition = strchr(numb, '.');
    if (dotPosition != NULL)
    {
        *dollars = atoi(numb);
        int length = strlen(dotPosition + 1);
        if (length == 1)
        {
            *cents = (dotPosition[1] - '0') * 10;
        }
        else
        {
            *cents = atoi(dotPosition + 1);
        }
    }
    else
    {
        *dollars = atoi(numb);
        *cents = 0;
    }
}

bool strings_compare(char *word_1, char *word_2)
{
    if (strlen(word_1) != strlen(word_2))
        return false;

    for (int j = 0; word_1[j]; j++)
    {
        if (tolower(word_1[j]) != tolower(word_2[j]))
            return false;
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

void print_cust(int numb, CustAccount custf)
{
    printf("\nNumber: %d | Name: %s | Surname: %s | Address: %s | Pesel: %s | Balance: %d.%d | Savings: %d.%d\n", numb, custf.name, custf.surname, custf.address, custf.pesel, custf.balance_dolars, custf.balance_cents, custf.savings_dolars, custf.savings_cents);
}

void get_input(char *input, const char *prompt, int max_length, bool (*validation_func)(char *), float max_value)
{
    while (1)
    {
        printf("\n%s: ", prompt);
        fgets(input, MAX_TEXT_LENGTH_D, stdin);
        if (strlen(input) <= MAX_TEXT_LENGTH)
        {
            input[strcspn(input, "\n")] = 0;
            if (validation_func(input))
            {
                if (max_value == -1 || atof(input) <= max_value)
                {
                    break;
                }
                else
                {
                    printf("Wrong value! - Number is too big.\n");
                }
            }
            else
            {
                printf("\nIncorrect input, try again.\n");
            }
        }
        else
        {
            printf("\n\nERROR: Input too long, try again.\n\n");
            flush_stdin();
        }
    }
}

void get_name(char *namef)
{
    get_input(namef, "Input name", MAX_TEXT_LENGTH_D, is_name, -1);
}

void get_surname(char *surnamef)
{
    get_input(surnamef, "Input surname", MAX_TEXT_LENGTH_D, is_name, -1);
}

void get_address(char *addressf)
{
    get_input(addressf, "Input address", MAX_TEXT_LENGTH_D, is_address, -1);
}

void get_pesel(char *peself)
{
    get_input(peself, "Input pesel", MAX_TEXT_LENGTH_D, is_pesel, -1);
}

void get_balance(char *balancef)
{
    get_input(balancef, "Input balance", MAX_TEXT_LENGTH_D, is_float, MAX_SAVINGS);
}

void get_savings(char *savingsf)
{
    get_input(savingsf, "Input savings", MAX_TEXT_LENGTH_D, is_float, MAX_SAVINGS);
}

void get_value(char *valuef)
{
    get_input(valuef, "Input value", MAX_TEXT_LENGTH_D, is_float, MAX_SAVINGS);
}

void get_accnr(char *accnrf, int accNof)
{
    get_input(accnrf, "Input account number", MAX_TEXT_LENGTH_D, is_int, accNof - 1);
}

void create_acc(int *accNof, FILE *fptr)
{
    char balancef[MAX_TEXT_LENGTH_D], savingsf[MAX_TEXT_LENGTH_D];
    CustAccount custf;
    memset(&custf, 0, sizeof(CustAccount));

    get_name(custf.name);
    get_surname(custf.surname);
    get_address(custf.address);
    get_pesel(custf.pesel);
    get_balance(balancef);
    get_savings(savingsf);
    divide(balancef, &custf.balance_dolars, &custf.balance_cents);
    divide(savingsf, &custf.savings_dolars, &custf.savings_cents);

    if (fseek(fptr, (*accNof - 1) * sizeof(CustAccount), SEEK_SET))
    {
        perror("Error seeking in clients.dat");
        exit(3);
    }

    (*accNof)++;

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
        if (strings_compare(user_inputf, "account.number") || strings_compare(user_inputf, "name") || strings_compare(user_inputf, "surname") || strings_compare(user_inputf, "address") | strings_compare(user_inputf, "pesel"))
        {
            break;
        }
        printf("\nWrong input! - try again.\n\n");
    }
}

void search_records(char *field, char *search_value, int accNof, FILE *fptr)
{
    CustAccount custf;
    int number_found = 0;
    rewind(fptr);

    for (int j = 1; j < accNof; j++)
    {
        if (fread(&custf, sizeof(CustAccount), 1, fptr) != 1)
        {
            perror("Error reading from clients.dat");
            exit(2);
        }

        if (strings_compare(field, "account.number"))
        {
            if (j == atoi(search_value))
            {
                print_cust(j, custf);
                number_found++;
                break;
            }
        }
        else if (strings_compare(field, "name"))
        {
            if (strings_compare(custf.name, search_value))
            {
                print_cust(j, custf);
                number_found++;
            }
        }
        else if (strings_compare(field, "surname"))
        {
            if (strings_compare(custf.surname, search_value))
            {
                print_cust(j, custf);
                number_found++;
            }
        }
        else if (strings_compare(field, "address"))
        {
            if (strings_compare(custf.address, search_value))
            {
                print_cust(j, custf);
                number_found++;
            }
        }
        else if (strings_compare(field, "pesel"))
        {
            if (strings_compare(custf.pesel, search_value))
            {
                print_cust(j, custf);
                number_found++;
            }
        }
    }
    if (number_found == 0)
    {
        printf("\nNo records were found.\n");
    }
    else
    {
        printf("%d records were found.\n", number_found);
    }

    rewind(fptr);
}

void search(int accNof, FILE *fptr)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n");
    }
    else
    {
        char user_input[MAX_TEXT_LENGTH_D];
        search_user_input(user_input, accNof, fptr);
        char search_value[MAX_TEXT_LENGTH_D];
        if (strings_compare(user_input, "account.number"))
        {
            get_accnr(search_value, accNof);
        }
        else if (strings_compare(user_input, "name") || strings_compare(user_input, "surname"))
        {
            get_name(search_value);
        }
        else if (strings_compare(user_input, "pesel"))
        {
            get_pesel(search_value);
        }
        else
        {
            get_address(search_value);
        }
        search_records(user_input, search_value, accNof, fptr);
    }
}

void transaction(int accNof, FILE *fptr, char *action)
{
    if (accNof <= 1)
    {
        printf("\nUnable to search accounts as none are yet saved.\n\n");
        return;
    }

    char searched_acc_nr[MAX_TEXT_LENGTH_D];
    char value[MAX_TEXT_LENGTH_D];
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
        printf("\n\nHow much money would you like to %s: \n", action);
        get_value(value);
        int dolars, cents;
        int help_var_1 = 0;
        int help_var_2 = 0;
        divide(value, &dolars, &cents);
        if (cents > custf.balance_cents)
        {
            help_var_1 = 1;
        }
        if (cents + custf.balance_cents > 100)
        {
            help_var_2 = 1;
        }
        if (strings_compare(action, "deposit"))
        {
            if ((custf.balance_dolars + dolars + help_var_2) >= MAX_SAVINGS)
            {
                printf("\nUnable to deposit money, too much funds on the account.\n");
                break;
            }
        }
        else if (strings_compare(action, "withdrawal"))
        {
            if ((custf.balance_dolars - dolars - help_var_1) < DEBT_LIMIT)
            {
                printf("\nUnable to withdraw money, not enough funds on the account.\n");
                break;
            }
        }

        if (confirmation())
        {
            if (strings_compare(action, "deposit"))
            {
                custf.balance_dolars = custf.balance_dolars + dolars + help_var_2;
                custf.balance_cents = (custf.balance_cents + cents) % 100;
            }
            else if (strings_compare(action, "withdrawal"))
            {
                custf.balance_dolars = custf.balance_dolars - dolars - help_var_1;
                if (help_var_1)
                {
                    custf.balance_cents += 100;
                }
                custf.balance_cents = custf.balance_cents - cents;
            }

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
            int dolars, cents;
            int help_var_1 = 0;
            int help_var_2 = 0;
            divide(trans_value, &dolars, &cents);
            if (cents > custfrom.balance_cents)
            {
                help_var_1 = 1;
            }
            if (cents + custto.balance_cents > 100)
            {
                help_var_2 = 1;
            }

            if ((custfrom.balance_dolars - dolars - help_var_1) < DEBT_LIMIT)
            {
                printf("\nUnable to transfer money, too little funds on the from account.\n");
                break;
            }
            else if ((custto.balance_dolars + dolars + help_var_2) >= MAX_SAVINGS)
            {
                printf("\nUnable to transfer money, too much funds on the to account.\n");
                break;
            }
            else
            {
                if (confirmation())
                {
                    custfrom.balance_dolars = custfrom.balance_dolars - dolars - help_var_1;
                    if (help_var_1)
                    {
                        custfrom.balance_cents += 100;
                    }
                    custfrom.balance_cents = custfrom.balance_cents - cents;
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
                    custto.balance_dolars = custto.balance_dolars + dolars + help_var_2;
                    custto.balance_cents = (custto.balance_cents + cents) % 100;
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
        bool is_confirmed;
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
                int dolars, cents;
                int help_var_1 = 0;
                int help_var_2 = 0;
                divide(save_value, &dolars, &cents);
                if (cents > custf.balance_cents)
                {
                    help_var_1 = 1;
                }
                if (cents + custf.savings_cents > 100)
                {
                    help_var_2 = 1;
                }
                if (custf.balance_dolars - dolars - help_var_1 < DEBT_LIMIT)
                {
                    printf("\nUnable to deposit money into savings, not enough funds on the account\n");
                    break;
                }
                else if (custf.savings_dolars + dolars + help_var_2 > MAX_SAVINGS)
                {
                    printf("\nUnable to deposit money into savings, too much money on the savings account\n");
                    break;
                }
                else
                {
                    if ((is_confirmed = confirmation()))
                    {
                        custf.savings_dolars = custf.savings_dolars + dolars + help_var_1;
                        custf.savings_cents = (custf.savings_cents + cents) % 100;
                        custf.balance_dolars = custf.balance_dolars - dolars - help_var_2;
                        if (help_var_1)
                        {
                            custf.balance_cents += 100;
                        }
                        custf.balance_cents = custf.balance_cents - cents;
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
                int dolars, cents;
                int help_var_1 = 0;
                int help_var_2 = 0;
                divide(save_value, &dolars, &cents);
                if (cents > custf.savings_cents)
                {
                    help_var_1 = 1;
                }
                if (cents + custf.balance_cents > 100)
                {
                    help_var_2 = 1;
                }
                if (custf.savings_dolars - dolars - help_var_1 < 0)
                {
                    printf("\nUnable to withdrawal money from savings, not enough funds on the savings account\n");
                    return;
                }
                else if (custf.balance_dolars + dolars + help_var_2 > MAX_SAVINGS)
                {
                    printf("\nUnable to withdrawal money from savings, too much money on the account\n");
                    return;
                }
                else
                {
                    if ((is_confirmed = confirmation()))
                    {
                        custf.savings_dolars = custf.savings_dolars - dolars - help_var_1;
                        custf.savings_cents = (custf.savings_cents - cents) % 100;
                        custf.balance_dolars = custf.balance_dolars + dolars + help_var_2;
                        custf.balance_cents = (custf.balance_cents + cents) % 100;
                    }
                    break;
                }
            }
        }
        if (is_confirmed)
        {
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
    }
}

void main_loop(int *accNo, FILE *fptr)
{
    char user_choice[MAX_TEXT_LENGTH];

    while (1)
    {
        choice_input(user_choice);

        if (strings_compare(user_choice, "close"))
        {
            break;
        }
        else if (strings_compare(user_choice, "create"))
        {
            create_acc(accNo, fptr);
        }
        else if (strings_compare(user_choice, "list"))
        {
            list(*accNo, fptr);
        }
        else if (strings_compare(user_choice, "search"))
        {
            search(*accNo, fptr);
        }
        else if (strings_compare(user_choice, "deposit"))
        {
            transaction(*accNo, fptr, "deposit");
        }
        else if (strings_compare(user_choice, "withdrawal"))
        {
            transaction(*accNo, fptr, "withdrawal");
        }
        else if (strings_compare(user_choice, "transfer"))
        {
            transfer(*accNo, fptr);
        }
        else if (strings_compare(user_choice, "save"))
        {
            save(*accNo, fptr);
        }
        else
        {
            printf("\nWrong input, try again.\n");
        }
    }
}

void gett_accNo(int *accNof, FILE *fptr)
{
    if(fseek(fptr, 0L, SEEK_END))
    {
        perror("Error seeking in clients.dat");
        exit(3);
    }
    long int res = ftell(fptr);
    *accNof = res/sizeof(CustAccount) + 1; 
}

void initialize_clients_file(int *accNof, FILE **fptr)
{
    if (!does_file_exist("clients.dat"))
    {
        CustAccount cust;
        memset(&cust, 0, sizeof(CustAccount));

        if ((*fptr = fopen("clients.dat", "w+b")) == NULL)
        {
            perror("Error opening clients.dat");
            exit(1);
        }
    }
    else
    {
        if ((*fptr = fopen("clients.dat", "r+b")) == NULL)
        {
            perror("Error opening clients.dat");
            exit(3);
        }
        gett_accNo(accNof, *fptr);
    }
}

int main(int argc, char *argv[])
{
    int accNo = 1;
    FILE *file_pointer;
    initialize_clients_file(&accNo, &file_pointer);
    printf("Hello user! Welcome to our bank :)\n\n");
    main_loop(&accNo, file_pointer);
    fclose(file_pointer);
    return 0;
}
