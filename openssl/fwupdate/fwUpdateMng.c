/************  Copyright 2004-2016 FICOSA CORPORATIVE ELECTRONICS  ************
 | Language:       |  MISRA C
 | Controller:     |  AR755X
 | Spec. Document: |
 |                 |
 |                 |
 |                 |
 |-----------------|------------------------------------------------------------
 | Project:        |
 | Reference:      |
 |------------------------------------------------------------------------------
 | MODIFICATIONS HISTORY
 | Date     - Coder -    Description
 | 1/10/16   GG     Fixing code style.
 |------------------------------------------------------------------------------
 | DESCRIPTION:
 |
 ******************************************************************************/

/* ------------------------------- includes --------------------------------- */
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <regex.h>
#include "FwUpdateMng.h"
/*#include "interfaces.h"*/
#include "le_print.h"
#include "le_fwupdate_interface.h"

/* -------------------------------- Defines --------------------------------- */
#define  IMAGE_SIZE 6656 /*13*512*/
#define static 

/* ----------------------------- Data Types ------------------------------- */


const char update_pkg_file[] = "/mnt/update/update_pkg";

const char cert_filestr[] = "/home/root/secondary.crt";
EVP_PKEY *fwpdate_pkey = NULL;
char secondary_decode[SECONDARY_SIZE] = {0};
char secondary_encode[SECONDARY_SIZE] = {0};


char sig_buf[SIGNATURE] = {0};
char sig_buf_dst[4096] = {0};
char fwupate_version[512] = {0};

char *fwupdat_data = NULL;
size_t fwupdat_data_len = 0;
/* polyspace< JUST-MISRA-5.6 :we need struct stat No functional impact and no issue caused. : Justify > */
struct stat update_pk_sb;

const char Version_Pattern[] = "W([0-9]{1,2})([.])([0-9]{1,2})_RC([0-9]{2})([.])([0-9]){14}";

typedef struct
{
    le_thread_Ref_t fw_update_thread;
    le_timer_Ref_t fwupdate_Download_timer_ref;
    FwUpdate_staus fw_update_status;

} FwUpdateHandler;

static FwUpdateHandler FwUpdatetheApp;


/* --------------------------- Routine prototypes --------------------------- */


/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeTimeoutThread
|----------------------------------------------------------------------------
| Routine description:
|.Thread used to recover from problems connecting to a service, probably because the service is
|.down.  It will timeout after 20 seconds, print an error message, and then exit.
| Once successfully connected to the service, this thread should be stopped.
|---------------------------------------------------------------------------
| Parameters description:
| param_name: contextPtr
| result:
/---------------------------------------------------------------------------- */
static void* FwUpdateUpgradeTimeoutThread(void* contextPtr);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeTryConnect
|----------------------------------------------------------------------------
| Routine description:
|.TTry calling the given function to connect to a service.  If can't connect to the service within
|.20 seconds, then the program will exit.
|---------------------------------------------------------------------------
| Parameters description:
| param_name: connectFuncPtr, serviceNamePtr
| result:
/---------------------------------------------------------------------------- */
static void FwUpdateUpgradeTryConnect(ConnectServiceFunc_t connectFuncPtr, char* serviceNamePtr);

