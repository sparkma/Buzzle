
#include "BZStageTest.h"

#if defined(_DEBUG) && 0

#include "support/data_support/uthash.h"
#include "CCObject.h"
#include "CCArray.h"
#include "CCString.h"

#undef CCDictionary
class CCDictionary;

class CCDictElement
{
    #define MAX_KEY_LEN  256
public:
    CCDictElement(const char* pszKey, CCObject* pObject)
    {
        init();
        m_pObject = pObject;

        const char* pStart = pszKey;
        
        int len = strlen(pszKey);
        if (len > MAX_KEY_LEN )
        {
            char* pEnd = (char*)&pszKey[len-1];
            pStart = pEnd - (MAX_KEY_LEN-1);
        }

        strcpy(m_szKey, pStart);
    }

    CCDictElement(int iKey, CCObject* pObject)
    {
        init();
        m_iKey = iKey;
        m_pObject = pObject;
    }

    inline const char* getStrKey() const
    {
        CCAssert(m_szKey[0] != '\0', "Should not call this function for integer dictionary");
        return m_szKey;
    }

    inline int getIntKey() const 
    {
        CCAssert(m_szKey[0] == '\0', "Should not call this function for string dictionary");
        return m_iKey;
    }

    inline CCObject* getObject() const
    {
        return m_pObject;
    }

private:
    inline void init()
    {
        m_iKey = 0;
        m_pObject = NULL;
        memset(m_szKey, 0, sizeof(m_szKey));
        memset(&hh, 0, sizeof(hh));
    }

private:
    char m_szKey[MAX_KEY_LEN+1];  /** hash key of string type*/
    int  m_iKey;        /** hash key of integer type */
    CCObject* m_pObject;/** hash value */
public:
    UT_hash_handle hh; /* makes this class hashable */
    friend class CCDictionary;
};


#define CCDICT_FOREACH(__dict__, __el__) \
    CCDictElement* pTmp##__dict__##__el__ = NULL; \
    HASH_ITER(hh, (__dict__)->m_pElements, __el__, pTmp##__dict__##__el__)


class CCDictionary : public CCObject
{
public:
    CCDictionary();
    ~CCDictionary();

    /// return the number of items
    unsigned int count();

    /// return all the keys
    CCArray* allKeys();

    /** @warning : We use '==' to compare two objects*/
    CCArray* allKeysForObject(CCObject* object);

    CCObject* objectForKey(const std::string& key);
    CCObject* objectForKey(int key);
    const CCString* valueForKey(const std::string& key);
    const CCString* valueForKey(int key);


    void setObject(CCObject* pObject, const std::string& key);
    void setObject(CCObject* pObject, int key);

    void removeObjectForKey(const std::string& key);
    void removeObjectForKey(int key);
    void removeObjectsForKeys(CCArray* pKeyArray);
    void removeObjectForElememt(CCDictElement* pElement);
    void removeAllObjects();

    virtual CCObject* copyWithZone(CCZone* pZone);

    /* static functions */
    static CCDictionary* dictionary();

    static CCDictionary* dictionaryWithDictionary(CCDictionary* srcDict);
#if 0
    /**
    @brief   Generate a CCDictionary pointer by file
    @param   pFileName  The file name of *.plist file
    @return  The CCDictionary pointer generated from the file
    */
    static CCDictionary* dictionaryWithContentsOfFile(const char *pFileName);

    /*
    @brief The same meaning as dictionaryWithContentsOfFile(), but it doesn't call autorelease, so the
           invoker should call release().
    */
    static CCDictionary* dictionaryWithContentsOfFileThreadSafe(const char *pFileName);
#endif

private:
    void setObjectUnSafe(CCObject* pObject, const std::string& key);
    void setObjectUnSafe(CCObject* pObject, const int key);
    
public:
    CCDictElement* m_pElements;
private:
    
    enum CCDictType
    {
        kCCDictUnknown = 0,
        kCCDictStr,
        kCCDictInt
    };
    CCDictType m_eDictType;
    CCDictType m_eOldDictType;
};

#include "CCString.h"
//#include "CCInteger.h"


class CCInteger : public CCObject
{
public:
    CCInteger(int v)
        : m_nValue(v) {}
    int getValue() const {return m_nValue;}

    static CCInteger* integerWithInt(int v)
    {
        CCInteger* pRet = new CCInteger(v);
        pRet->autorelease();
        return pRet;
    }
private:
    int m_nValue;
};


using namespace std;

CCDictionary::CCDictionary()
: m_pElements(NULL)
, m_eDictType(kCCDictUnknown)
, m_eOldDictType(kCCDictUnknown)
{

}

CCDictionary::~CCDictionary()
{
    removeAllObjects();
}

unsigned int CCDictionary::count()
{
    return HASH_COUNT(m_pElements);
}

CCArray* CCDictionary::allKeys()
{
    int iKeyCount = this->count();
    if (iKeyCount <= 0) return NULL;
    CCArray* pArray = CCArray::arrayWithCapacity(iKeyCount);

    CCDictElement *pElement, *tmp;
    if (m_eDictType == kCCDictStr)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            CCString* pOneKey = new CCString(pElement->m_szKey);
            pOneKey->autorelease();
            pArray->addObject(pOneKey);
        }
    }
    else if (m_eDictType == kCCDictInt)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            CCInteger* pOneKey = new CCInteger(pElement->m_iKey);
            pOneKey->autorelease();
            pArray->addObject(pOneKey);
        }
    }
    
    return pArray;
}

