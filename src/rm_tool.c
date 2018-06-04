/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: rm_tool.c
**
** ��   ��   ��: Yang.Zhuang (��׳)
**
** �ļ���������: 2017 �� 12 �� 27 ��
**
** ��        ��: �ݹ�ɾ������
*********************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#define VERSION         "beta-1.0"                                      /*  �汾��                      */
/*********************************************************************************************************
**ȫ�ֱ���iRecursiveTimes������ݹ���ȹ����ջ���
*********************************************************************************************************/
static INT  iRecursiveTimes = 256;                                      /*  ���ݹ����                */
/*********************************************************************************************************
** ��������: usage
** ��������: ��ӡ����ʹ�÷���
** �䡡��  : NONE
** �䡡��  : ����ʹ�÷���
** ��  ��  : NONE
*********************************************************************************************************/
static VOID usage (VOID)
{
    fprintf(stderr,"rm_tool [-r R] <target>\n");

}
/*********************************************************************************************************
** ��������: __unlink_recursive
** ��������: ɾ���ļ������ļ��Ƕ��Ŀ¼ʱ���ݹ�ɾ���ļ�
** �䡡��  : pcFileName      �ļ���
** �䡡��  : NONE
** ��  ��  : ERROR_CODE
*********************************************************************************************************/
static INT __unlink_recursive (CPCHAR   pcFileName)
{
    struct stat    stFile;
    struct dirent *pdDirectoryInfo;
    DIR           *pdDir;
    CHAR           cPathLen[PATH_MAX];
    BOOL           bIsFail             =  LW_FALSE;

    /*
     *  �ж�Ŀ���ļ�����
     */
    if (lstat(pcFileName, &stFile) < 0) {
        return  (PX_ERROR);
    }

    /*
     *  ��Ŀ¼�Ļ�ֱ��ɾ��
     */
    if (!S_ISDIR(stFile.st_mode)) {
        unlink(pcFileName);

        return  (ERROR_NONE);
    }

    /*
     *  �жϵݹ����
     */
    if (0 == iRecursiveTimes) {
        fprintf(stderr,"NOTICE: Path is too long!!! \n");
        errno = EFBIG;
        return  (PX_ERROR);
    }

    /*
     *  �ļ���
     */
    pdDir = opendir(pcFileName);                                        /*  ��ȡ�Ѵ򿪵�Ŀ¼��Ϣ        */
    if (NULL == pdDir) {
        return  (PX_ERROR);
    }

    pdDirectoryInfo = readdir(pdDir);
    while (NULL != pdDirectoryInfo) {                                   /*  ��Ŀ¼���еݹ�              */
        iRecursiveTimes--;
        if (!strcmp(pdDirectoryInfo->d_name, "..") || !strcmp(pdDirectoryInfo->d_name, ".")) {
            continue;
        }

        sprintf(cPathLen, "%s/%s", pcFileName, pdDirectoryInfo->d_name);

        if (__unlink_recursive(cPathLen) < 0) {
            bIsFail = LW_TRUE;
            break;
        }
        pdDirectoryInfo = readdir(pdDir);
    }

    if (closedir(pdDir) < 0) {                                          /*  �ر�Ŀ¼���                */
        return  (PX_ERROR);
    }

    if (bIsFail) {
        return  (PX_ERROR);
    } else {
        rmdir(pcFileName);                                              /*  ɾ��Ŀ��Ŀ¼                */
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: main������
** ��������: �ݹ�ɾ���ļ�
** �䡡��  : ֱ����д�ļ�������ѡ��ݹ�ѡ����ļ���
** �䡡��  : NONE
** ��  ��  : ERROR_CODE
*********************************************************************************************************/
INT main (INT iArgc, CHAR *pcArgv[])
{
    INT  i          = 1;
    INT  iRet       = 0;
    BOOL bRecursive = LW_FALSE;

    if ((iArgc < 2) || (iArgc > 3)) {                                   /*  ���������ж�                */
        usage();

        return  (ERROR_NONE);
    }

    if ((iArgc >= 2) && (!strcmp(pcArgv[1], "-r") ||                     /*  �����жϣ��Ƿ�ݹ�ɾ��      */
                         !strcmp(pcArgv[1], "-R"))) {
        if (2 == iArgc) {                                               /*  �ų�ֻ�� -r �� -R ���������*/
            usage();

            return  (ERROR_NONE);
        }

        bRecursive = LW_TRUE;                                           /*  ���Ƿ�ݹ�Ϊ 1              */
        i          = 2;
    }

    for (; i < iArgc; i++) {                                            /*  ɾ������                    */
        iRet = bRecursive ? __unlink_recursive(pcArgv[i]) : unlink(pcArgv[i]);
        if (iRet < 0) {
            fprintf(stderr, "can not remove this file, error: %s\n", strerror(errno));

            return  (PX_ERROR);
        }
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
