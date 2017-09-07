


int sensorPin = A0; // select the input pin for the input device

const int numberOfLEDs = 10;
int counter =0;
int ms_delay=100; // 10ms is to fast for small array
const int num_of_lit_up = 5;//  num of LED should be on at one time, instead of full length, shall look like moving bricks. 
int lit_up[num_of_lit_up];  // contain the pin numbers to be lit up

boolean has_empty_slots;

long loop_cnt = 0;

boolean upwards = true;

int last_idx = 0;

int off_count = 0;  // to detect if all light off;

void setup() {
  Serial.begin(9600);

  // setup pin for output
  for (int i=2 ; i<=11; i++){  //CODE_SMELL: magic number, easy to corrupt
    pinMode(i, OUTPUT);
  }

  // setting each pin to LOW so as not to light the LED
  for (int i=2 ; i<=11; i++){
    digitalWrite(i, LOW);
  }

  // init array, if the value of the element is not -1, then the corresponding LED should be lit up
  for (int i=0 ; i<num_of_lit_up && i < numberOfLEDs; i++){
    lit_up[i] = -1;
  }


  upwards = true;
  
  has_empty_slots = true;

  loop_cnt = -1;

}



boolean check_has_empty_slots(){
  boolean has_empty = false;
  for (int i = 0; i< num_of_lit_up; i++){
    if ( lit_up[i] == -1 ) {
      has_empty = true;
      break;
    }  
  }
  return has_empty;
}

boolean check_has_empty_slots_downwards(){
  boolean has_empty = false;
  for (int i = num_of_lit_up; i>0 ; i--){
    if ( lit_up[i] == -1 ) {
      has_empty = true;
      break;
    }  
  }
  return has_empty;
}


int get_current_max_pin_num() {
  int max_pin_num = 1; // 1:non-exist, 2 is the smallest for 2-11 arrangement 
  for (int i=0; i< num_of_lit_up; i++ ) {
    if ( lit_up[i] > max_pin_num ) {
      max_pin_num = lit_up[i];
    }
  }

  return max_pin_num;
}

int get_current_min_pin_num() {
  int min_pin_num = 12; // 12:non-exist, 11 is the larget for 2-11 arrangement 
  for (int i=0; i< num_of_lit_up; i++ ) {
    if ( lit_up[i] < min_pin_num && lit_up[i] != -1 ) {
      min_pin_num = lit_up[i];
    }
  }

  return min_pin_num;
}


void add_next_pin_downwards(){
  int min_pin_num  = get_current_min_pin_num();

//  Serial.print("mix_pin_num  ");
//  Serial.print(" : ");
//  Serial.print(min_pin_num);
//  Serial.println("");

  if ( min_pin_num > 0) {
    // find empty slot to fill in

    if ( check_has_empty_slots_downwards() ) {
      for (int i = 0; i< num_of_lit_up; i++){
        if ( lit_up[i] == -1 ) {
          lit_up[i] = min_pin_num-1; // May not work if reverse

          last_idx = i;
//          Serial.print("adding lit_up[ ");
//          Serial.print(i);
//          Serial.print(" ]");
//          Serial.print(" : ");
//          Serial.print(min_pin_num-1);
//          Serial.println("");
          break;
        }  
      }
    } else { // no empty slot
      // replace the oldest
      last_idx += 1;
      int idx = (last_idx) % num_of_lit_up;
      int digitToLow = lit_up[idx];
      digitalWrite(digitToLow, LOW);
      
      lit_up[idx] = min_pin_num-1 ;
      last_idx=idx;
      
//      Serial.print("replacing lit_up[ ");
//      Serial.print(idx);
//      Serial.print(" ]");
//      Serial.print(" : ");
//      Serial.print(min_pin_num-1);
//      Serial.println("");
    }

    
  } else {
    // reached 11 pin, not adding, but 
    // TODO: 
    Serial.println("unimplemented 3");

  }
}

void add_next_pin(){
  int max_pin_num  = get_current_max_pin_num();

//  Serial.print("max_pin_num  ");
//  Serial.print(" : ");
//  Serial.print(max_pin_num);
//  Serial.println("");
          
  if ( max_pin_num <= 11) {
    // find empty slot to fill in
     
    if ( check_has_empty_slots() ) {
      for (int i = 0; i< num_of_lit_up; i++){
        if ( lit_up[i] == -1 ) {
          lit_up[i] = max_pin_num+1; // May not work if reverse

//          Serial.print("adding lit_up[ ");
//          Serial.print(i);
//          Serial.print(" ]");
//          Serial.print(" : ");
//          Serial.print(max_pin_num+1);
//          Serial.println("");
          break;
        }  
      }
    } else { // no empty slot
      // replace the oldest
      int idx = (loop_cnt) % num_of_lit_up;
      int digitToLow = lit_up[idx];
      digitalWrite(digitToLow, LOW);
      
      lit_up[idx] = max_pin_num+1 ;
      
//      Serial.print("replacing lit_up[ ");
//      Serial.print(idx);
//      Serial.print(" ]");
//      Serial.print(" : ");
//      Serial.print(max_pin_num+1);
//      Serial.println("");
    }
    
  } else { // reached 11 pin, not adding, but 
    // TODO: 
    Serial.println("unimplemented 1");
  }
}


void on_off_step(){
   
   if ( upwards ) {
     int current_max_pin_num = get_current_max_pin_num();
     if  ( current_max_pin_num < 11 ) {
        // still can add next +1 pin number
         add_next_pin();
     
     } else {
        // reach max pin to lit up, considering turn-off previous one
  
        int oldest_idx = (loop_cnt ) % num_of_lit_up ;
        int digitPin = lit_up[oldest_idx];
        lit_up[oldest_idx] = -1;

        off_count += 1;
        digitalWrite(digitPin, LOW);
        
        
//        Serial.println("doing 2 .. ");

        if ( off_count - num_of_lit_up == 0 ) {
          // reset
          off_count = 0;
          // flip
          upwards = !upwards;
        }
     }
   } else {  // reverse orger
      int current_min_pin_num = get_current_min_pin_num();

      
       
      if  ( current_min_pin_num >2 ) {
        // still can add next -1 pin number
         add_next_pin_downwards();
     
       } else {
          // reach max pin to lit up, considering turn-off previous one
    
          int oldest_idx = (loop_cnt ) % num_of_lit_up ;
          int digitPin = lit_up[oldest_idx];
          lit_up[oldest_idx] = -1;

          off_count += 1;
          digitalWrite(digitPin, LOW);
          
//          Serial.println("doing 4 .. ");

          if ( off_count - num_of_lit_up == 0 ) {
            // reset
            off_count = 0;
            // flip
            upwards = !upwards;
          }
       }
     }
 
}

void loop() {

  loop_cnt += 1;
  /**

  Strategy 2: maintain an array of lit up pins
  
  **/

//  Serial.print(loop_cnt);
//  Serial.println("");

  // --------- changing status ---------


  on_off_step(); 

  
  // --------- display ----------
//  //check lit_up element
//  for (int i=0 ; i<=num_of_lit_up && i < numberOfLEDs; i++){
//    int digit = lit_up[i];
//    if (digit != -1 && digit >=2 && digit <=11 ) {
//      digitalWrite(digit, HIGH);  // TODO: digit should be 
//    } else {
//      digitalWrite(digit, LOW);
//    }
//  }
   int idx = loop_cnt % num_of_lit_up;
   int digit = lit_up[idx];
   if ( digit >=2 && digit <= 11) {
     digitalWrite(digit, HIGH);
   }


 

  delay(ms_delay);
  

  
  
  
}
