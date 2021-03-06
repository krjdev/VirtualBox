/* $Id: CloudGateway.cpp 93312 2022-01-18 13:15:12Z vboxsync $ */
/** @file
 * Implementation of local and cloud gateway management.
 */

/*
 * Copyright (C) 2019-2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#define LOG_GROUP LOG_GROUP_MAIN_CONSOLE

/* Make sure all the stdint.h macros are included - must come first! */
#ifndef __STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
# define __STDC_CONSTANT_MACROS
#endif

#include "LoggingNew.h"
#include "ApplianceImpl.h"
#include "CloudNetworkImpl.h"
#include "CloudGateway.h"

#include <iprt/http.h>
#include <iprt/inifile.h>
#include <iprt/net.h>
#include <iprt/path.h>
#include <iprt/vfs.h>
#include <iprt/uri.h>
#ifdef DEBUG
#include <iprt/file.h>
#include <VBox/com/utils.h>
#endif

#ifdef VBOX_WITH_LIBSSH
/* Prevent inclusion of Winsock2.h */
#define _WINSOCK2API_
#include <libssh/libssh.h>
#endif /* VBOX_WITH_LIBSSH */


static HRESULT setMacAddress(const Utf8Str& str, RTMAC& mac)
{
    int rc = RTNetStrToMacAddr(str.c_str(), &mac);
    if (RT_FAILURE(rc))
    {
        LogRel(("CLOUD-NET: Invalid MAC address '%s'\n", str.c_str()));
        return E_INVALIDARG;
    }
    return S_OK;
}


HRESULT GatewayInfo::setCloudMacAddress(const Utf8Str& mac)
{
    return setMacAddress(mac, mCloudMacAddress);
}


HRESULT GatewayInfo::setLocalMacAddress(const Utf8Str& mac)
{
    return setMacAddress(mac, mLocalMacAddress);
}


class CloudError
{
public:
    CloudError(HRESULT hrc, const Utf8Str& strText) : mHrc(hrc), mText(strText) {};
    HRESULT getRc() { return mHrc; };
    Utf8Str getText() { return mText; };

private:
    HRESULT mHrc;
    Utf8Str mText;
};


static void handleErrors(HRESULT hrc, const char *pszFormat, ...)
{
    if (FAILED(hrc))
    {
        va_list va;
        va_start(va, pszFormat);
        Utf8Str strError(pszFormat, va);
        va_end(va);
        LogRel(("CLOUD-NET: %s (rc=%x)\n", strError.c_str(), hrc));
        throw CloudError(hrc, strError);
    }

}


class CloudClient
{
public:
    CloudClient(ComPtr<IVirtualBox> virtualBox, const Bstr& strProvider, const Bstr& strProfile);
    ~CloudClient() {};

    void startCloudGateway(const ComPtr<ICloudNetwork> &network, GatewayInfo& gateway);
    void stopCloudGateway(const GatewayInfo& gateway);

private:
    ComPtr<ICloudProviderManager> mManager;
    ComPtr<ICloudProvider>        mProvider;
    ComPtr<ICloudProfile>         mProfile;
    ComPtr<ICloudClient>          mClient;
};


CloudClient::CloudClient(ComPtr<IVirtualBox> virtualBox, const Bstr& strProvider, const Bstr& strProfile)
{
    HRESULT hrc = virtualBox->COMGETTER(CloudProviderManager)(mManager.asOutParam());
    handleErrors(hrc, "Failed to obtain cloud provider manager object");
    hrc = mManager->GetProviderByShortName(strProvider.raw(), mProvider.asOutParam());
    handleErrors(hrc, "Failed to obtain cloud provider '%ls'", strProvider.raw());
    hrc = mProvider->GetProfileByName(strProfile.raw(), mProfile.asOutParam());
    handleErrors(hrc, "Failed to obtain cloud profile '%ls'", strProfile.raw());
    hrc = mProfile->CreateCloudClient(mClient.asOutParam());
    handleErrors(hrc, "Failed to create cloud client");
}


