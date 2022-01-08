void setup () {
// initialize the serial communication:
	Serial.begin(500000);
	pinMode(14, INPUT);  // Setup for leads off detection LO +
	pinMode(12, INPUT);  // Setup for leads off detection LO -
}

#define AVGCNT 8
unsigned int avg(int *vals, unsigned char cnt) {
	int32_t tot;
	for (unsigned char i=0; i<cnt; i++) tot += vals[i];
	tot /= cnt;
	return (unsigned int) tot;
}

void loop () {
	static int slow=0;
	static int avv[AVGCNT];
	static int8_t  avvi=0;
	if ((digitalRead(10) == 1) || (digitalRead(11) == 1)) {
		Serial.println('!');
	} else {
		int v;
		v=analogRead(A0);
		avv[avvi] = v;
		if (++avvi >= AVGCNT) avvi=0;
		slow = avg(avv, AVGCNT);
		Serial.print(v);
		Serial.print('\t');
		Serial.println(slow);
		// send the value of analog input 0:
	}
			//Wait for a bit to keep serial data from saturating
	delay (1);
}