/*---------------------------------------------------------------------------
| Portability: FwUpdateDownloadFirmwareReq
|----------------------------------------------------------------------------
| Routine description:
|.Process the download firmware command
|---------------------------------------------------------------------------
| Parameters description:
| param_name: fileName
| result: le_result_t
| LE_OK if the download was successful
| LE_FAULT if there was an issue during the download process
/---------------------------------------------------------------------------- */
static le_result_t FwUpdateDownloadFirmwareReq(const char* fileName);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeStartReq
|----------------------------------------------------------------------------
| Routine description:
|.Firmware Update Req
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result: BOOL
| TRUE if the Firmware Update was successful
| FALSE if there was an issue during the Firmware Update process
/---------------------------------------------------------------------------- */
BOOL FwUpdateUpgradeStartReq(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeMngReq
|----------------------------------------------------------------------------
| Routine description:
|.Web Interface Firmware Update Req
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:
/---------------------------------------------------------------------------- */
void FwUpdateUpgradeMngReq(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeDisconnService
|----------------------------------------------------------------------------
| Routine description:
| Firware Update upgrade disconnect service
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *fwupdate_ptr
| result:void
/---------------------------------------------------------------------------- */
void FwUpdateUpgradeDisconnService( void* fwupdate_ptr);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeThread
|----------------------------------------------------------------------------
| Routine description:
| Firware Update upgrade thread
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *contextPtr
| result:void
/---------------------------------------------------------------------------- */
void* FwUpdateUpgradeThread(void* contextPtr);


/*---------------------------------------------------------------------------
| Portability: FwUpdatePrintIt
|----------------------------------------------------------------------------
| Routine description:
| Firware Update Pint message in hex format
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *label,*buff, fwupdate_len
| result:void
/---------------------------------------------------------------------------- */
static void FwUpdatePrintIt(const char* label, const char* buff, size_t fwupdate_len);

/*---------------------------------------------------------------------------
| Portability: FwUpdateBase64Decode
|----------------------------------------------------------------------------
| Routine description:
| Firware Update base decode
| openssl command: base64 -d
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *in_str,in_len, *out_str
| result:BOOL
/---------------------------------------------------------------------------- */
static BOOL FwUpdateBase64Decode(char *in_str, SI_32 in_len, char out_str[]);

/*---------------------------------------------------------------------------
| Portability: FwUpdateVerfySecCert
|----------------------------------------------------------------------------
| Routine description:
| Firware Update verify secondary certificate
| openssl commamd:openssl verify -CAfile cacert.pem secondary.crt
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:BOOL
/---------------------------------------------------------------------------- */
static BOOL FwUpdateVerfySecCert(void);

/*---------------------------------------------------------------------------
| Portability: FwExtracSecondaryKeys
|----------------------------------------------------------------------------
| Routine description:
| Firware Update Extracting secondary keys
| openssl commamd:openssl x509 -in secondary.crt -pubkey -noout > secondary_public.pem
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:BOOL
/---------------------------------------------------------------------------- */
static BOOL FwExtracSecondaryKeys(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdateRSAVerifySignature
|----------------------------------------------------------------------------
| Routine description:
| Firware Update Validating signature
| openssl commamd:openssl dgst -sha256 -verify secondary_public.pem -signature signature image.cwe
|---------------------------------------------------------------------------
| Parameters description:
| param_name: * msg_hash msg_hash_len *msg msg_len * authentic
| result:BOOL
/---------------------------------------------------------------------------- */
static SI_32 FwUpdateRSAVerifySignature( UI_8* msg_hash, size_t msg_hash_len, const char* msg, size_t msg_len, SI_32* authentic);

/*---------------------------------------------------------------------------
| Portability: FwUpdateCmpVersion
|----------------------------------------------------------------------------
| Routine description:
| FwUpdate Cmp Version compare the version number of the new file against the current SW version and verify that the new SW version is higher
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:BOOL
/---------------------------------------------------------------------------- */
static BOOL FwUpdateCmpVersion(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdateExtractSignatureImage
|----------------------------------------------------------------------------
| Routine description:
| Firware Update Extracting secondary keys signature image
| openssl commamd:dd if=update_pkg of=signature.64 bs=512 count=4
|---------------------------------------------------------------------------
| Parameters description:
| param_name: * msg_hash msg_hash_len *msg msg_len * authentic
| result:BOOL
/---------------------------------------------------------------------------- */
static BOOL FwUpdateExtractSignatureImage(void);
/*---------------------------------------------------------------------------
| Portability: FwUpdateVerifyReq
|----------------------------------------------------------------------------
| Routine description:
| Firware Update verify req
|1.Extracting secondary certificate
|     1> open updat file
|     2>read secondary.64 from file
|     3>decode base64 secondary.64 to secondary.crt
|2.Validating secondary certificate:
|3.Extracting secondary keys:
|4.Extracting signature
|     1>read  signature.64 from file
|     1>decode base64 signature.64 to signature
|5.Validating signature
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *fw_version
| result:SI_32
/---------------------------------------------------------------------------- */
FwUpdate_Verify_result FwUpdateVerifyReq(char fw_version[]);

/* -------------------------------- Routines -------------------------------- */

static void* FwUpdateUpgradeTimeoutThread(void* contextPtr)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    char* serviceNamePtr = contextPtr;

    /* This thread doesn't have to do anything else, at least for now, so just sleep.*/
    sleep((UI_32)10);


    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Error: can't connect to service; is %s running?\n", __FUNCTION__, __LINE__, serviceNamePtr);
    /*exit(EXIT_FAILURE);*/

    return NULL;
}


static void FwUpdateUpgradeTryConnect(ConnectServiceFunc_t connectFuncPtr, char* serviceNamePtr)
{
    /* Print out message before trying to connect to service to give user some kind of feedback */
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Connecting to service ..\n", __FUNCTION__, __LINE__);

    /* Use a separate thread for recovery.  It will be stopped once connected to the service.*/
    /* Make the thread joinable, so we can be sure the thread is stopped before continuing.*/
    le_thread_Ref_t threadRef = le_thread_Create("timout thread", FwUpdateUpgradeTimeoutThread, serviceNamePtr);
    le_thread_SetJoinable(threadRef);
    le_thread_Start(threadRef);

    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Connecting to service ..\n", __FUNCTION__, __LINE__);
    /* Try connecting to the service */
    connectFuncPtr();
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Connecting to service ..\n", __FUNCTION__, __LINE__);

    /* Connected to the service, so stop the timeout thread */
    le_thread_Cancel(threadRef);
    le_thread_Join(threadRef, NULL);
}


static le_result_t FwUpdateDownloadFirmwareReq(const char* fileName)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    le_result_t result = LE_FAULT;
    SI_32 download_fd;
    /*char buf[6658] = {0};*/

    /* Open the file */
    download_fd = open_new( fileName, O_RDONLY);
    /*download_fd = open( fileName, O_RDONLY);*/
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, download_fd = %d\n", __FUNCTION__, __LINE__, download_fd);

    /* becase image is start from 13*512 */
    lseek(download_fd, IMAGE_SIZE,0);
    /*int res = read(download_fd, buf, 6656);*/
    /*SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, res = %d\n", __FUNCTION__, __LINE__, res);*/

    if ( download_fd == -1 ) {
        /*Inform the user of the error; it's also useful to log this info*/
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "Can't open file '%s' : %m\n", fileName); /* %m replace strerror(errno) % */
        /*printf("Can't open file '%s' : %m\n", fileName);*/
        result = LE_FAULT;
    }
    else {

        FwUpdateUpgradeTryConnect(le_fwupdate_ConnectService, "fwupdateService");

        /*Connected to service so continue*/
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Download started ...\n", __FUNCTION__, __LINE__);

        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, download_fd = %d\n", __FUNCTION__, __LINE__, download_fd);
        le_result_t ret = le_fwupdate_Download(download_fd);
        if ( ret == LE_OK ) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Download successful; please wait for modem to reset\n", __FUNCTION__, __LINE__);
            result = LE_OK;
            le_fwupdate_DisconnectService();
            FwUpdatetheApp.fw_update_status = UPDATE_DOENLOADED;
        }
        else {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Error in download, ret is %d\n", __FUNCTION__, __LINE__, ret);
            le_fwupdate_DisconnectService();
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Error in download!!!\n", __FUNCTION__, __LINE__);
            result = LE_FAULT;
        }
        close(download_fd);
    }
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result is %d\n", __FUNCTION__, __LINE__, result);
    return result;
}


