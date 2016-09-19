/*
 * main.cpp
 *
 *  Created on: 2014-10-30
 *      Author: hong
 */
#include "config.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	int ret = 0;

	/* cas */
#if CASBASEINGCC_CPP
extern int CasBaseInGcc(int argc, char *argv[]);
	ret = CasBaseInGcc(argc, argv);
#endif

	/* env */
#if ENVTEST_CPP
extern int EnvTest(int argc, char *argv[]);
	ret = EnvTest(argc, argv);
#endif

	/* error */
#if ERRORTEST_CPP
extern int ErrorTest(int argc, char *argv[]);
	ret = ErrorTest(argc, argv);
#endif

	/* file */
#if DIRTEST_CPP
extern int DirTest(int argc, char *argv[]);
	ret = DirTest(argc, argv);
#endif

#if FCNTLTEST_CPP
extern int FcntlTest(int argc, char *argv[]);
	ret = FcntlTest(argc, argv);
#endif

#if FILETEST_CPP
extern int FileTest(int argc, char *argv[]);
	ret = FileTest(argc, argv);
#endif

#if SYNCTEST_CPP
extern int SyncTest(int argc, char *argv[]);
	ret = SyncTest(argc, argv);
#endif

#if FILECOPY_CPP
extern int FileCopy(int argc, char *argv[]);
	ret = FileCopy(argc, argv);
#endif

	/* fileANSI */
#if FILEANSITEST_CPP
extern int FileANSITest(int argc, char *argv[]);
	ret = FileANSITest(argc, argv);
#endif

#if FILEEXIST_CPP
extern int FileExist(int argc, char *argv[]);
	ret = FileExist(argc, argv);
#endif

#if FILE_FD_TEST_CPP
extern int FileFdTest(int argc, char *argv[]);
	ret = FileFdTest(argc, argv);
#endif

	/* icmp */
#if PING_CPP
extern int Ping(int argc, char *argv[]);
	ret = Ping(argc, argv);
#endif

	/* getopt */
#if GETAUTHORITY_CPP
extern int GetAuthority(int argc, char *argv[]);
	ret = GetAuthority(argc, argv);
#endif

	/* getopt */
#if GETOPTTEST_CPP
extern int GetoptTest(int argc, char *argv[]);
	ret = GetoptTest(argc, argv);
#endif

#if GETOPTLONGTEST_CPP
extern int GetoptlongTest(int argc, char *argv[]);
	ret = GetoptlongTest(argc, argv);
#endif

	/* logger */
#if LOGGERROLL_CPP
extern int LoggerRoll(int argc, char *argv[]);
	ret = LoggerRoll(argc, argv);
#endif

	/* mipsBacktrace */
#if MIPSBACKTRACETEST_CPP
extern int MipsBacktraceTest(int argc, char *argv[]);
	printf("[main][%p]\n", main);
	ret = MipsBacktraceTest(argc, argv);
#endif

#if MIPSBACKTRACEBYSELF_CPP
extern int MipsBacktraceByself(int argc, char *argv[]);
	ret = MipsBacktraceByself(argc, argv);
#endif

#if MIPSBACKTRACEWHENRECVSIGSEGV_CPP
extern int MipsBacktraceWhenRecvSIGSEGV(int argc, char *argv[]);
	ret = MipsBacktraceWhenRecvSIGSEGV(argc, argv);
#endif

#if MIPSGETPC_CPP
extern int MipsGetPC(int argc, char *argv[]);
	/* use asm get ra, fp, sp registers */
	Uint *__ra = NULL;
	Uint *__sp = NULL;
	Uint *__fp = NULL;

	__asm__ volatile("move %0, $29" : "=r"(__sp));	/* sp */
	__asm__ volatile("move %0, $30" : "=r"(__fp));	/* fp */
	__asm__ volatile("move %0, $31" : "=r"(__ra));	/* ra */

	printf("main sp=[0x%08X] *sp=[0x%08X]\n", __sp, *__sp);
	printf("main fp=[0x%08X] *fp=[0x%08X]\n", __fp, *__fp);
	printf("main ra=[0x%08X] ", __ra);
	PrintFuncName(__ra);
	printf("main addr [0x%08X]\n", main);
	printf("\n");

	ret = MipsGetPC(argc, argv);
#endif

	/* mipsPtrace */
#if MIPSPTRACETEST_CPP
extern int MipsPtraceTest(int argc, char *argv[]);
	ret = MipsPtraceTest(argc, argv);
#endif

#if MIPSWAITPIDTEST_CPP
extern int MipsWaitpidTest(int argc, char *argv[]);
	ret = MipsWaitpidTest(argc, argv);
#endif

