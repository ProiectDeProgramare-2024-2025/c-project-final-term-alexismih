#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define MAX_CHANNELS 1000
#define MAX_USERS 1000
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"

typedef struct {
    char username[100];
    char password[100];
} User;

User users[MAX_USERS];
int userCount = 0;
int loggedInUserIndex = -1;

typedef struct {
    char name[100];
    int subscribers;
    float rating;
} Channel;

Channel channels[MAX_CHANNELS];
Channel allChannels[MAX_CHANNELS];
int allChannelCount = 0;
int channelCount = 0;

void header() {
    system(CLEAR);
    printf("--------------\n");
    printf("1 - Register\n");
    printf("2 - Login\n");
    printf("3 - Logout\n");
    printf("4 - Search Channel\n");
    printf("5 - Subscribe to Channel\n");
    printf("6 - Unsubscribe from Channel\n");
    printf("7 - Rate Channel\n");
    printf("8 - Display Top 25 Channels\n");
    printf("0 - Exit\n");
    printf("--------------\n");
}

void submenuHeader(const char *title) {
    system(CLEAR);
    printf("==== %s ====\n", title);
}
void loadUsersFromFile() {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (userCount >= MAX_USERS) break;

        User u;
        if (sscanf(line, "%99[^,],%99[^\n]", u.username, u.password) == 2) {
            users[userCount++] = u;
        }
    }

    fclose(file);
}

void loadAllChannels() {
    FILE *file = fopen("channel.txt", "r");
    if (file == NULL) return;

    char line[256];
    allChannelCount = 0;
    while (fgets(line, sizeof(line), file)) {
        if (allChannelCount >= MAX_CHANNELS) break;

        Channel ch;
        if (sscanf(line, "%99[^,],%d,%f", ch.name, &ch.subscribers, &ch.rating) == 3) {
            allChannels[allChannelCount++] = ch;
        }
    }

    fclose(file);
}

void saveAllChannelsToFile() {
    FILE *file = fopen("channel.txt", "w");
    if (file == NULL) return;

    for (int i = 0; i < allChannelCount; i++) {
        fprintf(file, "%s,%d,%.2f\n", allChannels[i].name, allChannels[i].subscribers, allChannels[i].rating);
    }

    fclose(file);
}


void waitForUser() {
    printf("Press Enter to return...");
    getchar(); getchar();
}

void registerUser() {
    submenuHeader("User Registration");

    if (userCount >= MAX_USERS) {
        printf("User limit reached.\n");
        waitForUser();
        return;
    }

    char username[100], password[100];
    getchar();
    printf("Enter username: ");
    scanf("%[^\n]", username);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists.\n");
            waitForUser();
            return;
        }
    }

    getchar();
    printf("Enter password: ");
    scanf("%[^\n]", password);

    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Error opening user file.\n");
        waitForUser();
        return;
    }

    fprintf(file, "%s,%s\n", username, password);
    fclose(file);

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    userCount++;

    printf("User '%s' registered successfully.\n", username);
    waitForUser();
}

void login() {
    submenuHeader("User Login");

    char username[100], password[100];
    getchar();
    printf("Enter username: ");
    scanf("%[^\n]", username);

    getchar();
    printf("Enter password: ");
    scanf("%[^\n]", password);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            loggedInUserIndex = i;
            printf("Login successful! Welcome, %s\n", username);
            waitForUser();
            return;
        }
    }

    printf("Invalid username or password.\n");
    waitForUser();
}

void logout() {
    submenuHeader("User Logout");
    if (loggedInUserIndex != -1) {
        printf("User '%s' logged out.\n", users[loggedInUserIndex].username);
        loggedInUserIndex = -1;
    } else {
        printf("No user is currently logged in.\n");
    }
    waitForUser();
}

void subscribeChannel() {
    submenuHeader("Subscribe to Channel");

    char channelName[100];
    int subscribers;
    float rating;

    getchar();
    printf("Enter channel name: ");
    scanf("%[^\n]", channelName);

    printf("Enter number of subscribers: ");
    scanf("%d", &subscribers);

    printf("Enter average rating (0.0 - 5.0): ");
    scanf("%f", &rating);

    if (rating < 0.0 || rating > 5.0) {
        printf("Invalid rating. Must be between 0.0 and 5.0\n");
        waitForUser();
        return;
    }

    FILE *file = fopen("subscribed_channels.txt", "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        waitForUser();
        return;
    }

    fprintf(file, "%s,%d,%.2f\n", channelName, subscribers, rating);
    fclose(file);

    printf("Channel '%s' subscribed successfully!\n", channelName);
    channelCount++;
    waitForUser();

}

void loadChannelsFromFile() {
    FILE *file = fopen("subscribed_channels.txt", "r");
    if (file == NULL) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (channelCount >= MAX_CHANNELS) break;

        Channel ch;
        if (sscanf(line, "%99[^,],%d,%f", ch.name, &ch.subscribers, &ch.rating) == 3) {
            channels[channelCount++] = ch;
        }
    }

    fclose(file);
}


