// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "rocketeer_main.h"


static qwqz_handle qwqz_engine = NULL;


void impl_draw() {
}


void impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, (float)width, (float)height);
}


int impl_main(int argc, char** argv) {
  LOGV("impled\n");
  return 1;
}