BOOL FwUpdateUpgradeStartReq(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;

    if (FwUpdateDownloadFirmwareReq(update_pkg_file) == LE_FAULT) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateDownloadFirmwareReq failed!\n", __FUNCTION__, __LINE__);
        result = FALSE;
    }
    else {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateDownloadFirmwareReq success!\n", __FUNCTION__, __LINE__);
        result = TRUE;
    }

    return result;
}


void FwUpdateUpgradeMngReq(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d\n", __FUNCTION__, __LINE__);
    le_fwupdate_DisconnectService();
    if (FwUpdateUpgradeStartReq() == FALSE) {
        /* FwUpdateUpgradeStartReq failed*/
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s ,%d, FwUpdateUpgradeStartReq failed!\n", __FUNCTION__, __LINE__);
    }
    else {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s ,%d, FwUpdateUpgradeStartReq success!\n", __FUNCTION__, __LINE__);
        FwUpdatetheApp.fw_update_status = UPDATE_DOENLOADED;
    }
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "rsp has been sent!");
}


void FwUpdateUpgradeDisconnService( void* fwupdate_ptr)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d\n", __FUNCTION__, __LINE__);
    le_fwupdate_DisconnectService();
    FwUpdatetheApp.fw_update_thread = NULL;
}


void* FwUpdateUpgradeThread(void* contextPtr)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d\n", __FUNCTION__, __LINE__);
    le_thread_AddDestructor(FwUpdateUpgradeDisconnService, NULL);
    FwUpdateUpgradeMngReq();

    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d\n", __FUNCTION__, __LINE__);
    return NULL;
}

/* polyspace< JUST-MISRA-16.7 : buff have decaared ad const No functional impact and no issue caused. : Justify > */
static void FwUpdatePrintIt(const char* label, const char* buff, size_t fwupdate_len)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    if((buff == NULL) || (fwupdate_len == (size_t)0)) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, buf or fwupdate_len is 0\n", __FUNCTION__, __LINE__);
    }
    else if(label == NULL) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "label is NULL \n");
    }
    else {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s:,fwupdate_len is %d\n", label, fwupdate_len);
        size_t i = (size_t)0;
        for(i= (size_t)0; i < fwupdate_len; ++i) {
            /*SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%02X", buff[i]);*/
            /*printf("%02X", buff[i]);*/
        }
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "\n");
    }
}



static BOOL FwUpdateBase64Decode(char *in_str, SI_32 in_len, char out_str[])
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;

    BIO *b64, *bio;
    SI_32 decode_size = 0;

    if ((in_str == NULL) || (out_str == NULL) || (in_len <= 0))  {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, in_str of out_str is NULL\n", __FUNCTION__, __LINE__);
        result = FALSE;
    }
    else {

        b64 = BIO_new(BIO_f_base64());
        /*BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);*/
        bio = BIO_new_mem_buf(in_str, in_len);
        bio = BIO_push(b64, bio);

        decode_size = BIO_read(bio, out_str, in_len);
        out_str[decode_size] = '\0';

        BIO_free_all(bio);
        result = TRUE;
    }
    return result;
}



