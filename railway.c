#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Passenger structure
struct passenger {
    char name[50];
    int age;
};

// Booking structure
struct booking {
    int booking_id;
    int train_no;
    int person_count;
    char from[30];
    char to[30];
    float fare_per_person;
    struct passenger persons[10];
};

// Function to remove newline from fgets input
void removenewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Function to generate unique 5-digit booking ID
int generatebookingID() {
    static int id = 10000;
    return ++id;
}

// Function to display all available trains
void viewtrains() {
    printf("\n--------------------------------------------------------------\n");
    printf("Train No | Train Name         | From   | To          | Fare\n");
    printf("--------------------------------------------------------------\n");
    printf("101      | Shatabdi Express   | Delhi  | Lucknow     | 650\n");
    printf("102      | Rajdhani Express   | Delhi  | Mumbai      | 950\n");
    printf("103      | Duronto Express    | Kolkata| Delhi       | 850\n");
    printf("104      | Tejas Express      | Delhi  | Chandigarh  | 500\n");
    printf("-------------------------------------------------------------\n");
}

// Function to get fare and stations by train number
void gettraindetails(int train_no, float *fare, char *from, char *to) {
    switch (train_no) {
        case 101: *fare = 650; strcpy(from, "Delhi"); strcpy(to, "Lucknow"); break;
        case 102: *fare = 950; strcpy(from, "Delhi"); strcpy(to, "Mumbai"); break;
        case 103: *fare = 850; strcpy(from, "Kolkata"); strcpy(to, "Delhi"); break;
        case 104: *fare = 500; strcpy(from, "Delhi"); strcpy(to, "Chandigarh"); break;
        default: *fare = 0; strcpy(from, "Unknown"); strcpy(to, "Unknown");
    }
}

// Function to print booking ticket
void printticket(struct booking b) {
    printf("\n---------------------------------\n");
    printf("           TRAIN TICKET\n");
    printf("---------------------------------\n");
    printf("Booking ID : %d\n", b.booking_id);
    printf("Train No   : %d\n", b.train_no);
    printf("From       : %s\n", b.from);
    printf("To         : %s\n", b.to);
    printf("Fare/Seat  : %.2f Rs\n", b.fare_per_person);
    printf("Total Fare : %.2f Rs\n", b.fare_per_person * b.person_count);
    printf("---------------------------------\n");
    printf("Passengers:\n");
    for (int i = 0; i < b.person_count; i++) {
        printf("%d. %s (Age %d)\n", i + 1, b.persons[i].name, b.persons[i].age);
    }
    printf("---------------------------------\n");
}

// Function to book ticket
void bookticket() {
    struct booking b;
    FILE *fp;
    char input[10];

    viewtrains();
    printf("\nEnter Train Number: ");
    fgets(input, sizeof(input), stdin);
    removenewline(input);
    b.train_no = atoi(input);

    if (b.train_no < 101 || b.train_no > 104) {
        printf("\nInvalid Train Number.\n");
        return;
    }

    printf("Enter Number of Passengers (max 10): ");
    fgets(input, sizeof(input), stdin);
    removenewline(input);
    b.person_count = atoi(input);

    if (b.person_count <= 0 || b.person_count > 10) {
        printf("\nInvalid number of passengers.\n");
        return;
    }

    // Take passenger details
    for (int i = 0; i < b.person_count; i++) {
        printf("\nEnter name of passenger %d: ", i + 1);
        fgets(b.persons[i].name, sizeof(b.persons[i].name), stdin);
        removenewline(b.persons[i].name);

        printf("Enter age of %s: ", b.persons[i].name);
        fgets(input, sizeof(input), stdin);
        removenewline(input);
        b.persons[i].age = atoi(input);
    }

    // Generate booking ID
    b.booking_id = generatebookingID();

    // Get train details and fare
    gettraindetails(b.train_no, &b.fare_per_person, b.from, b.to);

    // Save booking in file
    fp = fopen("bookings.txt", "ab");
    if (fp == NULL) {
        printf("\nError opening file.\n");
        return;
    }
    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);

    // Show confirmation
    printf("\nBooking Confirmed Successfully.\n");
    printticket(b);
}

// Function to search booking by booking ID
void searchbooking() {
    struct booking b;
    FILE *fp;
    char input[10];
    int id, found = 0;

    printf("\nEnter Booking ID to Search: ");
    fgets(input, sizeof(input), stdin);
    removenewline(input);
    id = atoi(input);

    fp = fopen("bookings.txt", "rb");
    if (fp == NULL) {
        printf("\nNo booking records found!\n");
        return;
    }

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.booking_id == id) {
            printf("\nBooking Found.\n");
            printticket(b);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found)
        printf("\nNo booking found for this Ticket ID\n");
}

// Function to cancel booking by booking ID
void cancelbooking() {
    struct booking b;
    FILE *fp, *temp;
    char input[10];
    int id, found = 0;
    char confirm[5];

    printf("\nEnter Booking ID to Cancel: ");
    fgets(input, sizeof(input), stdin);
    removenewline(input);
    id = atoi(input);

    fp = fopen("bookings.txt", "rb");
    temp = fopen("temp.txt", "wb");

    if (fp == NULL) {
        printf("\nNo booking records found.\n");
        return;
    }

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.booking_id == id) {
            found = 1;
            printf("\nBooking Found:\n");
            printticket(b);
            printf("Are you sure you want to cancel this ticket? (y/n): ");
            fgets(confirm, sizeof(confirm), stdin);
            if (confirm[0] != 'y' && confirm[0] != 'Y') {
                fwrite(&b, sizeof(b), 1, temp);
                printf("\nCancellation aborted. Ticket retained.\n");
            } else {
                printf("\nBooking canceled successfully\n");
            }
        } else {
            fwrite(&b, sizeof(b), 1, temp);
        }
    }

    fclose(fp);
    fclose(temp);
    remove("bookings.txt");
    rename("temp.txt", "bookings.txt");

    if (!found)
        printf("\nNo booking found for this Booking ID\n");
}

// View all bookings
void viewbookings() {
    struct booking b;
    FILE *fp = fopen("bookings.txt", "rb");

    if (fp == NULL) {
        printf("\nNo bookings found\n");
        return;
    }

    printf("\n------ All Bookings ------\n");
    while (fread(&b, sizeof(b), 1, fp)) {
        printticket(b);
    }
    fclose(fp);
}

// Main function
int main() {
    char input[10];
    int choice;

    while (1) {
        printf("\n--------------------------------\n");
        printf("  RAILWAY RESERVATION SYSTEM  \n");
        printf("--------------------------------\n");
        printf("1. View Train Details\n");
        printf("2. Book Ticket\n");
        printf("3. View All Bookings\n");
        printf("4. Search Booking by Booking ID\n");
        printf("5. Cancel Booking\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        switch (choice) {
            case 1: viewtrains(); break;
            case 2: bookticket(); break;
            case 3: viewbookings(); break;
            case 4: searchbooking(); break;
            case 5: cancelbooking(); break;
            case 6:
                printf("\nThank you for using the system. Have a nice journey.\n");
                exit(0);
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
    return 0;
}