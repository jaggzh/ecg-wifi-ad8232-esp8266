void setup_wifi(void);

 // Can be called any time, but it won't do any processing except after 3s,
 // and every 3s thereafter.  (Pass it the
 // the passed time and millis() it will start to
 // print status.
 // It does not wait or delay.
 // There's no need to call this a billion times so you
 // Returns:
 //   true if connected
 //   false if not yet connected
 // Outputs to serial on connect. Usually call in loop()
 // pass it the current millis as of that loop
 // It's not necessary -- it doesn't actually do anything
 // except let you see the SSID and IP upon connect
 //
 // Once connected it will remember and won't output
 // again until the status changes.
bool loop_check_wifi();  // optional, for connection status Serial output
void loop_wifi();        // Required for loop updates

// Wait max of passed seconds for wifi
// Returns true immediately upon success
// False after timeout expires
bool setup_wait_wifi(int timeout_s=10);