#if MIPSPTRACEGETREG_CPP
extern int MipsPtraceGetReg(int argc, char *argv[]);
	ret = MipsPtraceGetReg(argc, argv);
#endif

	/* msgQueue */
#if MSGQUEUETEST_CPP
extern int MsgQueueTest(int argc, char *argv[]);
	ret = MsgQueueTest(argc, argv);
#endif

#if MSGQUEUERECV_CPP
extern int MsgQueueRecv(int argc, char *argv[]);
	ret = MsgQueueRecv(argc, argv);
#endif

#if MSGQUEUEATTR_CPP
extern int MsgQueueAttr(int argc, char *argv[]);
	ret = MsgQueueAttr(argc, argv);
#endif

#if MSGQUEUESARONEPROCESS_CPP
extern int MsgQueueSAROneProcess(int argc, char *argv[]);
	ret = MsgQueueSAROneProcess(argc, argv);
#endif

#if MSGQUEUE_SCHED_TIME_CPP
extern int MsgQueueSchedTime(int argc, char *argv[]);
	ret = MsgQueueSchedTime(argc, argv);
#endif

	/* pipe */
#if FIFOTEST_CPP
extern int FifoTest(int argc, char *argv[]);
	ret = FifoTest(argc, argv);
#endif

	/* pipe */
#if POLL_TEST_CPP
extern int PollTest(int argc, char *argv[]);
	ret = PollTest(argc, argv);
#endif

	/* prctl */
#if PRCTLTEST_CPP
extern int PrctlTest(int argc, char *argv[]);
	ret = PrctlTest(argc, argv);
#endif

	/* process */
#if FORKTEST_CPP
extern int ForkTest(int argc, char *argv[]);
	ret = ForkTest(argc, argv);
#endif

#if FORKFAIL_CPP
extern int ForkFail(int argc, char *argv[]);
	ret = ForkFail(argc, argv);
#endif

#if WAITTEST_CPP
extern int WaitTest(int argc, char *argv[]);
	ret = WaitTest(argc, argv);
#endif

#if CLONETEST_CPP
extern int CloneTest(int argc, char *argv[]);
	ret = CloneTest(argc, argv);
#endif

#if FATHERFORKANDWAIT_CPP
extern int FatherForkAndWait(int argc, char *argv[]);
	ret = FatherForkAndWait(argc, argv);
#endif

	/* pthread */
#if PTHREADCANCELTEST_CPP
extern int PthreadcancelTest(int argc, char *argv[]);
	ret = PthreadcancelTest(argc, argv);
#endif

#if PTHREAD_KEY_TEST_CPP
int PthreadKeyTest(int argc, char *argv[]);
	ret = PthreadKeyTest(argc, argv);
#endif

	/* segfault */
#if SEGFAULTTEST_CPP
extern int SegfaultTest(int argc, char *argv[]);
	ret = SegfaultTest(argc, argv);
#endif

	/* select */
#if SELECT_TEST_CPP
extern int SelectTest(int argc, char *argv[]);
	ret = SelectTest(argc, argv);
#endif

	/* semaphore */
#if NAMESEMAPHOREWAITANDPOST_CPP
extern int NameSemaphoreWaitAndPost(int argc, char *argv[]);
	ret = NameSemaphoreWaitAndPost(argc, argv);
#endif

#if SEMAPHORETAKEANDGIVE_CPP
extern int SemaphoreTakeAndGive(int argc, char *argv[]);
	ret = SemaphoreTakeAndGive(argc, argv);
#endif

	/* shareMemory */
#if SHAREMEMORYTEST_CPP
extern int ShareMemoryTest(int argc, char *argv[]);
	ret = ShareMemoryTest(argc, argv);
#endif

	/* signal */
#if SIGNALTEST_CPP
extern int SignalTest(int argc, char *argv[]);
	ret = SignalTest(argc, argv);
#endif

#if SIGNALCATCH_CPP
extern int SignalCatch(int argc, char *argv[]);
	ret = SignalCatch(argc, argv);
#endif

#if ATEXITTEST_CPP
extern int AtexitTest(int argc, char *argv[]);
	ret = AtexitTest(argc, argv);
#endif

	/* sleep */
#if SLEEPTEST_CPP
extern int SleepTest(int argc, char *argv[]);
	ret = SleepTest(argc, argv);
#endif

	/* socket */
#if SOCKETTEST_CPP
extern int SocketTest(int argc, char *argv[]);
	ret = SocketTest(argc, argv);
#endif

#if NETADDRTEST_CPP
extern int NetAddrTest(int argc, char *argv[]);
	ret = NetAddrTest(argc, argv);
#endif

#if GETHOSTTEST_CPP
extern int GetHostTest(int argc, char *argv[]);
	ret = GetHostTest(argc, argv);
