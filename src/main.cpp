#include <System.h>

System sys({ "greenhouse", "mgs", "hotbed-test", "INSIDIUS 2.4GHz", "scalable"});

void setup(void) { sys.setup(); }
void loop(void)  { sys.loop();  }