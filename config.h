// Ultrasonic Sensor
#define ULTRASONIC_INTERVALL 15
#define TRIGGER 23
#define ECHO 25
int distance;
int duration;

// Bluetooth data
int prevThrottle = 49;
int prevSteering = 49;
int throttle, steering, sliderVal, sliderId, button;
int automotive = 3; // starting in steering mode


// Engines
#define VELOCITY_L 3
#define ROTATIONDIR_L1 2
#define ROTATIONDIR_L2 4
#define ROTATIONDIR_R1 7
#define ROTATIONDIR_R2 8 
#define VELOCITY_R 9;

// Velocities
int velo_frontal = 80 + 175/2;
int velo_spin = 115;
int velo_slowturn;

// Servo
int angle = 35;
#define SERVO_INTERVALL 15
