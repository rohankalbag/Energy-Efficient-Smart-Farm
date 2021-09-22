//Implementation of queue data structure
template <typename T> class queue{
    T* arr;
    // capacity of the queue
    int capacity;
    // capacity currently present in the queue
    int current;
public:
    // Default constructor to initialise
    // an initial capacity of 1 element and
    // allocating storage using dynamic allocation
    queue()
    {
        arr = new T[1];
        capacity = 1;
        current = 0;
    }
 
    // Function to add an element at the last
    void push(T data)
    {
        // if the number of elements is equal to the
        // capacity, that means we don't have space to
        // accommodate more elements. We need to double the
        // capacity
        if (current == capacity) {
            T* temp = new T[2 * capacity];
 
            // copying old array elements to new array
            for (int i = 0; i < capacity; i++) {
                temp[i] = arr[i];
            }
 
            // deleting previous array
            delete[] arr;
            capacity *= 2;
            arr = temp;
        }
        // Inserting data
        arr[current] = data;
        current++;
    }
 
    // function to add element at any index
    void push(int data, int index)
    {
        // if index is equal to capacity then this
        // function is same as push defined above
        if (index == capacity)
            push(data);
        else
            arr[index] = data;
    }
 
    // function to extract element at any index
    T get(int index)
    {
        // if index is within the range
        if (index < current)
            return arr[index];
    }
 
    // function to delete last element
    void pop() { current--; }
 
    // function to get size of the vector
    int size() { return current; }
 
    // function to get capacity of the vector
    int getcapacity() { return capacity; }
 
    // function to print array elements
    void print()
    {
        for (int i = 0; i < current; i++) {
            Serial.begin(9600);
            Serial.print(arr[i]);
        }
        Serial.println();
    }
};

int threshold = 300; //300 here is a dummy value, we calculate original value during calibration.
// Analog threshold which we can vary depending on the crop, If the moisture sensor value is more than this we need to switch on sprinkler
queue <int> solqueue;
// vector int queue to hold the solenoid numbers of the solenoids that need to be switched on
queue <int> senqueue;
// vector int queue to hold the sensor numbers numbers of the sensors of the trees which need to be watered
int pump_relay = 13;
// control for the 0.5-1V pump for the given row 
int solenoid_1 = 12;
int solenoid_2 = 11;
int solenoid_3 = 10;
int solenoid_4 = 9;
int solenoid_5 = 8;
int solenoid_6 = 7;
int solenoid_7 = 6;
//above lines initialize digital pin numbers which act as the contols of the solenoid valves S1 to S7 as per the schematic diagram

int sensor_1 = A7;
int sensor_2 = A6;
int sensor_3 = A5;
int sensor_4 = A4;
int sensor_5 = A3;
int sensor_6 = A2;
int sensor_7 = A1;
//above lines initialize analog pin numbers of the moisture sensors M1 to M7 as per the schematic diagram
unsigned long request_time = 30000; // 30000 = 30*1000 ms (30 s) time for which sprinkler is turned on


void setup() {
  //To assign input or output characteristics
  pinMode(solenoid_1,OUTPUT);
  pinMode(solenoid_2,OUTPUT);  
  pinMode(solenoid_3,OUTPUT);  
  pinMode(solenoid_4,OUTPUT);  
  pinMode(solenoid_5,OUTPUT);  
  pinMode(solenoid_6,OUTPUT);  
  pinMode(solenoid_7,OUTPUT); 
  
  pinMode(sensor_1,INPUT);  
  pinMode(sensor_2,INPUT);  
  pinMode(sensor_3,INPUT);  
  pinMode(sensor_4,INPUT);  
  pinMode(sensor_5,INPUT);  
  pinMode(sensor_6,INPUT);
  pinMode(sensor_7,INPUT);
        
}

void thresholdcheck(int solenoidno, int sensorno){
  if(analogRead(sensorno) >= threshold){
    //If the moisture sensor value is more than the threshold we need to switch on sprinkler
    solqueue.push(solenoidno);
    //add the request to turn on sprinkler into the two queues
    senqueue.push(sensorno);
    //push the data into the 2 queues
  }
}

bool continuecheck(int solenoidno, int sensorno){
  /* read the while(true) loop below to understand this*/
  if(analogRead(sensorno) >= threshold){
    return true;
    //the trees need more water hence we need to perform another watering cycle
    //return true and continue the iteration  
  }
  else{ 
    return false;
    //the trees are sufficiently watered, now break the loop and go to the next
    //return false and break the loop
  }
}


void loop() {
  // main iterative block  
  // a) sensing part
  //checking the dryness of the soil
  thresholdcheck(solenoid_1,sensor_1);
  thresholdcheck(solenoid_2,sensor_2); 
  thresholdcheck(solenoid_3,sensor_3); 
  thresholdcheck(solenoid_4,sensor_4); 
  thresholdcheck(solenoid_5,sensor_5); 
  thresholdcheck(solenoid_6,sensor_6); 
  thresholdcheck(solenoid_7,sensor_7);
  int sizeofqueue = solqueue.size();
  // b) request completion part
  //performing the requests until the queue is empty 
  while(sizeofqueue != 0){
    int currentsol = solqueue.get(sizeofqueue - 1);
    int currentsen = senqueue.get(sizeofqueue - 1);
    //get the value of the last element in the queue 
    solqueue.pop();
    senqueue.pop();
    //remove the last element in the queues
    sizeofqueue = solqueue.size();
    //get the new size of the queue
    while(true){
      if(continuecheck(currentsol,currentsen)){
        digitalWrite(pump_relay,HIGH); //Turn on the main pump
        digitalWrite(currentsol,HIGH); //Turn on the specific sprinkler with solenoid number currentsol
        delay(request_time); // Wait for 5 min
        digitalWrite(pump_relay,LOW);//Turn off the main pump
        digitalWrite(currentsol,LOW);//Turn off the specific sprinkler with solenoid number currentsol
      }
      else{
        break;
        }
    }
  }
}

