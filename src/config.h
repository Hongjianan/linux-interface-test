/*
 * config.h
 *
 *  Created on: 2014-12-4
 *      Author: hong
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define CONFIG_ABLE			1
#define CONFIG_DISABLE		0

/* backtrace */

/* cas */
#define CASBASEINGCC_CPP					CONFIG_DISABLE	/* cursesTest.cpp */

/* curses */
#define CURSESTEST_CPP						CONFIG_DISABLE	/* cursesTest.cpp */

/* env */
#define ENVTEST_CPP							CONFIG_DISABLE	/* envTest.cpp */

/* error */
#define ERRORTEST_CPP						CONFIG_DISABLE	/* errorTest.cpp */

/* file */
#define FCNTLTEST_CPP						CONFIG_DISABLE	/* fcntlTest.cpp */
#define DIRTEST_CPP							CONFIG_DISABLE	/* dirTest.cpp */
#define FILETEST_CPP						CONFIG_DISABLE	/* fileTest.cpp */
#define SYNCTEST_CPP						CONFIG_DISABLE	/* syncTest.cpp */
#define FILECOPY_CPP						CONFIG_DISABLE	/* fileCopy.cpp */

/* fileANSI */
#define FILEANSITEST_CPP					CONFIG_DISABLE	/* fileANSITest.cpp */
#define FILEEXIST_CPP						CONFIG_DISABLE	/* fileExist.cpp */
#define FILE_FD_TEST_CPP					CONFIG_DISABLE	/* fileFdTest.cpp */

/* getAuthority */
#define GETAUTHORITY_CPP					CONFIG_DISABLE	/* getAuthority.cpp */

/* getopt */
#define GETOPTTEST_CPP						CONFIG_DISABLE	/* getoptTest.cpp */
#define GETOPTLONGTEST_CPP					CONFIG_DISABLE	/* getoptlongTest.cpp */

/* icmp */
#define PING_CPP							CONFIG_DISABLE	/* ping.cpp */
#define ICMPTEST_CPP						CONFIG_DISABLE	/* icmpTest.cpp */

/* stdio */
#define PRINTFTEST_CPP						CONFIG_DISABLE	/* printfTest.cpp */
#define FREOPENTEST_CPP						CONFIG_DISABLE	/* preopenTest.cpp */

/* logger */
#define LOGGERROLL_CPP						CONFIG_DISABLE	/* loggerRoll.cpp */

/* mipsBacktrace */
#define MIPSBACKTRACETEST_CPP				CONFIG_DISABLE	/* mipsBacktraceTest.cpp */
#define MIPSREALIZEBACKTRACEBYSELF_CPP		CONFIG_DISABLE	/* mipsBacktraceByself.cpp */
#define MIPSBACKTRACEWHENRECVSIGSEGV_CPP	CONFIG_DISABLE	/* mipsBacktraceWhenRecvSIGSEGV.cpp */
#define MIPSGETPC_CPP						CONFIG_DISABLE	/* mipsGetPC.cpp */

/* mipsPtrace */
#define MIPSPTRACETEST_CPP					CONFIG_DISABLE	/* mipsPtraceTest.cpp */
#define MIPSWAITPIDTEST_CPP					CONFIG_DISABLE	/* mipsWaitpidTest.cpp */
#define MIPSPTRACEGETREG_CPP				CONFIG_DISABLE	/* mipsPtraceGetReg.cpp */

/* msgQueue */
#define MSGQUEUETEST_CPP					CONFIG_DISABLE	/* msgQueueTest.cpp */
#define MSGQUEUERECV_CPP					CONFIG_DISABLE	/* msgQueueRecv.cpp */
#define MSGQUEUEATTR_CPP					CONFIG_DISABLE	/* msgQueueAttr.cpp */
#define MSGQUEUESARONEPROCESS_CPP			CONFIG_DISABLE	/* msgQueueSAROneProcess.cpp */
#define MSGQUEUE_SCHED_TIME_CPP				CONFIG_DISABLE	/* msgQueueSchedTime.cpp */

/* pipe */
#define FIFOTEST_CPP						CONFIG_DISABLE	/* fifoTest.cpp */

/* poll */
#define POLL_TEST_CPP						CONFIG_DISABLE	/* pollTest.cpp */

/* prctl */
#define PRCTLTEST_CPP						CONFIG_DISABLE	/* prctlTest.cpp */

/* process */
#define WAITTEST_CPP						CONFIG_DISABLE	/* waitTest.cpp */
#define FORKTEST_CPP						CONFIG_DISABLE	/* forkTest.cpp */
#define CLONETEST_CPP						CONFIG_DISABLE	/* forkTest.cpp */
#define FORKFAIL_CPP						CONFIG_DISABLE	/* forkFail.cpp */
#define FATHERFORKANDWAIT_CPP				CONFIG_DISABLE	/* fatherforkandwait.cpp */

/* pthread */
#define PTHREADCANCELTEST_CPP				CONFIG_DISABLE	/* pthreadTest.cpp */
#define PTHREAD_KEY_TEST_CPP				CONFIG_ABLE	/* pthreadTest.cpp */

/* segfault */
#define SEGFAULTTEST_CPP					CONFIG_DISABLE	/* segfaultTest.cpp */

