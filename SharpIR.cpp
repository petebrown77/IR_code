#ifdef Arduino
  #include "Arduino.h"
#elif defined(SPARK)
  #include "Particle.h"
  #include "math.h"
#endif
#include "SharpIR.h"

// Initialisation function
//  + irPin : is obviously the pin where the IR sensor is attached
//  + sensorModel is a int to differentiate the two sensor models this library currently supports:
//    > 1080 is the int for the GP2Y0A21Y and 
//    > 20150 is the int for GP2Y0A02YK and 
//    > 100500 is the long for GP2Y0A710K0F
//    The numbers reflect the distance range they are designed for (in cm)
SharpIR::SharpIR(int irPin, long sensorModel) {
  
    _irPin=irPin;
    _model=sensorModel;
    
    // Define pin as Input
    pinMode (_irPin, INPUT);
    
    #ifdef ARDUINO
      analogReference(DEFAULT);
    #endif
}

// Sort an array
void SharpIR::sort(int a[], int size) {
    for(int i=0; i<(size-1); i++) {
        bool flag = true;
        for(int o=0; o<(size-(i+1)); o++) {
            if(a[o] > a[o+1]) {
                int t = a[o];
                a[o] = a[o+1];
                a[o+1] = t;
                flag = false;
            }
        }
        if (flag) break;
    }
}

// Read distance and compute it
int SharpIR::distance() {

    int ir_val[NB_SAMPLE] = {};
    int distanceCM;
    float current;
    int median;


    for (int i=0; i<NB_SAMPLE; i++){
        // Read analog value
        ir_val[i] = analogRead(_irPin);
    }
    
    // Get the approx median
    #if USE_MEDOFMEDIANS
        median = medianOfMedians(ir_val, NB_SAMPLE);
    #else
        sort(ir_val, NB_SAMPLE);
        median = ir_val[NB_SAMPLE/2];
    #endif
    
    if (_model==1080) {
        
        // Different expressions required as the Photon has 12 bit ADCs vs 10 bit for Arduinos
        #ifdef ARDUINO
          distanceCM = 29.988 * pow(map(median, 0, 1023, 0, 5000)/1000.0, -1.173);
        #elif defined(SPARK)
          distanceCM = 29.988 * pow(map(median, 0, 4095, 0, 5000)/1000.0, -1.173);
        #endif

    } else if (_model==20150){

        // Previous formula used by  Dr. Marcal Casas-Cartagena
        // puntualDistance=61.573*pow(voltFromRaw/1000, -1.1068);
        
        // Different expressions required as the Photon has 12 bit ADCs vs 10 bit for Arduinos
        #ifdef ARDUINO
          distanceCM = 60.374 * pow(map(median, 0, 1023, 0, 5000)/1000.0, -1.16);
        #elif defined(SPARK)
          distanceCM = 60.374 * pow(map(median, 0, 4095, 0, 5000)/1000.0, -1.16);
        #endif

    } else if (_model==430){

        // Different expressions required as the Photon has 12 bit ADCs vs 10 bit for Arduinos
        #ifdef ARDUINO
          distanceCM = 12.08 * pow(map(median, 0, 1023, 0, 5000)/1000.0, -1.058);
        #elif defined(SPARK)
          distanceCM = 12.08 * pow(map(median, 0, 4095, 0, 5000)/1000.0, -1.058);
        #endif
        
    } else if (_model==215){

        // Different expressions required as the Photon has 12 bit ADCs vs 10 bit for Arduinos
        // puntualDistance=5.2819*pow(voltFromRaw/1000, -1.161);
        #ifdef ARDUINO
          distanceCM = 5.2819 * pow(map(median, 0, 1023, 0, 5000)/1000.0, -1.161);
        #elif defined(SPARK)
          distanceCM = 5.2819 * pow(map(median, 0, 4095, 0, 5000)/1000.0, -1.161);
        #endif
        
    } else if (_model==100500){
        
        #ifdef ARDUINO
          current = map(median, 0, 1023, 0, 5000);
        #elif defined(SPARK)
          current = map(median, 0, 4095, 0, 5000);
        #endif
        // use the inverse number of distance like in the datasheet (1/L)
        // y = mx + b = 137500*x + 1125 
        // x = (y - 1125) / 137500
        // Different expressions required as the Photon has 12 bit ADCs vs 10 bit for Arduinos
        if (current < 1400 || current > 3300) {
          //false data
          distanceCM = 0;
        } else {
          distanceCM = 1.0 / (((current - 1125.0) / 1000.0) / 137.5);
        }
    }

    return distanceCM;
}

int SharpIR::medianOfMedians(int a[], int size){
  int ans;
  int numMedians = size / 5;
  int* medians = new int[numMedians];
  for(int i = 0; i < numMedians; i++){
    partialSort(a, i * 5, i * 5 + 4);
    medians[i] = a[i * 5 + 2];
  }
  if(numMedians > 25){
    ans = medianOfMedians(medians, numMedians);
  }else{
    sort(medians, numMedians);
    ans = medians[numMedians/2];
  }
  delete [] medians;
  medians = nullptr;
  return ans;
}

// Sort a partial array
void SharpIR::partialSort(int a[], int min, int max) {
    int t;
    bool flag;
    for(int i=min; i<max; i++) {
        flag = true;
        for(int o=min; o<(max-i); o++) {
            if(a[o] > a[o+1]) {
                t = a[o];
                a[o] = a[o+1];
                a[o+1] = t;
                flag = false;
            }
        }
        if (flag) break;
    }
}