CCArray* CCDictionary::allKeysForObject(CCObject* object)
{
    int iKeyCount = this->count();
    if (iKeyCount <= 0) return NULL;
    CCArray* pArray = CCArray::array();

    CCDictElement *pElement, *tmp;

    if (m_eDictType == kCCDictStr)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            if (object == pElement->m_pObject)
            {
                CCString* pOneKey = new CCString(pElement->m_szKey);
                pOneKey->autorelease();
                pArray->addObject(pOneKey);
            }
        }
    }
    else if (m_eDictType == kCCDictInt)
    {
        HASH_ITER(hh, m_pElements, pElement, tmp) 
        {
            if (object == pElement->m_pObject)
            {
                CCInteger* pOneKey = new CCInteger(pElement->m_iKey);
                pOneKey->autorelease();
                pArray->addObject(pOneKey);
            }
        }
    }
    return pArray;
}

CCObject* CCDictionary::objectForKey(const std::string& key)
{
    if (m_eDictType == kCCDictUnknown && m_eDictType == kCCDictUnknown) return NULL;
    CCAssert(m_eDictType == kCCDictStr, "this dictionary does not use string as key.");

    CCObject* pRetObject = NULL;
    CCDictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    if (pElement != NULL)
    {
        pRetObject = pElement->m_pObject;
    }
    return pRetObject;
}

CCObject* CCDictionary::objectForKey(int key)
{
    if (m_eDictType == kCCDictUnknown && m_eDictType == kCCDictUnknown) return NULL;
    CCAssert(m_eDictType == kCCDictInt, "this dictionary does not use integer as key.");

    CCObject* pRetObject = NULL;
    CCDictElement *pElement = NULL;
    HASH_FIND_INT(m_pElements, &key, pElement);
    if (pElement != NULL)
    {
        pRetObject = pElement->m_pObject;
    }
    return pRetObject;
}

inline CCString* CCString_stringWithCString(const char* psz)
{
	CCString* ps = new CCString(psz);
	ps->autorelease();
	return ps;
}

const CCString* CCDictionary::valueForKey(const std::string& key)
{
    CCString* pStr = (CCString*)objectForKey(key);
    if (pStr == NULL)
    {
        pStr = CCString_stringWithCString("");
    }
    return pStr;
}

const CCString* CCDictionary::valueForKey(int key)
{
    CCString* pStr = (CCString*)objectForKey(key);
    if (pStr == NULL)
    {
        pStr = CCString_stringWithCString("");
    }
    return pStr;
}

void CCDictionary::setObject(CCObject* pObject, const std::string& key)
{
    CCAssert(key.length() > 0 && pObject != NULL, "Invalid Argument!");
    if (m_eOldDictType == kCCDictUnknown)
    {
        m_eOldDictType = kCCDictStr;
    }
    m_eDictType = kCCDictStr;
    CCAssert(m_eDictType == m_eOldDictType, "this dictionary does not use string as key.");

    CCDictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    if (pElement == NULL)
    {
        setObjectUnSafe(pObject, key);
    }
    else if (pElement->m_pObject != pObject)
    {
        CCObject* pTmpObj = pElement->m_pObject;
        pTmpObj->retain();
        removeObjectForElememt(pElement);
        setObjectUnSafe(pObject, key);
        pTmpObj->release();
    }
}