/*===========================================
|*==                    G                  ==
|*==    Validating secondary certificate   ==
|*===========================================
|*openssl verify -CAfile cacert.pem secondary.crt*/
static BOOL FwUpdateVerfySecCert(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;
    BIO              *certbio = NULL;
    BIO               *outbio = NULL;
    X509          *error_cert = NULL;
    X509                *fwupdat_cert = NULL;
    X509_NAME    *certsubject = NULL;
    X509_STORE         *store = NULL;
    X509_STORE_CTX  *vrfy_ctx = NULL;
    SI_64 ret = -1;

    /* ---------------------------------------------------------- */
    /* These function calls initialize openssl for correct work.  */
    /* ---------------------------------------------------------- */
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    /* ---------------------------------------------------------- */
    /* Create the Input/Output BIO's.                             */
    /* ---------------------------------------------------------- */
    certbio = BIO_new(BIO_s_file());
    /*  polyspace< JUST-MISRA-8.1 :need output error information. No modification: Justify > */
    outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

    /* ---------------------------------------------------------- */
    /* Initialize the global certificate validation store object. */
    /* ---------------------------------------------------------- */
    store=X509_STORE_new();
    if (store == NULL) {
        BIO_printf(outbio, "Error creating X509_STORE_CTX object\n");
    }
    else {

        /* ---------------------------------------------------------- */
        /* Create the context structure for the validation operation. */
        /* ---------------------------------------------------------- */
        vrfy_ctx = X509_STORE_CTX_new();

        /* ---------------------------------------------------------- */
        /* Load the certificate and cacert chain from file (PEM).     */
        /* ---------------------------------------------------------- */
        /*  polyspace< JUST-MISRA-8.1 :depend on openssl format. No modification: Justify > */
        ret = BIO_read_filename(certbio, cert_filestr);
        fwupdat_cert = PEM_read_bio_X509(certbio, NULL, 0, NULL);
        if (fwupdat_cert  == NULL) {
            BIO_printf(outbio, "Error loading cert into memory\n");
            result = FALSE;
        }
        else {

            ret = X509_STORE_load_locations(store, CACERT_FILE_PATH, NULL);
            if (ret != 1) {
                BIO_printf(outbio, "Error loading CA cert or chain file\n");
            }
            else {
                /* do nothing*/
            }

            /* ---------------------------------------------------------- */
            /* Initialize the ctx structure for a verification operation: */
            /* Set the trusted cert store, the unvalidated cert, and any  */
            /* potential certs that could be needed (here we set it NULL) */
            /* ---------------------------------------------------------- */
            X509_STORE_CTX_init(vrfy_ctx, store, fwupdat_cert, NULL);

            /* ---------------------------------------------------------- */
            /* Check the complete cert chain can be build and validated.  */
            /* Returns 1 on success, 0 on verification failures, and -1   */
            /* for trouble with the ctx object (i.e. missing certificate) */
            /* ---------------------------------------------------------- */
            ret = X509_verify_cert(vrfy_ctx);
            BIO_printf(outbio, "Verification return code: %ld\n", ret);
            if (ret == 1) {
                result = TRUE;
            }
            else {
                result = FALSE;
            }


            if((ret == 0) || (ret == 1)) {
                BIO_printf(outbio, "Verification result text: %s\n",
                           X509_verify_cert_error_string(vrfy_ctx->error));
            }
            else {
                /*Do Nothing */
            }

            /* ---------------------------------------------------------- */
            /* The error handling below shows how to get failure details  */
            /* from the offending certificate.                            */
            /* ---------------------------------------------------------- */
            if(ret == 0) {
                /*  get the offending certificate causing the failure */
                error_cert  = X509_STORE_CTX_get_current_cert(vrfy_ctx);
                certsubject = X509_NAME_new();
                certsubject = X509_get_subject_name(error_cert);
                BIO_printf(outbio, "Verification failed cert:\n");
                /*  polyspace< JUST-MISRA-8.1 :depend on openssl format. No modification: Justify > */
                X509_NAME_print_ex(outbio, certsubject, 0, (UI_64)XN_FLAG_MULTILINE);
                BIO_printf(outbio, "\n");
            }
        }

        /* ---------------------------------------------------------- */
        /* Free up all structures                                     */
        /* ---------------------------------------------------------- */
        X509_STORE_CTX_free(vrfy_ctx);
        X509_STORE_free(store);
        X509_free(fwupdat_cert);
        BIO_free_all(certbio);
        BIO_free_all(outbio);
    }
    /*BIO_free_all(certbio);*/
    /*BIO_free_all(outbio);*/
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result =%d", __FUNCTION__, __LINE__, result);
    return result;
}


/*======================================================================================
|*==                  H                                                               ==
|*==            Extracting secondary keys                                             ==
|*==   openssl x509 -in secondary.crt -pubkey -noout > secondary_public.pem           ==
|*======================================================================================*/

static BOOL FwExtracSecondaryKeys(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;
    BIO *certbio = NULL;
    BIO *outbio = NULL;
    X509 *fwpdate_cert = NULL;

    /* ---------------------------------------------------------- */
    /* These function calls initialize openssl for correct work.  */
    /* ---------------------------------------------------------- */
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    /* ---------------------------------------------------------- */
    /* Create the Input/Output BIO's.                             */
    /* ---------------------------------------------------------- */
    certbio = BIO_new(BIO_s_file());
    /*  polyspace< JUST-MISRA-8.1 :need output error information. No modification: Justify > */
    outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

    /* ---------------------------------------------------------- */
    /* Load the certificate from file (PEM).                      */
    /* ---------------------------------------------------------- */
    /*  polyspace< JUST-MISRA-12.7 :need openssl format. No modification: Justify > */
    if (BIO_read_filename(certbio, cert_filestr) == 0) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
        result = FALSE;
    }
    else {
        fwpdate_cert = PEM_read_bio_X509(certbio, NULL, 0, NULL);
        if (fwpdate_cert == NULL) {
            BIO_printf(outbio, "Error loading cert into memory\n");
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
            result = FALSE;
        }
        else {

            /* ---------------------------------------------------------- */
            /* Extract the certificate's public key data.                 */
            /* ---------------------------------------------------------- */
            fwpdate_pkey = X509_get_pubkey(fwpdate_cert);
            if (fwpdate_pkey == NULL) {
                BIO_printf(outbio, "Error getting public key from certificate");
                result = FALSE;

            }
            /* ---------------------------------------------------------- */
            /* Print the public key information and the key in PEM format */
            /* ---------------------------------------------------------- */
            /* display the key type and size here */
            if (fwpdate_pkey != NULL) {
                switch (fwpdate_pkey->type) {
                case EVP_PKEY_RSA:
                    BIO_printf(outbio, "%d bit RSA Key\n\n", EVP_PKEY_bits(fwpdate_pkey));
                    break;
                case EVP_PKEY_DSA:
                    BIO_printf(outbio, "%d bit DSA Key\n\n", EVP_PKEY_bits(fwpdate_pkey));
                    break;
                default:
                    BIO_printf(outbio, "%d bit non-RSA/DSA Key\n\n", EVP_PKEY_bits(fwpdate_pkey));
                    break;
                }
            }
            else if(PEM_write_bio_PUBKEY(outbio, fwpdate_pkey) == 0){
                BIO_printf(outbio, "Error writing public key data in PEM format");
            }
            else {
                /* Do nothing */
            }
        }
    }

    /* EVP_PKEY_free(fwpdate_pkey);*/
    X509_free(fwpdate_cert);
    BIO_free_all(certbio);
    BIO_free_all(outbio);
    result = TRUE;
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result =%d", __FUNCTION__, __LINE__, result);
    return result;
}