#endif

#if GETLOCALIP_CPP
extern int GetLocalIP(int argc, char *argv[]);
	ret = GetLocalIP(argc, argv);
#endif

	/* socketRaw*/
#if RAWSOCKETUDPSENDTEST_CPP
extern int RawSocketUdpSendTest(int argc, char *argv[]);
	ret = RawSocketUdpSendTest(argc, argv);
#endif

#if RAWSOCKETDUMP_CPP
extern int RawSocketDump(int argc, char *argv[]);
	ret = RawSocketDump(argc, argv);
#endif

#if RAWSOCKETARP_CPP
extern int RawSocketArp(int argc, char *argv[]);
	ret = RawSocketArp(argc, argv);
#endif

	/* stdio */
#if FREOPENTEST_CPP
extern int FreopenTest(int argc, char *argv[]);
	ret = FreopenTest(argc, argv);
#endif

#if PRINTFTEST_CPP
extern int PrintfTest(int argc, char *argv[]);
	ret = PrintfTest(argc, argv);
#endif

#if FGETSTEST_CPP
extern int FgetsTest(int argc, char *argv[]);
	ret = FgetsTest(argc, argv);
#endif

	/* string */
#if STRINGTEST_CPP
extern int StringTest(int argc, char *argv[]);
	ret = StringTest(argc, argv);
#endif

	/* system */
#if SYSTEMTEST_CPP
extern int SystemTest(int argc, char *argv[]);
	ret = SystemTest(argc, argv);
#endif

	/* sync */
#if SPINLOCK_CPP
#endif

#if MUTEXTEST_CPP
extern int MutexTest(int argc, char *argv[]);
	ret = MutexTest(argc, argv);
#endif

#if RECURSIVETEST_CPP
extern int RecursiveTest(int argc, char *argv[]);
	ret = RecursiveTest(argc, argv);
#endif

#if RESEARCH_PTHREAD_MUTEX_T_CPP
extern int Research_pthread_mutex_t(int argc, char *argv[]);
	ret = Research_pthread_mutex_t(argc, argv);
#endif

	/* tcp */
#if TCPTEST_CPP
extern int TcpTest(int argc, char *argv[]);
	ret = TcpTest(argc, argv);
#endif

#if TCPFILETRANSMITTER_CPP
extern int TcpFileTransmitter(int argc, char *argv[]);
	ret = TcpFileTransmitter(argc, argv);
#endif

#if TCPCHATMULTIPROCESS_CPP
extern int TcpChatMultiProcess(int argc, char *argv[]);
	ret = TcpChatMultiProcess(argc, argv);
#endif

#if TCPSELECT_CPP
extern int TcpSelect(int argc, char *argv[]);
	ret = TcpSelect(argc, argv);
#endif

#if SELECTTEST_CPP
extern int SelectTest(int argc, char *argv[]);
	ret = SelectTest(argc, argv);
#endif

	/* time */
#if TIMETEST_CPP
extern int TimeTest(int argc, char *argv[]);
	ret = TimeTest(argc, argv);
#endif

#if TIMESTEST_CPP
extern int TimesTest(int argc, char *argv[]);
	ret = TimesTest(argc, argv);
#endif

#if REGTIME_CPP
extern int RegTime(int argc, char *argv[]);
	ret = RegTime(argc, argv);
#endif

	/* timer */
#if TIMERTEST_CPP
extern int Timer(int argc, char *argv[]);
	ret = RegTime(argc, argv);
#endif
	/* top */
#if TOPTEST_CPP
extern int TopTest(int argc, char *argv[]);
	ret = TopTest(argc, argv);
#endif

	/* udp */
#if UDPORDERSENDRECV_CPP
extern int UdpOrderSendRecv(int argc, char *argv[]);
	ret = UdpOrderSendRecv(argc, argv);
#endif

#if UDPCONCURRENCESERVER_CPP
extern int UdpConcurrenceServer(int argc, char *argv[]);
	ret = UdpConcurrenceServer(argc, argv);
#endif

#if UDPPACKAGETCP_CPP
extern int UdpPackageTcp(int argc, char *argv[]);
	ret = UdpPackageTcp(argc, argv);
#endif

#if UDPSEND_CPP
extern int UdpSend(int argc, char *argv[]);
	ret = UdpSend(argc, argv);
#endif

#if UDPRECVCHECKSUM_CPP
extern int UdpRecvCheckSum(int argc, char *argv[]);
	ret = UdpRecvCheckSum(argc, argv);
#endif

	/* uname */
#if UNAMETEST_CPP
extern int UnameTest(int argc, char *argv[]);
	ret = UnameTest(argc, argv);
#endif

	return ret;
}

/* end file */
