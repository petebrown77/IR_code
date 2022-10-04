#include "SharpIR.h"

#define pin A7
#define model 100500
SharpIR sensor = SharpIR(pin, model);

//TODO, template this
class Queue {
  private:
    int length = 10;

  public:
    int buffer[10];
    Queue() {
      for (int i = 0; i < 10; i++) {
        buffer[i] = 0;
      }
    }
    void push(int value) {
      for (int i = 0; i < sizeof(buffer)/sizeof(int); i++) {
        buffer[i] = buffer[i+1];
      }

      buffer[9] = value;
    }

    int average() {
      int total = 0;
      for (int i = 0; i < 10; i++) {
        total += buffer[i];
      }

      return total / 10;
    }
    
    void print() {
      /*
        for (int i = 0; i < sizeof(buffer)/sizeof(int); i++) {
            std::cout << buffer[i] << " ";
        }
        std::cout << std::endl;
        */
    }
};

Queue queue;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SharpIR sensor = SharpIR(A7, 100500);
}

void loop() {
  // put your main code here, to run repeatedly:
  int distance = sensor.distance();
  queue.push(distance);

  int avg = queue.average();
  Serial.println(avg);
}