/*============================================================================================
|*==              J                                                                         ==
|*==              Validating signature                                                      ==
|*==    openssl dgst -sha256 -verify secondary_public.pem -signature signature image.cwe*   ==
|*============================================================================================*/

/* polyspace< JUST-MISRA-16.7 : msg have decaared ad const No functional impact and no issue caused. : Justify > */
static SI_32 FwUpdateRSAVerifySignature( UI_8* msg_hash, size_t msg_hash_len, const char* msg, size_t msg_len, SI_32* authentic)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    SI_32 result = -1;
    *authentic = 0;
    EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

    if ((msg_hash == NULL) || (msg == NULL) || (m_RSAVerifyCtx == NULL) || (fwpdate_pkey == NULL)) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, msg_hash or msg is NULL\n", __FUNCTION__, __LINE__);
        result = -1;
    }
    else {

        if (EVP_DigestVerifyInit(m_RSAVerifyCtx, NULL, EVP_sha256(), NULL, fwpdate_pkey) <= 0) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
            result = -1;
        }
        else if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, msg, msg_len) <= 0) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
            result = -1;
        }
        else {

            *authentic = EVP_DigestVerifyFinal(m_RSAVerifyCtx, msg_hash, msg_hash_len);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, authentic = %d\n", __FUNCTION__, __LINE__, *authentic);

            EVP_MD_CTX_cleanup(m_RSAVerifyCtx);

            if (*authentic >= 0){
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
                result =  1;
            }else {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
                result = -1;
            }
        }
    }
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result = %d\n", __FUNCTION__, __LINE__, result);
    return result;
}

static BOOL FwUpdateCmpVersion(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;
    char app_version[100] = {0};
    /* polyspace< JUST-MISRA-20.9 : #We need fopen to open file# : Justify > */
    /* File Handler, Purpose: Get software version, Resource Release Place: FwUpdateCmpVersion */
    FILE* f = fopen("/legato/systems/current/apps/WifiBoxManager/info.properties", "r+");
    if (f == NULL) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d, open info.properties fail!", __FUNCTION__, __LINE__);
        result = FALSE;
    }
    else {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
        char version_data[50] = {0};
        /* polyspace< JUST-MISRA-20.9 : #We need fgets to get information in file# : Justify > */
        while(fgets(version_data, 50, f) != 0) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s line=%d, data = %s", __FUNCTION__, __LINE__,version_data);
            if (strncmp(version_data, "app.version", strlen("app.version")) == 0) {
                /* polyspace< JUST-MISRA-20.9 : #We need sscanf to get information from string# : Justify > */
                sscanf(version_data, "app.version=%s", app_version);
                break;
            }
            else {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
            }
        }
        /* polyspace< JUST-MISRA-20.9 : #We need fclose to close file# : Justify > */
        /* File Handler, Resource Allocation Place: WebInterfaceInit */
        fclose(f);
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, fwupat_version=%s, app_version=%s", __FUNCTION__, __LINE__, fwupate_version, app_version);
        /**** match version string *****/
        regmatch_t pmatch[100];
        const UI_32 nmatch = (UI_32)100;
        regex_t reg;
        SI_32 res = 0;
        regcomp(&reg, Version_Pattern, REG_EXTENDED);
        res = regexec(&reg, (const char*)fwupate_version, (size_t)nmatch, pmatch, 0);
        if ((SI_32)REG_NOMATCH == res) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, no match!!", __FUNCTION__, __LINE__, result);
            result = FALSE;
        }
        else {
            UI_32 ver_first_cur = (UI_32)0, ver_second_cur = (UI_32)0, ver_third_cur = (UI_32)0;
            UI_32 ver_first = (UI_32)0, ver_second = (UI_32)0, ver_third = (UI_32)0;
            char ver_time_cur[50] = {0};
            char ver_time[50] = {0};
            sscanf(app_version,"W%d.%d_RC%d.%s", &ver_first_cur, &ver_second_cur, &ver_third_cur, ver_time_cur);
            sscanf(fwupate_version,"W%d.%d_RC%d.%s", &ver_first, &ver_second, &ver_third, ver_time);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "cur version: first=%d, second=%d, third=%d\n", ver_first_cur, ver_second_cur, ver_third_cur);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "new version: first=%d, second=%d, third=%d", ver_first, ver_second, ver_third);
            if (ver_first > ver_first_cur) {
                result = TRUE;
            }
            else if ((ver_first == ver_first_cur) && (ver_second > ver_second_cur)) {
                result = TRUE;
            }
            else if ((ver_first == ver_first_cur) && (ver_second == ver_second_cur) && (ver_third > ver_third_cur)) {
                result = TRUE;
            }
            else if ((ver_first == ver_first_cur) && (ver_second == ver_second_cur) && (ver_third == ver_third_cur)
                     && (strncmp(ver_time, ver_time_cur, strlen("20170518151312")) > 0)) {
                result = TRUE;
            }
            else {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d,fwupdat vesion is lower current version in board!!! ", __FUNCTION__, __LINE__);
                result = FALSE;
            }
        }
        regfree(&reg);
    }
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result =%d", __FUNCTION__, __LINE__, result);
    return result;
}