void CCDictionary::setObject(CCObject* pObject, int key)
{
    CCAssert(pObject != NULL, "Invalid Argument!");
    if (m_eOldDictType == kCCDictUnknown)
    {
        m_eOldDictType = kCCDictInt;
    }
    m_eDictType = kCCDictInt;
    CCAssert(m_eDictType == m_eOldDictType, "this dictionary does not use integer as key.");

    CCDictElement *pElement = NULL;
    HASH_FIND_INT(m_pElements, &key, pElement);
    if (pElement == NULL)
    {
        setObjectUnSafe(pObject, key);
    }
    else if (pElement->m_pObject != pObject)
    {
        CCObject* pTmpObj = pElement->m_pObject;
        pTmpObj->retain();
        removeObjectForElememt(pElement);
        setObjectUnSafe(pObject, key);
        pTmpObj->release();
    }

}

void CCDictionary::removeObjectForKey(const std::string& key)
{
    if (m_eOldDictType == kCCDictUnknown) 
    {
        return;
    }
    CCAssert(m_eDictType == kCCDictStr, "this dictionary does not use string as its key");
    CCAssert(key.length() > 0, "Invalid Argument!");
    CCDictElement *pElement = NULL;
    HASH_FIND_STR(m_pElements, key.c_str(), pElement);
    removeObjectForElememt(pElement);
}

void CCDictionary::removeObjectForKey(int key)
{
    if (m_eOldDictType == kCCDictUnknown) 
    {
        return;
    }
    CCAssert(m_eDictType == kCCDictInt, "this dictionary does not use integer as its key");
    CCDictElement *pElement = NULL;
    HASH_FIND_INT(m_pElements, &key, pElement);
    removeObjectForElememt(pElement);
}

void CCDictionary::setObjectUnSafe(CCObject* pObject, const std::string& key)
{
    pObject->retain();
    CCDictElement* pElement = new CCDictElement(key.c_str(), pObject);
    HASH_ADD_STR(m_pElements, m_szKey, pElement);
}

void CCDictionary::setObjectUnSafe(CCObject* pObject, const int key)
{
    pObject->retain();
    CCDictElement* pElement = new CCDictElement(key, pObject);
    HASH_ADD_INT(m_pElements, m_iKey, pElement);
}

void CCDictionary::removeObjectsForKeys(CCArray* pKeyArray)
{
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(pKeyArray, pObj)
    {
        CCString* pStr = (CCString*)pObj;
        //removeObjectForKey(pStr->getCString());
		removeObjectForKey(pStr->m_sString.c_str());
    }
}

void CCDictionary::removeObjectForElememt(CCDictElement* pElement)
{
    if (pElement != NULL)
    {
        HASH_DEL(m_pElements, pElement);
        pElement->m_pObject->release();
        CC_SAFE_DELETE(pElement);
    }
}

void CCDictionary::removeAllObjects()
{
    CCDictElement *pElement, *tmp;
    HASH_ITER(hh, m_pElements, pElement, tmp) 
    {
        HASH_DEL(m_pElements, pElement);
        pElement->m_pObject->release();
        CC_SAFE_DELETE(pElement);
    }
}

CCObject* CCDictionary::copyWithZone(CCZone* pZone)
{
    CCAssert(pZone == NULL, "CCDirctionary should not be inherited.");
    CCDictionary* pNewDict = new CCDictionary();

    CCDictElement* pElement = NULL;
    if (m_eDictType == kCCDictInt)
    {
        CCDICT_FOREACH(this, pElement)
        {
            pNewDict->setObject(pElement->getObject()->copy()->autorelease(), pElement->getIntKey());
        }
    }
    else if (m_eDictType == kCCDictStr)
    {
        CCDICT_FOREACH(this, pElement)
        {
            pNewDict->setObject(pElement->getObject()->copy()->autorelease(), pElement->getStrKey());
        }
    }

    return pNewDict;
}

CCDictionary* CCDictionary::dictionary()
{
    CCDictionary* pRet = new CCDictionary();
    if (pRet != NULL)
    {
        pRet->autorelease();
    }
    return pRet;
}

CCDictionary* CCDictionary::dictionaryWithDictionary(CCDictionary* srcDict)
{
    CCDictionary* pNewDict = (CCDictionary*)srcDict->copy();
    pNewDict->autorelease();
    return pNewDict;
}

#if 0
extern CCDictionary* ccFileUtils_dictionaryWithContentsOfFileThreadSafe(const char *pFileName);

