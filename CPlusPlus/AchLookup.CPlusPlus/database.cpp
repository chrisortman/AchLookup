#include "stdafx.h"

#include <sqlce_err.h>
#include <sqlce_oledb.h>
#include <sqlce_sync.h>

HRESULT				hr						= NOERROR;
DBPROPSET			dbpropset[2];
DBPROP				dbprop[1]; // property array to initialize the provider
DBPROP				sscedbprop[3]; // property array for security and case-sensitivity properties

INT					i						= 0;
IDBDataSourceAdmin  *pIDBDataSourceAdmin	= NULL;
IUnknown			*pIUnknownSession		= NULL;

int create_database()
{
	CoInitialize(NULL);
	hr = CoCreateInstance(CLSID_SQLSERVERCE, 0, CLSCTX_INPROC_SERVER,
		IID_IDBDataSourceAdmin, (void**)& pIDBDataSourceAdmin);

	if(FAILED(hr))
	{
		goto Exit;
	}

	//initialize property structures
	VariantInit(&dbprop[0].vValue);
	for(int i=0; i< sizeof(sscedbprop) / sizeof(sscedbprop[0]); i++)
	{
		VariantInit(&sscedbprop[i].vValue);
	}

	//specify the property with name of the database
	dbprop[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
	dbprop[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	dbprop[0].vValue.vt = VT_BSTR;
	dbprop[0].vValue.bstrVal = SysAllocString(L"c:\\code\\achlookup\\db.sdf");

	if(NULL == dbprop[0].vValue.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	//specify the encryption mode.
	sscedbprop[0].dwPropertyID = DBPROP_SSCE_ENCRYPTIONMODE;
	sscedbprop[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	sscedbprop[0].vValue.vt = VT_I4;
	sscedbprop[0].vValue.intVal = DBPROPVAL_SSCE_EM_PLATFORM_DEFAULT;

	sscedbprop[1].dwPropertyID = DBPROP_SSCE_DBPASSWORD;
	sscedbprop[1].dwOptions = DBPROPOPTIONS_REQUIRED;
	sscedbprop[1].vValue.vt = VT_BSTR;
	sscedbprop[1].vValue.bstrVal = SysAllocString(L"welcome");

	if(NULL == sscedbprop[1].vValue.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	//set the case sensitivity to true
	sscedbprop[2].dwPropertyID = DBPROP_SSCE_DBCASESENSITIVE;
	sscedbprop[2].dwOptions = DBPROPOPTIONS_REQUIRED;
	sscedbprop[2].vValue.vt = VT_BOOL;
	sscedbprop[2].vValue.boolVal = VARIANT_TRUE;

	dbpropset[0].guidPropertySet = DBPROPSET_DBINIT;
	dbpropset[0].rgProperties = dbprop;
	dbpropset[0].cProperties = sizeof(dbprop)/sizeof(dbprop[0]);

	dbpropset[1].guidPropertySet = DBPROPSET_SSCE_DBINIT;
	dbpropset[1].rgProperties = sscedbprop;
	dbpropset[1].cProperties = sizeof(sscedbprop)/sizeof(sscedbprop[0]);

	//create and initialize the database.
	hr = pIDBDataSourceAdmin->CreateDataSource(sizeof(dbpropset)/sizeof(dbpropset[0]),
		dbpropset,NULL,IID_IUnknown, &pIUnknownSession);

	if(FAILED(hr))
	{
		goto Exit;
	}

	//at this point the database is created

Exit:
	//probably should do more cleanup
	CoUninitialize();
	return 0;

}