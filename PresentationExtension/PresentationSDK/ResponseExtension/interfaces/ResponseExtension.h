

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Mar 18 14:27:47 2009
 */
/* Compiler settings for ResponseExtension.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ResponseExtension_h__
#define __ResponseExtension_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IActiveResponseDevice_FWD_DEFINED__
#define __IActiveResponseDevice_FWD_DEFINED__
typedef interface IActiveResponseDevice IActiveResponseDevice;
#endif 	/* __IActiveResponseDevice_FWD_DEFINED__ */


#ifndef __IResponseDevice_FWD_DEFINED__
#define __IResponseDevice_FWD_DEFINED__
typedef interface IResponseDevice IResponseDevice;
#endif 	/* __IResponseDevice_FWD_DEFINED__ */


#ifndef __IResponseDeviceEnumerator_FWD_DEFINED__
#define __IResponseDeviceEnumerator_FWD_DEFINED__
typedef interface IResponseDeviceEnumerator IResponseDeviceEnumerator;
#endif 	/* __IResponseDeviceEnumerator_FWD_DEFINED__ */


#ifndef __IResponseDevice_FWD_DEFINED__
#define __IResponseDevice_FWD_DEFINED__
typedef interface IResponseDevice IResponseDevice;
#endif 	/* __IResponseDevice_FWD_DEFINED__ */


#ifndef __IResponseDeviceEnumerator_FWD_DEFINED__
#define __IResponseDeviceEnumerator_FWD_DEFINED__
typedef interface IResponseDeviceEnumerator IResponseDeviceEnumerator;
#endif 	/* __IResponseDeviceEnumerator_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ResponseExtension_0000 */
/* [local] */ 

typedef struct tagResponseInfo
    {
    unsigned long buttonIndex;
    unsigned char useTime;
    __int64 time;
    unsigned long uncertainty;
    } 	ResponseInfo;



extern RPC_IF_HANDLE __MIDL_itf_ResponseExtension_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ResponseExtension_0000_v0_0_s_ifspec;

#ifndef __IActiveResponseDevice_INTERFACE_DEFINED__
#define __IActiveResponseDevice_INTERFACE_DEFINED__

/* interface IActiveResponseDevice */
/* [object][oleautomation][dual][version][uuid] */ 


