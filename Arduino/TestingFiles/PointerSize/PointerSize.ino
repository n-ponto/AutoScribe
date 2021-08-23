// Figure out the size of pointers on the Arduino UNO

#define QUEUE_SIZE 256

typedef struct instructionQueue
{
    uint8_t queue[QUEUE_SIZE];
    struct instructionQueue *next;
} InstructionQueue;

void setup()
{
    Serial.begin(9600);
    double d = 12.3;
    int i = 45;
    long l = 6789;
    InstructionQueue *iq = new InstructionQueue;

    double *dptr = &d;
    int *iptr = &i;
    long *lprt = &l;

    String dblStr = "Double\tvalue: " + String(d) + "\tsize: " + String(int(sizeof(d))) + "\tpointersize: " + String(int(sizeof(dptr)));
    String intStr = "Int\tvalue: " + String(i) + "\tsize: " + String(int(sizeof(i))) + "\tpointersize: " + String(int(sizeof(iptr)));
    String lngStr = "Long\tvalue: " + String(l) + "\tsize: " + String(int(sizeof(l))) + "\tpointersize: " + String(int(sizeof(lprt)));

    Serial.println(dblStr);
    Serial.println(intStr);
    Serial.println(lngStr);
    Serial.println("InstructionQueue\tiq size: " + String(int(sizeof(iq))) + "\tderef iq size: " + String(int(sizeof(*iq))) + "\tInstructionQueue size: " + String(int(sizeof(InstructionQueue))) );
}

void loop() {
    
}