CCDictionary* CCDictionary::dictionaryWithContentsOfFileThreadSafe(const char *pFileName)
{
    return ccFileUtils_dictionaryWithContentsOfFileThreadSafe(pFileName);
}

CCDictionary* CCDictionary::dictionaryWithContentsOfFile(const char *pFileName)
{
    CCDictionary* pRet = dictionaryWithContentsOfFileThreadSafe(pFileName);
    pRet->autorelease();
    return pRet;
}
#endif

class CCTest : public CCObject
{
public:
	CCTest() { autorelease(); };
	virtual ~CCTest() {}
};

#include "AUtils.h"
#include "AString.h"

template <class _T>
void _test(CCObject* p, int times, bool add)
{

	_T* pdict = new _T();
	int i;
	for (i = 0; i < times; i++)
	{
		char sz[64];
#if 0
		sprintf(sz, "%c%c%c%c%c%c%c%c_%d", 
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			CAUtils::Rand() * 1000 + 23);
#else
		sprintf(sz, "%d", CAUtils::Rand() * times + 23);
#endif
		if (add)
		{
			{
				pdict->setObject(p, sz);
			}
		}
		else
		{
			{
				pdict->objectForKey(sz);
			}
		}
	}
}

template <class _T>
void _test_int(CCObject* p, int times, bool add)
{

	bool bnum = true;
	_T* pdict = new _T();
	int i;
	for (i = 0; i < times; i++)
	{
		char sz[64];
#if 0
		sprintf(sz, "%c%c%c%c%c%c%c%c_%d", 
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			'A' + (int)(CAUtils::Rand() * 26),
			CAUtils::Rand() * 1000 + 23);
#else
		sprintf(sz, "%d", CAUtils::Rand() * times + 23);
#endif
		if (add)
		{
			if (bnum)
			{
				unsigned int a = CAString::hash(sz);
				pdict->setObject(p, (int)a);
			}
		}
		else
		{
			if (bnum)
			{
				unsigned int a = CAString::hash(sz);
				pdict->objectForKey((int)a);
			}
		}
	}
}

#include "platform.h"
static double _timediff(struct cc_timeval* plast)
{
    struct cc_timeval now;

	//CCTime::gettimeofdayCocos2d(&now, NULL);
    LARGE_INTEGER liTime, liFreq;
	QueryPerformanceFrequency( &liFreq );
    QueryPerformanceCounter( &liTime );
    now.tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart - 7686000000);
    now.tv_usec    = (long)
		( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - 
		(liTime.QuadPart / liFreq.QuadPart) * 1000000.0);

	double diff = (double)(now.tv_sec - plast->tv_sec) + 
		(double)(now.tv_usec - plast->tv_usec) / 1000000.0;

	*plast = now;
	return diff;
}

#define TEST_TIMES	700000


void test()
{
	CCTest* p = new CCTest();
	//CCDictionary* pdict0 = new CCDicttionary();
	//CCMutableDictionary* pdict1 = new CCMutableDictionary<std::string>)();

    struct cc_timeval now;
	//CCTime::gettimeofdayCocos2d(&now, NULL);
	_timediff(&now);
	_test<CCMutableDictionary<std::string> >(p, TEST_TIMES, true);
	double diff1 = _timediff(&now);
	_test<CCDictionary>(p, TEST_TIMES, true);
	double diff2 = _timediff(&now);
	_test<CCMutableDictionary<std::string> >(p, TEST_TIMES, false);
	double diff3 = _timediff(&now);
	_test<CCDictionary>(p, TEST_TIMES, false);
	double diff4 = _timediff(&now);

	_test_int<CCMutableDictionary<int> >(p, TEST_TIMES, true);
	double diff5 = _timediff(&now);
	_test_int<CCDictionary>(p, TEST_TIMES, true);
	double diff6 = _timediff(&now);
	_test_int<CCMutableDictionary<int> >(p, TEST_TIMES, false);
	double diff7 = _timediff(&now);
	_test_int<CCDictionary>(p, TEST_TIMES, false);
	double diff8 = _timediff(&now);

	double p0 = diff2 * 100.0f / diff1;
	double p1 = diff4 * 100.0f / diff3;
	double p2 = diff6 * 100.0f / diff5;
	double p3 = diff8 * 100.0f / diff7;
	p0 = p1 = 0;
}

#endif

BZStageTest::BZStageTest(const char* name) : CAStage(name)
{
	GUARD_FUNCTION();
}

BZStageTest::~BZStageTest(void)
{
}