void CloudClient::startCloudGateway(const ComPtr<ICloudNetwork> &network, GatewayInfo& gateway)
{
    ComPtr<IProgress> progress;
    ComPtr<ICloudNetworkGatewayInfo> gatewayInfo;
    HRESULT hrc = mClient->StartCloudNetworkGateway(network, Bstr(gateway.mPublicSshKey).raw(),
                                                    gatewayInfo.asOutParam(), progress.asOutParam());
    handleErrors(hrc, "Failed to launch compute instance");
    hrc = progress->WaitForCompletion(-1);
    handleErrors(hrc, "Failed to launch compute instance (wait)");

    Bstr instanceId;
    hrc = gatewayInfo->COMGETTER(InstanceId)(instanceId.asOutParam());
    handleErrors(hrc, "Failed to get launched compute instance id");
    gateway.mGatewayInstanceId = instanceId;

    Bstr publicIP;
    hrc = gatewayInfo->COMGETTER(PublicIP)(publicIP.asOutParam());
    handleErrors(hrc, "Failed to get cloud gateway public IP address");
    gateway.mCloudPublicIp = publicIP;

    Bstr secondaryPublicIP;
    hrc = gatewayInfo->COMGETTER(SecondaryPublicIP)(secondaryPublicIP.asOutParam());
    handleErrors(hrc, "Failed to get cloud gateway secondary public IP address");
    gateway.mCloudSecondaryPublicIp = secondaryPublicIP;

    Bstr macAddress;
    hrc = gatewayInfo->COMGETTER(MacAddress)(macAddress.asOutParam());
    handleErrors(hrc, "Failed to get cloud gateway public IP address");
    gateway.setCloudMacAddress(macAddress);
}


void CloudClient::stopCloudGateway(const GatewayInfo& gateway)
{
    ComPtr<IProgress> progress;
    HRESULT hrc = mClient->TerminateInstance(Bstr(gateway.mGatewayInstanceId).raw(), progress.asOutParam());
    handleErrors(hrc, "Failed to terminate compute instance");
#if 0
    /* Someday we may want to wait until the cloud gateway has terminated. */
    hrc = progress->WaitForCompletion(-1);
    handleErrors(hrc, "Failed to terminate compute instance (wait)");
#endif
}


HRESULT startCloudGateway(ComPtr<IVirtualBox> virtualBox, ComPtr<ICloudNetwork> network, GatewayInfo& gateway)
{
    HRESULT hrc = S_OK;

    try {
        hrc = network->COMGETTER(Provider)(gateway.mCloudProvider.asOutParam());
        hrc = network->COMGETTER(Profile)(gateway.mCloudProfile.asOutParam());
        CloudClient client(virtualBox, gateway.mCloudProvider, gateway.mCloudProfile);
        client.startCloudGateway(network, gateway);
    }
    catch (CloudError e)
    {
        hrc = e.getRc();
    }

    return hrc;
}


