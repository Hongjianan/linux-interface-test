/*
 * threadAttr.h
 *
 *  Created on: 2016-2-24
 *      Author: hongjianan
 */

#ifndef THREADATTR_H_
#define THREADATTR_H_

#include "baseTypedef.h"

STATUS SetSelfPrio(const Int32 prio);

STATUS UsdpaaBindSelfToCpu(const Uint32 cpuNo);
STATUS UsdpaaNoBindSelfToCpu(void);

#endif /* THREADATTR_H_ */
