#pragma once

#include <cstdint>
 
// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/dt_recv.h#L87
 
enum class SendPropType : int
{
    INT = 0,
    FLOAT,
    VECTOR,
    VECTOR2D,
    STRING,
    ARRAY,
    DATATABLE,
    INT64,
    SENDPROPTYPEMAX
};
 
struct DataVariant
{
    union
    {
        float   Float;
        long    Int;
        char* String;
        void* Data;
        float   Vector[3];
        int64_t Int64;
    };
 
    SendPropType type;
};
 
struct RecvProp;
struct RecvTable
{
    RecvProp* props;
    int propsCount;
    void* decoder;
    char* tableName;
    bool initialized;
    bool inMainList;
};
 
struct RecvProp
{
    char* varName;
    SendPropType recvType;
    int flags;
    int stringBufferSize;
    bool insideArray;
    const void* extraData;
    RecvProp* arrayProp;
    void* arrayLengthProxyFn;
    void* proxyFn;
    void* dataTableProxyFn;
    RecvTable* dataTable;
    int offset;
    int elementStride;
    int elements;
    const char* parentArrayPropName;
};
 
class ClientClass
{
public:
    void* CreateClientClassFn;
    void* CreateEventFn;
    char* networkName;
    RecvTable* recvTable;
    ClientClass* next;
    int classID;
    const char* mapClassname;
};
 