#if 0 /* Disabled until proxy support is implemented */
static bool getProxyForIpAddr(ComPtr<IVirtualBox> virtualBox, const com::Utf8Str &strIpAddr, Bstr &strProxyType, Bstr &strProxyHost, Bstr &strProxyPort)
{
#ifndef VBOX_WITH_PROXY_INFO
    RT_NOREF(virtualBox, strIpAddr, strProxyType, strProxyHost, strProxyPort);
    LogRel(("CLOUD-NET: Proxy support is disabled. Using direct connection.\n"));
    return false;
#else /* VBOX_WITH_PROXY_INFO */
    ComPtr<ISystemProperties> systemProperties;
    ProxyMode_T enmProxyMode;
    HRESULT hrc = virtualBox->COMGETTER(SystemProperties)(systemProperties.asOutParam());
    if (FAILED(hrc))
    {
        LogRel(("CLOUD-NET: Failed to obtain system properties. hrc=%x\n", hrc));
        return false;
    }
    hrc = systemProperties->COMGETTER(ProxyMode)(&enmProxyMode);
    if (FAILED(hrc))
    {
        LogRel(("CLOUD-NET: Failed to obtain default machine folder. hrc=%x\n", hrc));
        return false;
    }
    if (enmProxyMode == ProxyMode_NoProxy)
        return false;

    Bstr proxyUrl;
    if (enmProxyMode == ProxyMode_Manual)
    {
        hrc = systemProperties->COMGETTER(ProxyURL)(proxyUrl.asOutParam());
        if (FAILED(hrc))
        {
            LogRel(("CLOUD-NET: Failed to obtain proxy URL. hrc=%x\n", hrc));
            return false;
        }
        Utf8Str strProxyUrl = proxyUrl;
        if (!strProxyUrl.contains("://"))
            strProxyUrl = "http://" + strProxyUrl;
        const char *pcszProxyUrl = strProxyUrl.c_str();
        RTURIPARSED Parsed;
        int rc = RTUriParse(pcszProxyUrl, &Parsed);
        if (RT_FAILURE(rc))
        {
            LogRel(("CLOUD-NET: Failed to parse proxy URL: %ls (rc=%d)\n", proxyUrl.raw(), rc));
            return false;
        }
        char *pszHost = RTUriParsedAuthorityHost(pcszProxyUrl, &Parsed);
        if (!pszHost)
        {
            LogRel(("CLOUD-NET: Failed to get proxy host name from proxy URL: %s\n", pcszProxyUrl));
            return false;
        }
        strProxyHost = pszHost;
        RTStrFree(pszHost);
        char *pszScheme = RTUriParsedScheme(pcszProxyUrl, &Parsed);
        if (!pszScheme)
        {
            LogRel(("CLOUD-NET: Failed to get proxy scheme from proxy URL: %s\n", pcszProxyUrl));
            return false;
        }
        strProxyType = Utf8Str(pszScheme).toUpper();
        RTStrFree(pszScheme);
        uint32_t uProxyPort  = RTUriParsedAuthorityPort(pcszProxyUrl, &Parsed);
        if (uProxyPort == UINT32_MAX)
        if (!pszScheme)
        {
            LogRel(("CLOUD-NET: Failed to get proxy port from proxy URL: %s\n", pcszProxyUrl));
            return false;
        }
        strProxyPort = BstrFmt("%d", uProxyPort);
    }
    else
    {
        /* Attempt to use system proxy settings (ProxyMode_System) */
        RTHTTP hHttp;
        int rc = RTHttpCreate(&hHttp);
        if (RT_FAILURE(rc))
        {
            LogRel(("CLOUD-NET: Failed to create HTTP context (rc=%Rrc)\n", rc));
            return false;
        }
        rc = RTHttpUseSystemProxySettings(hHttp);
        if (RT_FAILURE(rc))
        {
            LogRel(("CLOUD-NET: Failed to use system proxy (rc=%Rrc)\n", rc));
            RTHttpDestroy(hHttp);
            return false;
        }

        RTHTTPPROXYINFO proxy;
        rc = RTHttpQueryProxyInfoForUrl(hHttp, ("http://" + strIpAddr).c_str(), &proxy);
        if (RT_FAILURE(rc))
        {
            LogRel(("CLOUD-NET: Failed to get proxy for %s (rc=%Rrc)\n", strIpAddr.c_str(), rc));
            RTHttpDestroy(hHttp);
            return false;
        }
        switch (proxy.enmProxyType)
        {
            case RTHTTPPROXYTYPE_NOPROXY:
                RTHttpFreeProxyInfo(&proxy);
                RTHttpDestroy(hHttp);
                return false;
            case RTHTTPPROXYTYPE_HTTP:
                strProxyType = "HTTP";
                break;
            case RTHTTPPROXYTYPE_HTTPS:
                strProxyType = "HTTPS";
                break;
            case RTHTTPPROXYTYPE_SOCKS4:
                strProxyType = "SOCKS4";
                break;
            case RTHTTPPROXYTYPE_SOCKS5:
                strProxyType = "SOCKS5";
                break;
            case RTHTTPPROXYTYPE_UNKNOWN:
            case RTHTTPPROXYTYPE_INVALID:
            case RTHTTPPROXYTYPE_END:
            case RTHTTPPROXYTYPE_32BIT_HACK:
                break;
        }
        AssertStmt(strProxyType.isNotEmpty(), LogRel(("CLOUD-NET: Unknown proxy type: %d\n", proxy.enmProxyType)));
        strProxyHost = proxy.pszProxyHost;
        if (proxy.uProxyPort != UINT32_MAX)
            strProxyPort.printf("%d", proxy.uProxyPort);
        RTHttpFreeProxyInfo(&proxy);
        RTHttpDestroy(hHttp);
    }
    return true;
#endif /* VBOX_WITH_PROXY_INFO */
}
#endif

