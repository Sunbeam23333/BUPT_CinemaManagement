#define MAX_CINEMA_NAME_LENGTH 100
#define MAX_HALL_TYPE_LENGTH 10
#define MAX_SHOW_TYPE_LENGTH 10
#define MAX_ID_LENGTH 10
#define MAX_NAME_LENGTH 50

typedef struct {
    char hallID[MAX_ID_LENGTH];
    int totalSeats;
    int availableSeats;
    char seatMap[MAX_CINEMA_NAME_LENGTH];  // Simplified representation of the seat layout
    char hallType[MAX_HALL_TYPE_LENGTH];  // E.g., "3D", "IMAX"
} Hall;

typedef struct {
    char showID[MAX_ID_LENGTH];
    char cinemaName[MAX_CINEMA_NAME_LENGTH];
    char hallID[MAX_ID_LENGTH];
    char movieName[MAX_NAME_LENGTH];
    char startTime[6];  // "hh:mm"
    char endTime[6];    // "hh:mm"
    int duration;       // in minutes
    int availableTickets;
    float price;
    char showType[MAX_SHOW_TYPE_LENGTH];  // E.g., "English", "Chinese"
    char discountInfo[MAX_CINEMA_NAME_LENGTH]; // Details of any discount
} Show;
