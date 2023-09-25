#ifndef GLDSYSINFOUTILS_H
#define GLDSYSINFOUTILS_H

#include <Windows.h>
#include <string>

    enum WindowsVersion
    {
        Windows,
        Windows32s,
        Windows95,
        Windows95OSR2,
        Windows98,
        Windows98SE,
        WindowsMillennium,
        WindowsNT351,
        WindowsNT40,
        WindowsNT40Server,
        Windows2000,
        WindowsXP,
        WindowsXPProfessionalx64,
        WindowsHomeServer,
        WindowsServer2003,
        WindowsServer2003R2,
        WindowsVista,
        WindowsServer2008,
        WindowsServer2008R2,
        Windows7,
        Windows8,
        WindowsServer2012,
        Windows8Point1,
        WindowsServer2012R2,
        Windows10,
        WindowsServer2016TechnicalPreview
    };

    enum WindowsEdition
    {
        EditionUnknown,                 // δ֪�汾

        Workstation,                    // ����վ
        Server,                         // �����
        AdvancedServer,                 // �߼������
        Home,                           // ��ͥ��

        Ultimate,                       // �콢��
        HomeBasic,                      // ��ͥ��ͨ��
        HomePremium,                    // ��ͥ�߼���
        Enterprise,                     // ��ҵ��
        HomeBasic_N,                    // ��ͥ��ͨN��
        Business,                       // ��ҵ��
        StandardServer,                 // ��׼�����
        DatacenterServer,               // �������ķ����
        SmallBusinessServer,            // С��ҵ�����
        EnterpriseServer,               // ��ҵ�����
        Starter,                        // �����
        DatacenterServerCore,           // �������ķ�����İ�
        StandardServerCore,             // ��׼������İ�
        EnterpriseServerCore,           // ��ҵ������İ�
        EnterpriseServerIA64,           // ��ҵ����IA64��
        Business_N,                     // ���ð�N
        WebServer,                      // web�����
        ClusterServer,                  // ��Ⱥ�����
        HomeServer,                     // ��ͥ�����
        StorageExpressServer,           // �洢��Я�����
        StorageStandardServer,          // �洢��׼�����
        StorageWorkgroupServer,         // �洢����������
        StorageEnterpriseServer,        // �洢��ҵ�����
        ServerForSmallBusiness,         // ������ҵ�����
        SmallBusinessServerPremium,     // ������ҵ����߼���
        HomePremium_N,                  // ������ҵ����߼���
        Enterprise_N,                   // ������ҵ����߼���
        Ultimate_N,                     // ������ҵ����߼���
        WebServerCore,                  // ������ҵ����߼���
        MediumBusinessServerManagement, // ������ҵ����߼���
        MediumBusinessServerSecurity,   // �м���ҵ����ȫ��
        MediumBusinessServerMessaging,  // �м���ҵ������Ϣ��
        ServerFoundation,               // �����վ
        HomePremiumServer,              // ��ͥ�߼������
        ServerForSmallBusiness_V,       // ������ҵ����V��
        StandardServer_V,               // ��׼����V��
        DatacenterServer_V,             // �������ķ���V��
        EnterpriseServer_V,             // ��ҵ����V��
        DatacenterServerCore_V,         // �������ķ������V��
        StandardServerCore_V,           // ��׼�������V��
        EnterpriseServerCore_V,         // ��ҵ�������V��
        HyperV,                         // ��V��
        StorageExpressServerCore,       // �洢����������İ�
        StorageStandardServerCore,      // �洢��׼������İ�
        StorageWorkgroupServerCore,     // �洢�����������İ�
        StorageEnterpriseServerCore,    // �洢��ҵ������İ�
        Starter_N,                      // ����N��
        Professional,                   // רҵ��
        Professional_N,                 // רҵN��
        SBSolutionServer,               // �ض����������
        ServerForSBSolution,            // ���Ʒ����
        StandardServerSolutions,        // ��׼�����ư�
        StandardServerSolutionsCore,    // ��׼�����ƺ��İ�
        SBSolutionServer_EM,            // ���Ʒ���EM��
        ServerForSBSolution_EM,         // ��V��
        SolutionEmbeddedServer,         // ��Ƕ��
        SolutionEmbeddedServerCore,     // ��Ƕ������İ�
        SmallBusinessServerPremiumCore, // ������ҵ������İ�
        EssentialBusinessServerMGMT,    // ������ҵ����MGMT��
        EssentialBusinessServerADDL,    // ������ҵ����ADDL��
        EssentialBusinessServerMGMTSVC, // ������ҵ����MGMTSVC��
        EssentialBusinessServerADDLSVC, // ������ҵ����ADDLSVC��
        ClusterServer_V,                // ��Ⱥ����V��
        Embedded,                       // ��V��
        Starter_E,                      // ����E��
        HomeBasic_E,                    // ��ͥ��ͨE��
        HomePremium_E,                  // ��ͥ�߼�E��
        Professional_E,                 // רҵE��
        Enterprise_E,                   // ��ҵE��
        Ultimate_E                      // �콢E��
    };

    /**
    * @brief ��ȡ����ϵͳ�汾
    * @return
    */
    WindowsVersion getWindowsVersion();

    /**
    * @brief ��ȡwindows�汾
    * @return
    */
    WindowsEdition getWindowsEdition();

    /**
    * @brief ��ȡƽ̨����
    * @return
    */
    std::string getPlatformType();

    /**
    * @brief ��ȡ���汾
    * @return
    */
    DWORD getMajorVersion();

    /**
    * @brief ��ȡ�ΰ汾��
    * @return
    */
    DWORD getMinorVersion();

    /**
    * @brief ��ȡBuilder Number
    * @return
    */
    DWORD getBuildNumber();

    /**
    * @brief ��ȡƽ̨ID
    * @return
    */
    DWORD getPlatformID();

    /**
    * @brief ��ȡService Pack��Ϣ
    * @return
    */
    std::string getServicePackInfo();

    /**
    * @brief �Ƿ���32λƽ̨
    * @return
    */
    bool is32bitPlatform();

    /**
    * @brief �Ƿ���64λƽ̨
    * @return
    */
    bool is64bitPlatform();

    /**
    * @brief ����Ƿ�װĳһ���
    * @param  �����
    * @return
    */
    bool checkIsInstalled(std::string name);


#endif // GLDSYSINFOUTILS_H
