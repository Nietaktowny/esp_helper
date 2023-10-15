#include "error.h"
#include <stdio.h>



char* err_to_name(int err) {

  char* cErrName;
  switch (err) {
  case ERR_NULL_POINTER:
    cErrName = "Passed null pointer";
    break;
  case ERR_NO_MEMORY:
    cErrName = "Memory allocation didn't work";
    break;
  default:
    cErrName = strerror(err);
    break;
  }

  return cErrName;
}