/* select */
#define SELECT_TEST_CPP						CONFIG_DISABLE	/* selectTest.cpp */

/* semaphore */
#define NAMESEMAPHOREWAITANDPOST_CPP		CONFIG_DISABLE	/* nameSemaphoreWaitAndPost.cpp */
#define SEMAPHORETAKEANDGIVE_CPP			CONFIG_DISABLE	/* semaphoreTakeAndGive.cpp */

/* shareMemory */
#define SHAREMEMORYTEST_CPP					CONFIG_DISABLE	/* shareMemoryTest.cpp */

/* signal */
#define SIGNALTEST_CPP						CONFIG_DISABLE	/* signalTest.cpp */
#define SIGNALCATCH_CPP						CONFIG_DISABLE	/* signaCatch.cpp */
#define ATEXITTEST_CPP						CONFIG_DISABLE	/* atexitTest.cpp */

/* sleep */
#define SLEEPTEST_CPP						CONFIG_DISABLE	/* sleepTest.cpp */

/* socket */
#define SOCKETTEST_CPP 						CONFIG_DISABLE	/* socketTest.cpp */
#define NETADDRTEST_CPP						CONFIG_DISABLE	/* netAddrTest.cpp */
#define GETHOSTTEST_CPP						CONFIG_DISABLE 	/* getHostTest.cpp */
#define SELECTTEST_CPP						CONFIG_DISABLE	/* selectTest.cpp */
#define GETLOCALIP_CPP						CONFIG_DISABLE	/* getLocalIP.cpp */

/* socketRaw */
#define RAWSOCKETUDPSENDTEST_CPP			CONFIG_DISABLE	/* rawSocketUdpSendTest.cpp */
#define RAWSOCKETDUMP_CPP					CONFIG_DISABLE	/* rawSocketDump.cpp */
#define RAWSOCKETARP_CPP					CONFIG_DISABLE	/* rawSocketArp.cpp */

/* stdio */
#define FGETSTEST_CPP 						CONFIG_DISABLE	/* fgetsTest.cpp */
#define FREOPENTEST_CPP						CONFIG_DISABLE	/* freopenTest.cpp */
#define PRINTFTEST_CPP						CONFIG_DISABLE 	/* printfTest.cpp */

/* string */
#define STRINGTEST_CPP 						CONFIG_DISABLE	/* stringTest.cpp */

/* sync */
#define SPINLOCK_CPP 						CONFIG_DISABLE	/* spinlock.cpp */
#define MUTEXTEST_CPP						CONFIG_DISABLE	/* mutexTest.cpp */
#define RESEARCH_PTHREAD_MUTEX_T_CPP		CONFIG_DISABLE	/* research-pthread_mutext_t.cpp */
#define RECURSIVETEST_CPP					CONFIG_DISABLE	/* recursiveTest.cpp */

/* system */
#define SYSTEMTEST_CPP 						CONFIG_DISABLE	/* systemTest.cpp */

/* tcp */
#define TCPTEST_CPP							CONFIG_DISABLE	/* tcpTest.cpp */
#define TCPFILETRANSMITTER_CPP				CONFIG_DISABLE	/* fileTransmitter.cpp */
#define TCPCHATMULTIPROCESS_CPP				CONFIG_DISABLE	/* tcpChatMultiProcess.cpp */
#define TCPSELECT_CPP						CONFIG_DISABLE	/* tcpSelect.cpp */

/* time */
#define TIMETEST_CPP						CONFIG_DISABLE	/* timeTest.cpp */
#define TIMESTEST_CPP						CONFIG_DISABLE	/* timesTest.cpp */
#define REGTIME_CPP							CONFIG_DISABLE	/* regTime.cpp */

/* timer */
#define TIMERTEST_CPP						CONFIG_DISABLE	/* timerTest.cpp */

/* top */
#define TOPTEST_CPP							CONFIG_DISABLE	/*	topTest.cpp */

/* trace */
#define BACKTRACETEST_CPP					CONFIG_DISABLE	/* backtraceTest.cpp */
#define X86PTRACEATTACHTEST_CPP				CONFIG_DISABLE	/* ptraceAttachTest.cpp */
#define X86PTRACETEST_CPP					CONFIG_DISABLE	/* ptraceTest.cpp */

/* udp */
#define UDPCONCURRENCESERVER_CPP			CONFIG_DISABLE	/* udpConcurrenceServer.cpp */
#define UDPMULISENDONERECV_CPP				CONFIG_DISABLE	/* udpMuliSendOneRecv.cpp */
#define UDPSENDTEST_CPP						CONFIG_DISABLE	/* udpSendTest.cpp */
#define UDPORDERSENDRECV_CPP				CONFIG_DISABLE	/* udpOrderSendRecv.cpp */
#define UDPPACKAGETCP_CPP					CONFIG_DISABLE	/* udpPackageTcp.cpp */
#define UDPSEND_CPP							CONFIG_DISABLE	/* udpTest.cpp */
#define UDPRECVCHECKSUM_CPP					CONFIG_DISABLE	/* udpRecvCheckSum.cpp */

/* uname */
#define UNAMETEST_CPP						CONFIG_DISABLE	/* unameTest.cpp */

#endif /* CONFIG_H_ */
/* end file */