void unsubscribeChannel() {
   submenuHeader("Unsubscribe from Channel");

    if (channelCount == 0) {
        printf("No channels to unsubscribe from.\n");
        waitForUser();
        return;
    }

    char nameToRemove[100];
    getchar();
    printf("Enter the name of the channel to unsubscribe from: ");
    scanf("%[^\n]", nameToRemove);

    int found = 0;
    for (int i = 0; i < channelCount; i++) {
        if (strcasecmp(channels[i].name, nameToRemove) == 0) {
            found = 1;
            for (int j = i; j < channelCount - 1; j++) {
                channels[j] = channels[j + 1];
            }
            channelCount--;
            break;
        }
    }

    if (found) {
        FILE *file = fopen("subscribed_channels.txt", "w");
        if (file == NULL) {
            printf("Error opening file to update subscriptions.\n");
            waitForUser();
            return;
        }

        for (int i = 0; i < channelCount; i++) {
            fprintf(file, "%s,%d,%.2f\n", channels[i].name, channels[i].subscribers, channels[i].rating);
        }

        fclose(file);

        printf("Unsubscribed from '%s' successfully.\n", nameToRemove);
    } else {
        printf("Channel '%s' not found in subscriptions.\n", nameToRemove);
    }

    waitForUser();
}

void rateChannel() {
    submenuHeader("Rate Channel");

    if (allChannelCount == 0) {
        printf("No channels available to rate.\n");
        waitForUser();
        return;
    }

    char name[100];
    float newRating;

    getchar();
    printf("Enter channel name to rate: ");
    scanf("%[^\n]", name);

    printf("Enter new rating (0.0 to 5.0): ");
    scanf("%f", &newRating);

    if (newRating < 0.0 || newRating > 5.0) {
        printf("Invalid rating. Must be between 0.0 and 5.0\n");
        waitForUser();
        return;
    }

    int found = 0;
    for (int i = 0; i < allChannelCount; i++) {
        if (strcasecmp(allChannels[i].name, name) == 0) {
            allChannels[i].rating = newRating;
            found = 1;
            break;
        }
    }

    if (found) {
        saveAllChannelsToFile();
        printf("Rating updated for channel '%s'.\n", name);
    } else {
        printf("Channel '%s' not found.\n", name);
    }

    waitForUser();
}

int compareByRating(const void *a, const void *b) {
    Channel *ch1 = (Channel *)a;
    Channel *ch2 = (Channel *)b;
    if (ch2->rating > ch1->rating) return 1;
    else if (ch2->rating < ch1->rating) return -1;
    return 0;
}

void displayTopChannels() {
    submenuHeader("Top 25 Channels");

    if (allChannelCount == 0) {
        printf("No channel data available.\n");
        waitForUser();
        return;
    }

    Channel sorted[MAX_CHANNELS];
    memcpy(sorted, allChannels, sizeof(Channel) * allChannelCount);
    qsort(sorted, allChannelCount, sizeof(Channel), compareByRating);

    int count = (allChannelCount < 25) ? allChannelCount : 25;
    for (int i = 0; i < count; i++) {
    printf("%d. " COLOR_CYAN "%s" COLOR_RESET " - Subscribers: " COLOR_YELLOW "%d" COLOR_RESET ", Rating: " COLOR_GREEN "%.2f" COLOR_RESET "\n",
       i + 1, sorted[i].name, sorted[i].subscribers, sorted[i].rating);
    }

    waitForUser();
}

void saveToFile(const char *data) {
    FILE *file = fopen("channel.txt", "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "%s\n", data);
    fclose(file);
}

void readFromFile() {
    FILE *file = fopen("channel.txt", "r");
    if (file == NULL) {
        printf("No data available.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    waitForUser();
}
void searchChannel() {
    submenuHeader("Search Channel");
    char searchName[100];
    printf("Enter the channel name to search for: ");
    getchar();
    scanf("%[^\n]", searchName);

    FILE *file = fopen("channel.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        waitForUser();
        return;
    }
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char channelName[100], subscribers[100], rating[100];

        if (sscanf(line, "%99[^,],%99[^,],%99[^\n]", channelName, subscribers, rating) == 3) {
            if (strcasecmp(channelName, searchName) == 0) {
                printf("Channel found!\n");
                printf("Channel Name: %s\n", channelName);
                printf("Subscribers: %s\n", subscribers);
                printf("Average Rating: %s\n", rating);
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        printf("Channel '%s' not found.\n", searchName);
    }

    fclose(file);
    waitForUser();
}

void menu(int op) {
    switch(op) {
    case 1:
        registerUser();
        break;
    case 2:
        login();
        break;
    case 3:
        logout();
        break;
    case 4:
        searchChannel();
        break;
    case 5:
        subscribeChannel();
        break;
    case 6:
        unsubscribeChannel();
        break;
    case 7:
        rateChannel();
        break;
    case 8:
        displayTopChannels();
        break;
    default:
        printf("EXIT!\n");
    }
}


int main() {
   int option;
    loadUsersFromFile();
    loadChannelsFromFile();
    loadAllChannels();
    do {
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while(option > 0 && option <= 8);
    return 0;
}
