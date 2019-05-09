#include <System.h>

System sys({ "greenhouse", "mgs", "hotbed", "cbi", "gogolang"});

void setup(void) { sys.setup(); }
void loop(void)  { sys.loop();  }