static BOOL FwUpdateExtractSignatureImage(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;
    SI_32 sig_len;


    SI_64 location = 0;
    SI_32 fp_update = open_new(update_pkg_file,O_RDONLY);
    /*SI_32 fp_update = open(update_pkg_file,O_RDONLY);*/
    if (fp_update == -1) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, fopen update_pkg_file failed!!!\n", __FUNCTION__, __LINE__);
        result = FALSE;
    }
    else {
        /******* extract certificate and decode certificate *******/
        ssize_t ret = read(fp_update, secondary_encode, (size_t)SECONDARY_SIZE);
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read secondary_encode = %zd\n", __FUNCTION__, __LINE__, ret);
        if (FwUpdateBase64Decode(secondary_encode, (SI_32)strlen((const char*)secondary_encode), secondary_decode) == FALSE) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateBase64Decode failed!!!\n", __FUNCTION__, __LINE__);
            result = FALSE;
        }
        else if (ret < SECONDARY_SIZE) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read secondary_encode < 4096, ret= %d!!!\n", __FUNCTION__, __LINE__, ret);
            result = FALSE;
        }
        else {
            /* polyspace< JUST-MISRA-17.1,17.4 : we need index in pointer. : Justify > */
            SI_32 crtFd = open_new(cert_filestr, O_CREAT | O_WRONLY, S_IRWXO);
            /*SI_32 fp_update = open(update_pkg_file,O_RDONLY);*/
            ret = write(crtFd, secondary_decode,strlen(secondary_decode));
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, write cert file, fd=%d, ret=%zd !!\n", __FUNCTION__, __LINE__, crtFd, ret);
            close(crtFd);
            /****** extract signature and decode signature ********/
            location = 8*512L;
            if (lseek(fp_update, location,0) == -1) {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, fseek failed!!!\n", __FUNCTION__, __LINE__);
                result = FALSE;
            }
            else {
                ret = read(fp_update, sig_buf, (size_t)SIGNATURE);
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read signatue.64 = %zd\n", __FUNCTION__, __LINE__, ret);
                /*printf("sig_buf = %s\n", sig_buf);*/
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read signatue encode size = %d\n", __FUNCTION__, __LINE__, strlen(sig_buf));

                if (FwUpdateBase64Decode(sig_buf, (SI_32)strlen((const char*)sig_buf), sig_buf_dst) == FALSE) {
                    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateBase64Decode failed!!!\n", __FUNCTION__, __LINE__);
                    result = FALSE;
                }
                else if (ret < SIGNATURE) {
                    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read sig_buf < 2048, ret= %d!!!\n", __FUNCTION__, __LINE__, ret);
                    result = FALSE;
                }
                else {
                    /*printf("sig_buf_dst = %s\n", sig_buf_dst);*/
                    sig_len = (SI_32)strlen(sig_buf_dst);
                    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read signatue decode size = %d\n", __FUNCTION__, __LINE__, sig_len);
                    /*********** extract version ******************/
                    lseek(fp_update, 0L, 0);
                    location = 12*512L;
                    lseek(fp_update, location,0);
                    memset(fwupate_version, 0, sizeof(fwupate_version));
                    ret = read(fp_update, fwupate_version, (size_t)VERSION_SIZE);
                    if (ret < VERSION_SIZE) {
                        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read version size = %d\n", __FUNCTION__, __LINE__, ret);
                    }
                    else {
                        /* remove char '\n' in version, becasue extract file may contain '\n'*/
                        if (fwupate_version[strlen(fwupate_version) - 1] == '\n') {
                            fwupate_version[strlen(fwupate_version) - 1] = '\0';
                        }
                        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, read version size = %d, version=%s\n", __FUNCTION__, __LINE__, strlen(fwupate_version), fwupate_version);
                        /********** compute image size and extract image ***********/
                        lseek(fp_update, 0L, 0);
                        location = 13*512L;
                        fstat(fp_update, &update_pk_sb); /* read update file size */
                        /* polyspace< JUST-MISRA-10.3 : location is < SI_32. : Justify > */
                        fwupdat_data_len = (size_t)(update_pk_sb.st_size - location);
                        /*lseek(fp_update, location, 0);*/
                        fwupdat_data = mmap(NULL, update_pk_sb.st_size, PROT_READ, MAP_PRIVATE, fp_update, 0);
                        lseek(fp_update, 0L, 0);
                        /* polyspace< JUST-MISRA-11.3 : Return value of `mmap' in case of an error. : Justify > */
                        if (fwupdat_data == MAP_FAILED) {
                            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, mmap failed!", __FUNCTION__, __LINE__);
                            result = FALSE;
                        }
                        else {
                            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, mmap data len = %d\n", __FUNCTION__, __LINE__, update_pk_sb.st_size);
                            result = TRUE;
                        }
                    }
                }
            }
        }
        close(fp_update);
    }
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result =%d", __FUNCTION__, __LINE__, result);
    return result;

}

