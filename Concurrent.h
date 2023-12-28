template <typename Func, typename... Args>

// Make a function concurrent
// Function will be executed every interval (in milliseconds)
void concurrent(unsigned long interval, Func function, Args... args) {
  static unsigned long currentTime = millis();
  if (millis() - currentTime >= interval) {
    currentTime = millis();
    function(args...);
  }
}