HRESULT stopCloudGateway(ComPtr<IVirtualBox> virtualBox, GatewayInfo& gateway)
{
    if (gateway.mGatewayInstanceId.isEmpty())
        return S_OK;

    LogRel(("CLOUD-NET: Terminating cloud gateway instance '%s'...\n", gateway.mGatewayInstanceId.c_str()));

    HRESULT hrc = S_OK;
    try {
        CloudClient client(virtualBox, gateway.mCloudProvider, gateway.mCloudProfile);
        client.stopCloudGateway(gateway);
#if 0
# ifdef DEBUG
        char szKeyPath[RTPATH_MAX];

        int rc = GetVBoxUserHomeDirectory(szKeyPath, sizeof(szKeyPath), false /* fCreateDir */);
        if (RT_SUCCESS(rc))
        {
            rc = RTPathAppend(szKeyPath, sizeof(szKeyPath), "gateway-key.pem");
            AssertRCReturn(rc, rc);
            rc = RTFileDelete(szKeyPath);
            if (RT_FAILURE(rc))
                LogRel(("WARNING! Failed to delete private key %s with rc=%d\n", szKeyPath, rc));
        }
        else
            LogRel(("WARNING! Failed to get VirtualBox user home directory with '%Rrc'\n", rc));
# endif /* DEBUG */
#endif
    }
    catch (CloudError e)
    {
        hrc = e.getRc();
        LogRel(("CLOUD-NET: Failed to terminate cloud gateway instance (rc=%x).\n", hrc));
    }
    gateway.mGatewayInstanceId.setNull();
    return hrc;
}


HRESULT generateKeys(GatewayInfo& gateway)
{
#ifndef VBOX_WITH_LIBSSH
    RT_NOREF(gateway);
    return E_NOTIMPL;
#else /* VBOX_WITH_LIBSSH */
    ssh_key single_use_key;
    int rc = ssh_pki_generate(SSH_KEYTYPE_RSA, 2048, &single_use_key);
    if (rc != SSH_OK)
    {
        LogRel(("Failed to generate a key pair. rc = %d\n", rc));
        return E_FAIL;
    }

    char *pstrKey = NULL;
    rc = ssh_pki_export_privkey_base64(single_use_key, NULL, NULL, NULL, &pstrKey);
    if (rc != SSH_OK)
    {
        LogRel(("Failed to export private key. rc = %d\n", rc));
        return E_FAIL;
    }
    gateway.mPrivateSshKey = pstrKey;
#if 0
# ifdef DEBUG
    char szConfigPath[RTPATH_MAX];

    rc = GetVBoxUserHomeDirectory(szConfigPath, sizeof(szConfigPath), false /* fCreateDir */);
    if (RT_SUCCESS(rc))
    {
        rc = RTPathAppend(szConfigPath, sizeof(szConfigPath), "gateway-key.pem");
        AssertRCReturn(rc, rc);
        rc = ssh_pki_export_privkey_file(single_use_key, NULL, NULL, NULL, szConfigPath);
        if (rc != SSH_OK)
        {
            LogRel(("Failed to export private key to %s with rc=%d\n", szConfigPath, rc));
            return E_FAIL;
        }
#  ifndef RT_OS_WINDOWS
        rc = RTPathSetMode(szConfigPath, RTFS_UNIX_IRUSR | RTFS_UNIX_IWUSR); /* Satisfy ssh client */
        AssertRCReturn(rc, rc);
#  endif
    }
    else
    {
        LogRel(("Failed to get VirtualBox user home directory with '%Rrc'\n", rc));
        return E_FAIL;
    }
# endif /* DEBUG */
#endif
    ssh_string_free_char(pstrKey);
    pstrKey = NULL;
    rc = ssh_pki_export_pubkey_base64(single_use_key, &pstrKey);
    if (rc != SSH_OK)
    {
        LogRel(("Failed to export public key. rc = %d\n", rc));
        return E_FAIL;
    }
    gateway.mPublicSshKey = Utf8StrFmt("ssh-rsa %s single-use-key", pstrKey);
    ssh_string_free_char(pstrKey);
    ssh_key_free(single_use_key);

    return S_OK;
#endif /* VBOX_WITH_LIBSSH */
}