/**************************************************************************
 |1.Extracting secondary certificate
 |     1> open updat file
 |     2>read secondary.64 from file
 |     3>decode base64 secondary.64 to secondary.crt
 |2.Validating secondary certificate:
 |3.Extracting secondary keys:
 |4.Extracting signature
 |     1>read  signature.64 from file
 |     1>decode base64 signature.64 to signature
 |5.Validating signature
 ******************************************************************************/
LE_SHARED FwUpdate_Verify_result FwUpdateVerifyReq(char fw_version[])
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    FwUpdate_Verify_result result = VERIFY_FAILED;
    BOOL result_extract = FALSE;
    FwUpdatetheApp.fw_update_status = UPDATE_VERIFYING;
    result_extract = FwUpdateExtractSignatureImage();
    if ((result_extract == FALSE) || (fw_version == NULL)) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateVerfySecCert failed!\n", __FUNCTION__, __LINE__);
        result = VERIFY_SIGNATURE_FAILED;
        FwUpdatetheApp.fw_update_status = UPDATE_NONE;
    }
    else if (FwUpdateVerfySecCert() == FALSE) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateVerfySecCert failed!\n", __FUNCTION__, __LINE__);
        result = VERIFY_SIGNATURE_FAILED;
        FwUpdatetheApp.fw_update_status = UPDATE_NONE;
    }
    else {
        FwExtracSecondaryKeys();
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);

        SI_32 authentic;
        /* polyspace< JUST-MISRA-17.1,17.4 : we need index in pointer. : Justify > */
        char *data_image = fwupdat_data + (13*512);

        SI_32 ret = FwUpdateRSAVerifySignature( (UI_8*)sig_buf_dst, (size_t)512, data_image, fwupdat_data_len, &authentic);
        FwUpdatePrintIt("Signature", sig_buf, strlen((char*)sig_buf));
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, fwupdat_data_len = %d\n", __FUNCTION__, __LINE__, fwupdat_data_len);

        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, ret = %d\n", __FUNCTION__, __LINE__, ret);
        if ((ret == 1) && (authentic == 1)) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Signature Verified Ok!\n", __FUNCTION__, __LINE__);
            if (FwUpdateCmpVersion() == FALSE) {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateCmpVersion failed!!!\n", __FUNCTION__, __LINE__);
                result = VERIFY_VERSION_FAILED;
                FwUpdatetheApp.fw_update_status = UPDATE_NONE;
            }
            else {
                strcpy(fw_version, fwupate_version);
                fw_version[strlen(fw_version)] = '\0';
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdateVerifyReq success!!!, fw_version=%s\n", __FUNCTION__, __LINE__, fw_version);
                result = VERIFY_SUCCESS;
                FwUpdatetheApp.fw_update_status = UPDATE_VERIFYED;
            }
        }
        else {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, Signature Verified failed!\n", __FUNCTION__, __LINE__);
            result = VERIFY_SIGNATURE_FAILED;
            FwUpdatetheApp.fw_update_status = UPDATE_NONE;
        }


        EVP_PKEY_free(fwpdate_pkey);
    }

    if (result != VERIFY_SUCCESS) {
        /* REU_SYS033_SWUPDT_146:WifiBox shall discard all unverified Firmware files uploaded by the dealer.*/
        /* REU_SYS033_SWUPDT_152:WifiBox shall erase all discarted Firmware files. */
        FwUpdateDiscardReq();
    }
    else {
        /* Do nothing*/
    }
    munmap(fwupdat_data, update_pk_sb.st_size);
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result = %d, fw_update_status= %d\n", __FUNCTION__, __LINE__, result, FwUpdatetheApp.fw_update_status);
    return result;
}

LE_SHARED BOOL FwUpdateUpgradeReq(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    BOOL result = FALSE;

    if (FwUpdatetheApp.fw_update_status != UPDATE_VERIFYED) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, fw_update_status = %d\n", __FUNCTION__, __LINE__, FwUpdatetheApp.fw_update_status);
        result = FALSE;
    }
    else {

        if (FwUpdatetheApp.fw_update_thread == NULL) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
        }
        else {
            /* cancle old update thread */
            le_thread_Cancel(FwUpdatetheApp.fw_update_thread);
            le_thread_Join(FwUpdatetheApp.fw_update_thread, NULL);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
        }
        /* creat updat thread*/
        FwUpdatetheApp.fw_update_status = UPDATE_DOENLOADING;
        FwUpdatetheApp.fw_update_thread = le_thread_Create("FW_UPDATE_THREAD", FwUpdateUpgradeThread, NULL);
        le_thread_SetJoinable(FwUpdatetheApp.fw_update_thread);
        le_thread_Start(FwUpdatetheApp.fw_update_thread);
        SI_32 flag = 120;
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
        while(flag != 0) {
            sleep((UI_32)1);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
            if (FwUpdatetheApp.fw_update_status == UPDATE_DOENLOADED) {
                SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
                break;
            }
            else {
                flag = flag -1;
            }
        }
        if (FwUpdatetheApp.fw_update_status != UPDATE_DOENLOADED) {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
            result = FALSE;
        }
        else {
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);
            result = TRUE;
        }
        FwUpdatetheApp.fw_update_status = UPDATE_NONE;
        le_thread_Cancel(FwUpdatetheApp.fw_update_thread);
        le_thread_Join(FwUpdatetheApp.fw_update_thread, NULL);

        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d", __FUNCTION__, __LINE__);

    }
    /*result = FwUpdateUpgradeStartReq();*/
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result = %d\n", __FUNCTION__, __LINE__, result);
    return result;
}

