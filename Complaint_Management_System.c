#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int complaintID;
    char customerName[50];
    char complaint[200];
    char status[20];
    char timestamp[30];
} Complaint;

const char *FILENAME = "complaints.dat";
const char *ADMIN_PASSWORD = "admin123";

// Function prototypes
void registerComplaint();
void updateStatus();
void searchComplaint();
void viewAllComplaints();
void deleteComplaint();
void exportToCSV();
int generateComplaintID();
int adminLogin();

int main() {
    int choice;

    if (!adminLogin()) {
        printf("Access Denied.");
        return 0;
    }

    do {
        printf("\n--- Complaint Management System ---\n");
        printf("1. Register Complaint\n");
        printf("2. Update Complaint Status\n");
        printf("3. Search Complaint\n");
        printf("4. View All Complaints\n");
        printf("5. Delete Complaint\n");
        printf("6. Export to CSV\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: registerComplaint(); break;
            case 2: updateStatus(); break;
            case 3: searchComplaint(); break;
            case 4: viewAllComplaints(); break;
            case 5: deleteComplaint(); break;
            case 6: exportToCSV(); break;
            case 7: printf("Exiting...\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 7);

    return 0;
}

int adminLogin() {
    char pass[20];
    printf("Enter admin password: ");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = 0;
    return strcmp(pass, ADMIN_PASSWORD) == 0;
}

int generateComplaintID() {
    FILE *fp = fopen(FILENAME, "rb");
    Complaint c;
    int id = 1;

    if (fp != NULL) {
        while (fread(&c, sizeof(Complaint), 1, fp)) {
            if (c.complaintID >= id)
                id = c.complaintID + 1;
        }
        fclose(fp);
    }

    return id;
}

void registerComplaint() {
    FILE *fp = fopen(FILENAME, "ab");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    Complaint c;
    c.complaintID = generateComplaintID();

    printf("Enter customer name: ");
    fgets(c.customerName, sizeof(c.customerName), stdin);
    c.customerName[strcspn(c.customerName, "\n")] = 0;

    printf("Enter complaint description: ");
    fgets(c.complaint, sizeof(c.complaint), stdin);
    c.complaint[strcspn(c.complaint, "\n")] = 0;

    strcpy(c.status, "Pending");

    time_t now = time(NULL);
    strftime(c.timestamp, sizeof(c.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fwrite(&c, sizeof(Complaint), 1, fp);
    fclose(fp);

    printf("Complaint registered with ID: %d\n", c.complaintID);
}

void updateStatus() {
    FILE *fp = fopen(FILENAME, "rb+");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int id;
    printf("Enter Complaint ID to update: ");
    scanf("%d", &id);
    getchar();

    Complaint c;
    int found = 0;
    long pos;

    while (fread(&c, sizeof(Complaint), 1, fp)) {
        if (c.complaintID == id) {
            printf("Current status: %s\n", c.status);
            printf("Enter new status: ");
            fgets(c.status, sizeof(c.status), stdin);
            c.status[strcspn(c.status, "\n")] = 0;

            pos = ftell(fp) - sizeof(Complaint);
            fseek(fp, pos, SEEK_SET);
            fwrite(&c, sizeof(Complaint), 1, fp);

            printf("Status updated.\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Complaint ID not found.\n");

    fclose(fp);
}

void searchComplaint() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int id;
    char name[50];
    int choice, found = 0;

    printf("Search by:\n1. Complaint ID\n2. Customer Name\nChoose: ");
    scanf("%d", &choice);
    getchar();

    Complaint c;

    if (choice == 1) {
        printf("Enter Complaint ID: ");
        scanf("%d", &id);

        while (fread(&c, sizeof(Complaint), 1, fp)) {
            if (c.complaintID == id) {
                printf("\nID: %d\nName: %s\nComplaint: %s\nStatus: %s\nTime: %s\n",
                       c.complaintID, c.customerName, c.complaint, c.status, c.timestamp);
                found = 1;
                break;
            }
        }
    } else if (choice == 2) {
        printf("Enter Customer Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        while (fread(&c, sizeof(Complaint), 1, fp)) {
            if (strcasecmp(c.customerName, name) == 0) {
                printf("\nID: %d\nName: %s\nComplaint: %s\nStatus: %s\nTime: %s\n",
                       c.complaintID, c.customerName, c.complaint, c.status, c.timestamp);
                found = 1;
            }
        }
    }

    if (!found)
        printf("No matching complaint found.\n");

    fclose(fp);
}

void viewAllComplaints() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    Complaint c;
    printf("\n--- All Complaints ---\n");

    while (fread(&c, sizeof(Complaint), 1, fp)) {
        printf("ID: %d | Name: %s | Status: %s | Time: %s\n", c.complaintID, c.customerName, c.status, c.timestamp);
    }

    fclose(fp);
}

void deleteComplaint() {
    FILE *fp = fopen(FILENAME, "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (fp == NULL || temp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int id, found = 0;
    printf("Enter Complaint ID to delete: ");
    scanf("%d", &id);

    Complaint c;
    while (fread(&c, sizeof(Complaint), 1, fp)) {
        if (c.complaintID != id) {
            fwrite(&c, sizeof(Complaint), 1, temp);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);
    remove(FILENAME);
    rename("temp.dat", FILENAME);

    if (found)
        printf("Complaint deleted successfully.\n");
    else
        printf("Complaint ID not found.\n");
}

void exportToCSV() {
    FILE *fp = fopen(FILENAME, "rb");
    FILE *csv = fopen("complaints.csv", "w");

    if (fp == NULL || csv == NULL) {
        printf("Error opening file.\n");
        return;
    }

    Complaint c;
    fprintf(csv, "ComplaintID,CustomerName,Complaint,Status,Timestamp\n");

    while (fread(&c, sizeof(Complaint), 1, fp)) {
        fprintf(csv, "%d,%s,%s,%s,%s\n", c.complaintID, c.customerName, c.complaint, c.status, c.timestamp);
    }

    fclose(fp);
    fclose(csv);

    printf("Exported to complaints.csv\n");
}
