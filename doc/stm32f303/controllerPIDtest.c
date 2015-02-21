// controllerPIDtest.ino
#define MANUAL 0
#define AUTOMATIC 1
#define DIRECT 0
#define REVERSE 1
#define RAD_PER_PULSE 0.006135923152
#define motorDrivePin 5
#define motorDirectionPin 7
#define motorEnablePin 8

/*working variables*/
unsigned long lastTime;
double Input, Output, Setpoint;
//input is current location
double ITerm, lastInput;
double kp, ki, kd;
int SampleTime = 1; //1 ms
double outMin, outMax;
//maximum and minimum output values for the PID
bool inAuto = false;
int controllerDirection = DIRECT;
volatile unsigned long count = 0;

void setup() {
pinMode(4, INPUT); //set the second encoder channel up
attachInterrupt(0, integrate, RISING);
Serial.begin(9600);
digitalWrite(motorEnablePin, LOW); //safety
digitalWrite(motorDirectionPin, LOW); //safety
digitalWrite(motorDrivePin, HIGH); //safety

//pid setup
SetMode(AUTOMATIC); //auto PID mode
SetTunings(1, 0, 0);
SetOutputLimits(-3, 3); //+-24V
SetSampleTime(5); //something smaller than the loop delay
Setpoint = 12.56;
digitalWrite(motorEnablePin, HIGH); //let it go
}

void loop() {
//run the PID at 10ms sample rate
delay(10);
Compute();
outputToPWM(Output);
}

void integrate() {
	if(digitalRead(4)) { //the second channel is leading the first, so direction = 1
		count++;
	}
	else if (count>0) {
		count--;
	}
}

void outputToPWM(float input) { //writes the direction and stuff 
	if (input < 0) {
		input*= -1; //make it positive
		digitalWrite(motorDirectionPin, LOW); //clockwise, negative
	}
	else {
		digitalWrite(motorDirectionPin, HIGH); //ccw, positive	
	}
	if (input > 24) input = 24; //constrain it to a correct value

	analogWrite(motorDrivePin, (int)(-10.625*input + 255)); //write the PWM
}

void Compute()
{
   if(!inAuto) return;
   unsigned long now = millis(); // I dont think this will exactly work?
   int timeChange = (now - lastTime);
   if(timeChange>=SampleTime)
   {
      /*Compute all the working error variables*/
      Input = count * RAD_PER_PULSE; //indicates current location in rads
      Serial.print("current position: ");
      Serial.println(Input);
      double error = Setpoint - Input;
      Serial.print("error: ");
      Serial.println(error);
      ITerm+= (ki * error);
      if(ITerm > outMax) ITerm= outMax;
      else if(ITerm < outMin) ITerm= outMin;
      double dInput = (Input - lastInput);
 
      /*Compute PID Output*/
      Output = kp * error + ITerm- kd * dInput;
      if(Output > outMax) Output = outMax;
      else if(Output < outMin) Output = outMin;
      Serial.print("output: ");
      Serial.println(Output);
      //also translate it to the correct direction/duty combo
 
      /*Remember some variables for next time*/
      lastInput = Input;
      lastTime = now;
   }
}
 
void SetTunings(double Kp, double Ki, double Kd)
{
   if (Kp<0 || Ki<0|| Kd<0) return;
 
  double SampleTimeInSec = ((double)SampleTime)/1000;
   kp = Kp;
   ki = Ki * SampleTimeInSec;
   kd = Kd / SampleTimeInSec;
 
  if(controllerDirection ==REVERSE)
   {
      kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
}
 
void SetSampleTime(int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)SampleTime;
      ki *= ratio;
      kd /= ratio;
      SampleTime = (unsigned long)NewSampleTime;
   }
}
 
void SetOutputLimits(double Min, double Max)
{
   if(Min > Max) return;
   outMin = Min;
   outMax = Max;
 
   if(Output > outMax) Output = outMax;
   else if(Output < outMin) Output = outMin;
 
   if(ITerm > outMax) ITerm= outMax;
   else if(ITerm < outMin) ITerm= outMin;
}
 
void SetMode(int Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto == !inAuto)
    {  /*we just went from manual to auto*/
        Initialize();
    }
    inAuto = newAuto;
}
 
void Initialize()
{
   lastInput = Input;
   ITerm = Output;
   if(ITerm > outMax) ITerm= outMax;
   else if(ITerm < outMin) ITerm= outMin;
}
 
void SetControllerDirection(int Direction)
{
   controllerDirection = Direction;
}