LE_SHARED FwUpdate_staus FwUpdateGetStatus(char fw_verison[])
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    if (FwUpdatetheApp.fw_update_status == UPDATE_NONE) {
        FwUpdateDiscardReq();
    }
    else if (FwUpdatetheApp.fw_update_status == UPDATE_VERIFYED) {
        strcpy(fw_verison, fwupate_version);
    }
    else {
        /* Do nothing*/
    }
    FwUpdate_staus result = FwUpdatetheApp.fw_update_status;
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d,FwUpdate_staus = %d, fw_verison=%s \n", __FUNCTION__, __LINE__, result, fw_verison);
    return result;
}

LE_SHARED BOOL FwUpdateDiscardReq(void)
{
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdate_staus = %d\n", __FUNCTION__, __LINE__, FwUpdatetheApp.fw_update_status);
    if (access(update_pkg_file,F_OK) == 0) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
        unlink(update_pkg_file);
    }
    else {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, update_pkg have not exit!\n", __FUNCTION__, __LINE__);
    }
    if (access(cert_filestr,F_OK) == 0) {
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
        unlink(cert_filestr);
    }
    else {
         SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, secondary.crt have not exit!\n", __FUNCTION__, __LINE__);
    }
    FwUpdatetheApp.fw_update_status = UPDATE_NONE;
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, FwUpdate_staus = %d\n", __FUNCTION__, __LINE__, FwUpdatetheApp.fw_update_status);
    return TRUE;
}

/*--------------------------------------------------------------------------------------------------*/
/*
 * Process the query command, and print out the firmware, bootloader and linux versions.
 *
 * @return
 *      - LE_OK if it was possible to show all versions
 *      - LE_FAULT if that was not the case
 */
/*--------------------------------------------------------------------------------------------------*/
LE_SHARED le_result_t FwUpdageQueryVersion(char *firmware_ver, char *bootloader_ver, char *linux_ver)
{
    le_result_t result = LE_OK;

    FwUpdateUpgradeTryConnect(le_fwupdate_ConnectService, "fwupdateService");

    /* Connected to service so continue*/
    char version_str[MAX_VERS_BYTES];
    memset(version_str, 0, sizeof(version_str));
    struct utsname linuxInfo;

    if ((firmware_ver == NULL) || (bootloader_ver == NULL) || (linux_ver == NULL)) {
        result = LE_FAULT;
        SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d\n", __FUNCTION__, __LINE__);
    }
    else {
        if ( le_fwupdate_GetFirmwareVersion(version_str, sizeof(version_str)) == LE_OK ) {
            /* polyspace< JUST-MISRA-20.9 : #We need sprintf to get string# : Justify > */
            sprintf(firmware_ver, "Firmware Version: %s\n", version_str);
            memset(version_str, 0, sizeof(version_str));
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, firmware_ver: %s\n", __FUNCTION__, __LINE__, firmware_ver);
        }
        else {
            result = LE_FAULT;
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, get version failed!!!\n", __FUNCTION__, __LINE__);
        }

        if ( le_fwupdate_GetBootloaderVersion(version_str, sizeof(version_str)) == LE_OK ) {
            /* polyspace< JUST-MISRA-20.9 : #We need sprintf to get string# : Justify > */
            sprintf(bootloader_ver, "Bootloader Version: %s\n", version_str);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, bootloader_ver: %s\n", __FUNCTION__, __LINE__, bootloader_ver);
        }
        else {
            result = LE_FAULT;
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, get version failed!!!\n", __FUNCTION__, __LINE__);
        }

        if ( uname(&linuxInfo) == 0 ) {
            /* polyspace< JUST-MISRA-20.9 : #We need sprintf to get string# : Justify > */
            sprintf(linux_ver, "Linux Version: %s %s\n", linuxInfo.release, linuxInfo.version);
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, linux_ver: %s\n", __FUNCTION__, __LINE__, linux_ver);
        }
        else {
            result = LE_FAULT;
            SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, get version failed!!!\n", __FUNCTION__, __LINE__);
        }
    }

    le_fwupdate_DisconnectService();
    SendDebug(DEBUG_INFO, BT_DEBUG_ID, "%s, %d, result_str=%d\n", __FUNCTION__, __LINE__, result);
    return result;
}


/* polyspace< JUST-MISRA-8.1 : #This is Legato function# : Justify > */
/* polyspace< JUST-MISRA-16.5 : #This is Legato function# : Justify > */
/* polyspace< JUST-MISRA-20.2 : #This is Legato function# : Justify > */
COMPONENT_INIT
{
    FwUpdatetheApp.fw_update_status = UPDATE_NONE;
}
