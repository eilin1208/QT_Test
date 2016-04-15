#ifndef __CONFIG_H__
#define __CONFIG_H__

#define PLATFORM_WINDOWS			1
#define PLATFORM_DSP_BIOS			2
#define PLATFORM_LINUX				3


#ifdef  LINUX
#include <stdint.h>
#define RUN_PLATFORM				PLATFORM_LINUX

#endif

#endif