EXTERN_C const IID IID_IActiveResponseDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00CD547B-61DA-43F8-930B-45F55BA7ED2D")
    IActiveResponseDevice : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT __stdcall getName( 
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getXYIndices( 
            /* [out] */ long *xIndex,
            /* [out] */ long *yIndex) = 0;
        
        virtual /* [id] */ HRESULT __stdcall processMessage( 
            /* [in] */ BSTR message,
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getResponseCount( 
            /* [retval][out] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getResponseData( 
            /* [out] */ ResponseInfo *data,
            /* [out][in] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall clearResponseData( void) = 0;
        
        virtual /* [id] */ HRESULT __stdcall pollAxisPositions( 
            /* [out] */ double *positions,
            /* [out][in] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall pollAxisChanges( 
            /* [out] */ double *changes,
            /* [out][in] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getLastError( 
            /* [retval][out] */ BSTR *error) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveResponseDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveResponseDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveResponseDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveResponseDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActiveResponseDevice * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActiveResponseDevice * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActiveResponseDevice * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActiveResponseDevice * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( __stdcall *getName )( 
            IActiveResponseDevice * This,
            /* [retval][out] */ BSTR *name);
        
        /* [id] */ HRESULT ( __stdcall *getXYIndices )( 
            IActiveResponseDevice * This,
            /* [out] */ long *xIndex,
            /* [out] */ long *yIndex);
        
        /* [id] */ HRESULT ( __stdcall *processMessage )( 
            IActiveResponseDevice * This,
            /* [in] */ BSTR message,
            /* [retval][out] */ BSTR *result);
        
        /* [id] */ HRESULT ( __stdcall *getResponseCount )( 
            IActiveResponseDevice * This,
            /* [retval][out] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *getResponseData )( 
            IActiveResponseDevice * This,
            /* [out] */ ResponseInfo *data,
            /* [out][in] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *clearResponseData )( 
            IActiveResponseDevice * This);
        
        /* [id] */ HRESULT ( __stdcall *pollAxisPositions )( 
            IActiveResponseDevice * This,
            /* [out] */ double *positions,
            /* [out][in] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *pollAxisChanges )( 
            IActiveResponseDevice * This,
            /* [out] */ double *changes,
            /* [out][in] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *getLastError )( 
            IActiveResponseDevice * This,
            /* [retval][out] */ BSTR *error);
        
        END_INTERFACE
    } IActiveResponseDeviceVtbl;

    interface IActiveResponseDevice
    {
        CONST_VTBL struct IActiveResponseDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveResponseDevice_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveResponseDevice_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveResponseDevice_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveResponseDevice_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IActiveResponseDevice_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IActiveResponseDevice_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IActiveResponseDevice_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IActiveResponseDevice_getName(This,name)	\
    (This)->lpVtbl -> getName(This,name)

#define IActiveResponseDevice_getXYIndices(This,xIndex,yIndex)	\
    (This)->lpVtbl -> getXYIndices(This,xIndex,yIndex)

#define IActiveResponseDevice_processMessage(This,message,result)	\
    (This)->lpVtbl -> processMessage(This,message,result)

#define IActiveResponseDevice_getResponseCount(This,count)	\
    (This)->lpVtbl -> getResponseCount(This,count)

#define IActiveResponseDevice_getResponseData(This,data,count)	\
    (This)->lpVtbl -> getResponseData(This,data,count)

#define IActiveResponseDevice_clearResponseData(This)	\
    (This)->lpVtbl -> clearResponseData(This)

#define IActiveResponseDevice_pollAxisPositions(This,positions,count)	\
    (This)->lpVtbl -> pollAxisPositions(This,positions,count)

#define IActiveResponseDevice_pollAxisChanges(This,changes,count)	\
    (This)->lpVtbl -> pollAxisChanges(This,changes,count)

#define IActiveResponseDevice_getLastError(This,error)	\
    (This)->lpVtbl -> getLastError(This,error)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT __stdcall IActiveResponseDevice_getName_Proxy( 
    IActiveResponseDevice * This,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IActiveResponseDevice_getName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_getXYIndices_Proxy( 
    IActiveResponseDevice * This,
    /* [out] */ long *xIndex,
    /* [out] */ long *yIndex);


void __RPC_STUB IActiveResponseDevice_getXYIndices_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_processMessage_Proxy( 
    IActiveResponseDevice * This,
    /* [in] */ BSTR message,
    /* [retval][out] */ BSTR *result);


void __RPC_STUB IActiveResponseDevice_processMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_getResponseCount_Proxy( 
    IActiveResponseDevice * This,
    /* [retval][out] */ unsigned long *count);


void __RPC_STUB IActiveResponseDevice_getResponseCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_getResponseData_Proxy( 
    IActiveResponseDevice * This,
    /* [out] */ ResponseInfo *data,
    /* [out][in] */ unsigned long *count);


void __RPC_STUB IActiveResponseDevice_getResponseData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_clearResponseData_Proxy( 
    IActiveResponseDevice * This);


void __RPC_STUB IActiveResponseDevice_clearResponseData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_pollAxisPositions_Proxy( 
    IActiveResponseDevice * This,
    /* [out] */ double *positions,
    /* [out][in] */ unsigned long *count);


void __RPC_STUB IActiveResponseDevice_pollAxisPositions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_pollAxisChanges_Proxy( 
    IActiveResponseDevice * This,
    /* [out] */ double *changes,
    /* [out][in] */ unsigned long *count);


void __RPC_STUB IActiveResponseDevice_pollAxisChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IActiveResponseDevice_getLastError_Proxy( 
    IActiveResponseDevice * This,
    /* [retval][out] */ BSTR *error);


void __RPC_STUB IActiveResponseDevice_getLastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveResponseDevice_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ResponseExtension_0116 */
/* [local] */ 

typedef /* [uuid] */  DECLSPEC_UUID("D0297FFD-3636-4250-8700-1BFB24930E07") 
enum tagAxisType
    {	AT_RELATIVE	= 0,
    AT_ABSOLUTE	= 1,
    AT_FORCE_DWORD	= 0x7fffffff
    } 	AxisType;



extern RPC_IF_HANDLE __MIDL_itf_ResponseExtension_0116_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ResponseExtension_0116_v0_0_s_ifspec;

#ifndef __IResponseDevice_INTERFACE_DEFINED__
#define __IResponseDevice_INTERFACE_DEFINED__

/* interface IResponseDevice */
/* [object][oleautomation][dual][version][uuid] */ 


EXTERN_C const IID IID_IResponseDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0AA36774-958A-48F1-BC84-E80E3E91A7AA")
    IResponseDevice : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT __stdcall getName( 
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getPropertiesViewable( 
            /* [retval][out] */ unsigned char *viewable) = 0;
        
        virtual /* [id] */ HRESULT __stdcall showProperties( 
            HWND parentWindow) = 0;
        
        virtual /* [id] */ HRESULT __stdcall setDefaults( void) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getProperties( 
            /* [retval][out] */ BSTR *parameters) = 0;
        
        virtual /* [id] */ HRESULT __stdcall setProperties( 
            /* [in] */ BSTR parameters) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getButtonCount( 
            /* [retval][out] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getButtonNames( 
            /* [out] */ BSTR *names,
            /* [out][in] */ unsigned long *size) = 0;
        
        virtual /* [id] */ HRESULT __stdcall acquire( 
            /* [in] */ unsigned char useButtons,
            /* [in] */ unsigned char useAxes,
            /* [retval][out] */ IActiveResponseDevice **device) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getAxisCount( 
            /* [out] */ unsigned long *count,
            /* [out] */ AxisType *type) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getAxisNames( 
            /* [out] */ BSTR *names,
            /* [out][in] */ unsigned long *size) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getLastError( 
            /* [retval][out] */ BSTR *error) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResponseDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResponseDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResponseDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResponseDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IResponseDevice * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IResponseDevice * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IResponseDevice * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IResponseDevice * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( __stdcall *getName )( 
            IResponseDevice * This,
            /* [retval][out] */ BSTR *name);
        
        /* [id] */ HRESULT ( __stdcall *getPropertiesViewable )( 
            IResponseDevice * This,
            /* [retval][out] */ unsigned char *viewable);
        
        /* [id] */ HRESULT ( __stdcall *showProperties )( 
            IResponseDevice * This,
            HWND parentWindow);
        
        /* [id] */ HRESULT ( __stdcall *setDefaults )( 
            IResponseDevice * This);
        
        /* [id] */ HRESULT ( __stdcall *getProperties )( 
            IResponseDevice * This,
            /* [retval][out] */ BSTR *parameters);
        
        /* [id] */ HRESULT ( __stdcall *setProperties )( 
            IResponseDevice * This,
            /* [in] */ BSTR parameters);
        
        /* [id] */ HRESULT ( __stdcall *getButtonCount )( 
            IResponseDevice * This,
            /* [retval][out] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *getButtonNames )( 
            IResponseDevice * This,
            /* [out] */ BSTR *names,
            /* [out][in] */ unsigned long *size);
        
        /* [id] */ HRESULT ( __stdcall *acquire )( 
            IResponseDevice * This,
            /* [in] */ unsigned char useButtons,
            /* [in] */ unsigned char useAxes,
            /* [retval][out] */ IActiveResponseDevice **device);
        
        /* [id] */ HRESULT ( __stdcall *getAxisCount )( 
            IResponseDevice * This,
            /* [out] */ unsigned long *count,
            /* [out] */ AxisType *type);
        
        /* [id] */ HRESULT ( __stdcall *getAxisNames )( 
            IResponseDevice * This,
            /* [out] */ BSTR *names,
            /* [out][in] */ unsigned long *size);
        
        /* [id] */ HRESULT ( __stdcall *getLastError )( 
            IResponseDevice * This,
            /* [retval][out] */ BSTR *error);
        
        END_INTERFACE
    } IResponseDeviceVtbl;

    interface IResponseDevice
    {
        CONST_VTBL struct IResponseDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResponseDevice_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResponseDevice_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResponseDevice_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResponseDevice_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IResponseDevice_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IResponseDevice_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IResponseDevice_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IResponseDevice_getName(This,name)	\
    (This)->lpVtbl -> getName(This,name)

#define IResponseDevice_getPropertiesViewable(This,viewable)	\
    (This)->lpVtbl -> getPropertiesViewable(This,viewable)

#define IResponseDevice_showProperties(This,parentWindow)	\
    (This)->lpVtbl -> showProperties(This,parentWindow)

#define IResponseDevice_setDefaults(This)	\
    (This)->lpVtbl -> setDefaults(This)

#define IResponseDevice_getProperties(This,parameters)	\
    (This)->lpVtbl -> getProperties(This,parameters)

#define IResponseDevice_setProperties(This,parameters)	\
    (This)->lpVtbl -> setProperties(This,parameters)

#define IResponseDevice_getButtonCount(This,count)	\
    (This)->lpVtbl -> getButtonCount(This,count)

#define IResponseDevice_getButtonNames(This,names,size)	\
    (This)->lpVtbl -> getButtonNames(This,names,size)

#define IResponseDevice_acquire(This,useButtons,useAxes,device)	\
    (This)->lpVtbl -> acquire(This,useButtons,useAxes,device)

#define IResponseDevice_getAxisCount(This,count,type)	\
    (This)->lpVtbl -> getAxisCount(This,count,type)

#define IResponseDevice_getAxisNames(This,names,size)	\
    (This)->lpVtbl -> getAxisNames(This,names,size)

#define IResponseDevice_getLastError(This,error)	\
    (This)->lpVtbl -> getLastError(This,error)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT __stdcall IResponseDevice_getName_Proxy( 
    IResponseDevice * This,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IResponseDevice_getName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getPropertiesViewable_Proxy( 
    IResponseDevice * This,
    /* [retval][out] */ unsigned char *viewable);


void __RPC_STUB IResponseDevice_getPropertiesViewable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_showProperties_Proxy( 
    IResponseDevice * This,
    HWND parentWindow);


void __RPC_STUB IResponseDevice_showProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_setDefaults_Proxy( 
    IResponseDevice * This);


void __RPC_STUB IResponseDevice_setDefaults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getProperties_Proxy( 
    IResponseDevice * This,
    /* [retval][out] */ BSTR *parameters);


void __RPC_STUB IResponseDevice_getProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_setProperties_Proxy( 
    IResponseDevice * This,
    /* [in] */ BSTR parameters);


void __RPC_STUB IResponseDevice_setProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getButtonCount_Proxy( 
    IResponseDevice * This,
    /* [retval][out] */ unsigned long *count);


void __RPC_STUB IResponseDevice_getButtonCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getButtonNames_Proxy( 
    IResponseDevice * This,
    /* [out] */ BSTR *names,
    /* [out][in] */ unsigned long *size);


void __RPC_STUB IResponseDevice_getButtonNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_acquire_Proxy( 
    IResponseDevice * This,
    /* [in] */ unsigned char useButtons,
    /* [in] */ unsigned char useAxes,
    /* [retval][out] */ IActiveResponseDevice **device);


void __RPC_STUB IResponseDevice_acquire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getAxisCount_Proxy( 
    IResponseDevice * This,
    /* [out] */ unsigned long *count,
    /* [out] */ AxisType *type);


void __RPC_STUB IResponseDevice_getAxisCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getAxisNames_Proxy( 
    IResponseDevice * This,
    /* [out] */ BSTR *names,
    /* [out][in] */ unsigned long *size);


void __RPC_STUB IResponseDevice_getAxisNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDevice_getLastError_Proxy( 
    IResponseDevice * This,
    /* [retval][out] */ BSTR *error);


void __RPC_STUB IResponseDevice_getLastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResponseDevice_INTERFACE_DEFINED__ */


#ifndef __IResponseDeviceEnumerator_INTERFACE_DEFINED__
#define __IResponseDeviceEnumerator_INTERFACE_DEFINED__

/* interface IResponseDeviceEnumerator */
/* [object][oleautomation][dual][version][uuid] */ 


EXTERN_C const IID IID_IResponseDeviceEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7F2C1D70-B69A-4488-817E-17140910773B")
    IResponseDeviceEnumerator : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT __stdcall getDeviceCount( 
            /* [retval][out] */ unsigned long *count) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getDevice( 
            /* [in] */ unsigned long index,
            /* [retval][out] */ IResponseDevice **device) = 0;
        
        virtual /* [id] */ HRESULT __stdcall getLastError( 
            /* [retval][out] */ BSTR *error) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IResponseDeviceEnumeratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IResponseDeviceEnumerator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IResponseDeviceEnumerator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IResponseDeviceEnumerator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IResponseDeviceEnumerator * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IResponseDeviceEnumerator * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IResponseDeviceEnumerator * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IResponseDeviceEnumerator * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( __stdcall *getDeviceCount )( 
            IResponseDeviceEnumerator * This,
            /* [retval][out] */ unsigned long *count);
        
        /* [id] */ HRESULT ( __stdcall *getDevice )( 
            IResponseDeviceEnumerator * This,
            /* [in] */ unsigned long index,
            /* [retval][out] */ IResponseDevice **device);
        
        /* [id] */ HRESULT ( __stdcall *getLastError )( 
            IResponseDeviceEnumerator * This,
            /* [retval][out] */ BSTR *error);
        
        END_INTERFACE
    } IResponseDeviceEnumeratorVtbl;

    interface IResponseDeviceEnumerator
    {
        CONST_VTBL struct IResponseDeviceEnumeratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IResponseDeviceEnumerator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IResponseDeviceEnumerator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IResponseDeviceEnumerator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IResponseDeviceEnumerator_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IResponseDeviceEnumerator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IResponseDeviceEnumerator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IResponseDeviceEnumerator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IResponseDeviceEnumerator_getDeviceCount(This,count)	\
    (This)->lpVtbl -> getDeviceCount(This,count)

#define IResponseDeviceEnumerator_getDevice(This,index,device)	\
    (This)->lpVtbl -> getDevice(This,index,device)

#define IResponseDeviceEnumerator_getLastError(This,error)	\
    (This)->lpVtbl -> getLastError(This,error)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT __stdcall IResponseDeviceEnumerator_getDeviceCount_Proxy( 
    IResponseDeviceEnumerator * This,
    /* [retval][out] */ unsigned long *count);


void __RPC_STUB IResponseDeviceEnumerator_getDeviceCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDeviceEnumerator_getDevice_Proxy( 
    IResponseDeviceEnumerator * This,
    /* [in] */ unsigned long index,
    /* [retval][out] */ IResponseDevice **device);


void __RPC_STUB IResponseDeviceEnumerator_getDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT __stdcall IResponseDeviceEnumerator_getLastError_Proxy( 
    IResponseDeviceEnumerator * This,
    /* [retval][out] */ BSTR *error);


void __RPC_STUB IResponseDeviceEnumerator_getLastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IResponseDeviceEnumerator_INTERFACE_DEFINED__ */



#ifndef __ResponseExtension_LIBRARY_DEFINED__
#define __ResponseExtension_LIBRARY_DEFINED__

/* library ResponseExtension */
/* [uuid] */ 





EXTERN_C const IID LIBID_ResponseExtension;
#endif /* __ResponseExtension_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


