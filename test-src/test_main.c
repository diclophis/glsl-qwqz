// test impl


#include "opengles_bridge.h"
#include "libqwqz.h"
#include "impl_main.h"
#include "test_main.h"


static qwqz_handle qwqz_engine = NULL;


void impl_draw() {
  qwqz_draw(qwqz_engine);
}


void impl_resize(int width, int height) {
  qwqz_resize(qwqz_engine, (float)width, (float)height);
}


int impl_main(int argc, char** argv) {
  if (argc == 3) {
    qwqz_engine = qwqz_create(argv[1], argv[2]);
    return 0;
  }

  return 1;
}
