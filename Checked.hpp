#pragma once

#define CHECKED_UNINIT_BYTE 0xCD
#define CHECKED_FREED_BYTE 0xFD

#ifdef _WIN64
    #define CHECKED_UNINIT_POINTER ((void*)(size_t)0xCDCDCDCDCDCDCDCD)
    #define CHECKED_FREED_POINTER ((void*)(size_t)0xFDFDFDFDFDFDFDFD)
#else
    #define CHECKED_UNINIT_POINTER ((void*)(size_t)0xCDCDCDCD)
    #define CHECKED_FREED_POINTER ((void*)(size_t)0xFDFDFDFD)
#endif

#ifndef CHECKED_ASSERT
    #ifdef CHECKED_NEEDS_SPEED
        #define CHECKED_ASSERT(x) /*empty*/
    #else
        #ifndef ASSERT
            #include <assert.h>
            #define ASSERT assert
        #endif
        #define CHECKED_ASSERT(x) ASSERT(x)
    #endif
#endif

template <typename T_ITEM, size_t t_count>
struct CheckedArray
{
    typedef CheckedArray<T_ITEM, t_count> T_SELF;

    T_ITEM m_items[t_count];

    CheckedArray()
    {
#ifndef CHECKED_NEEDS_SPEED
        memset(m_items, CHECKED_UNINIT_BYTE, t_count * sizeof(T_ITEM));
#endif
    }

    explicit CheckedArray(const T_ITEM (&array)[t_count])
    {
        memcpy(m_items, array, sizeof(m_items));
    }

    CheckedArray(const T_SELF& array)
    {
        memcpy(m_items, array.m_items, sizeof(m_items));
    }

    T_SELF& operator=(const T_ITEM (&array)[t_count])
    {
        memcpy(m_items, array, sizeof(m_items));
        return *this;
    }

    T_SELF& operator=(const T_SELF& array)
    {
        memcpy(m_items, array.m_items, sizeof(m_items));
        return *this;
    }

    ~CheckedArray()
    {
#ifndef CHECKED_NEEDS_SPEED
        memset(m_items, CHECKED_FREED_BYTE, t_count * sizeof(T_ITEM));
#endif
    }

    size_t size() const
    {
        return t_count;
    }

    size_t byte_size() const
    {
        return t_count * sizeof(T_ITEM);
    }

    T_ITEM* data()
    {
        return m_items;
    }

    const T_ITEM* data() const
    {
        return m_items;
    }

    explicit operator T_ITEM*()
    {
        return m_items;
    }

    explicit operator const T_ITEM*() const
    {
        return m_items;
    }

    T_ITEM* operator&()
    {
        return m_items;
    }

    const T_ITEM* operator&() const
    {
        return m_items;
    }

    T_ITEM& operator[](size_t index)
    {
        CHECKED_ASSERT(index < t_count);
        return m_items[index];
    }

    const T_ITEM& operator[](size_t index) const
    {
        CHECKED_ASSERT(index < t_count);
        return m_items[index];
    }

    bool operator==(const T_ITEM* items) const
    {
        return m_items == items;
    }

    bool operator!=(const T_ITEM* items) const
    {
        return m_items != items;
    }
};

struct CheckedAllocFree
{
    void *checked_malloc(size_t cb)
    {
        return malloc(cb);
    }

    void checked_free(void *ptr)
    {
        free(ptr);
    }
};

template <typename T_ITEM, typename T_ALLOC_FREE = CheckedAllocFree>
struct CheckedVector : T_ALLOC_FREE
{
    typedef CheckedVector<T_ITEM, T_ALLOC_FREE> T_SELF;

    T_ITEM* m_items;
    size_t m_count;

    CheckedVector() : m_items(NULL), m_count(0)
    {
    }

    explicit CheckedVector(size_t count) : m_items(NULL), m_count(0)
    {
        Alloc(count);
    }

    template <size_t t_count>
    explicit CheckedVector(const T_ITEM (&array)[t_count]) : m_items(NULL), m_count(0)
    {
        if (Alloc(t_count))
            memcpy(m_items, array, t_count * sizeof(T_ITEM));
    }

    CheckedVector(const T_ITEM* items, size_t count) : m_items(NULL), m_count(0)
    {
        CHECKED_ASSERT(items != CHECKED_UNINIT_POINTER);
        CHECKED_ASSERT(items != CHECKED_FREED_POINTER);
        if (Alloc(count))
            memcpy(m_items, items, count * sizeof(T_ITEM));
    }

    explicit CheckedVector(const T_SELF& array) : m_items(NULL), m_count(0)
    {
        CHECKED_ASSERT(array.m_items != CHECKED_UNINIT_POINTER);
        CHECKED_ASSERT(array.m_items != CHECKED_FREED_POINTER);
        if (Alloc(array.m_count))
            memcpy(m_items, array.m_items, array.m_count * sizeof(T_ITEM));
    }

    template <size_t t_count>
    T_SELF& operator=(const T_ITEM (&array)[t_count])
    {
        if (Alloc(t_count))
            memcpy(m_items, array, t_count * sizeof(T_ITEM));
        return *this;
    }

    T_SELF& operator=(const T_SELF& array)
    {
        CHECKED_ASSERT(array.m_items != CHECKED_UNINIT_POINTER);
        CHECKED_ASSERT(array.m_items != CHECKED_FREED_POINTER);
        if (this != &array)
        {
            if (Alloc(array.m_count))
                memcpy(m_items, array.m_items, array.m_count * sizeof(T_ITEM));
        }
        return *this;
    }

    ~CheckedVector()
    {
        Free();
    }

    size_t size() const
    {
        return m_count;
    }

    size_t byte_size() const
    {
        return m_count * sizeof(T_ITEM);
    }

    T_ITEM* data()
    {
        return m_items;
    }

    const T_ITEM* data() const
    {
        return m_items;
    }

    explicit operator T_ITEM*()
    {
        return m_items;
    }
    
    explicit operator const T_ITEM*() const
    {
        return m_items;
    }

    T_ITEM* operator&()
    {
        return m_items;
    }

    const T_ITEM* operator&() const
    {
        return m_items;
    }

    T_ITEM& operator[](size_t index)
    {
        CHECKED_ASSERT(m_count == 0 || m_items != NULL);
        CHECKED_ASSERT(index < m_count);
        return m_items[index];
    }

    const T_ITEM& operator[](size_t index) const
    {
        CHECKED_ASSERT(m_count == 0 || m_items != NULL);
        CHECKED_ASSERT(index < m_count);
        return m_items[index];
    }

    bool Alloc(size_t cNew)
    {
        Free();

        if (cNew == 0)
            return true;

        size_t cbNew = cNew * sizeof(T_ITEM);
        T_ITEM* items = (T_ITEM*)T_ALLOC_FREE::checked_malloc(cbNew);
        if (items == NULL)
        {
            CHECKED_ASSERT(0);
            return false;
        }

        PostAlloc(items, cNew);

        m_count = cNew;
        m_items = items;
        return true;
    }

    void Free()
    {
        if (m_items == NULL)
            return;

        PreFree(m_items, m_count);

        m_count = 0;

        T_ITEM* old_items = m_items;
        m_items = NULL;
        T_ALLOC_FREE::checked_free(old_items);
    }

    bool ReAlloc(size_t cNew)
    {
        CHECKED_ASSERT(m_items != CHECKED_UNINIT_POINTER);
        CHECKED_ASSERT(m_items != CHECKED_FREED_POINTER);

        if (cNew == 0)
        {
            Free();
            return true;
        }

        if (cNew == m_count)
            return true;

        if (cNew < m_count)
        {
#ifndef CHECKED_NEEDS_SPEED
            size_t offset = cNew * sizeof(T_ITEM);
            size_t amount = (m_count - cNew) * sizeof(T_ITEM);
            memset(((unsigned char*)m_items) + offset, CHECKED_FREED_BYTE, amount);
#endif
            return true;
        }

        size_t cbNew = cNew * sizeof(T_ITEM);
        T_ITEM* new_items = (T_ITEM*)T_ALLOC_FREE::checked_malloc(cbNew);
        if (new_items == NULL)
        {
            CHECKED_ASSERT(false);
            return false;
        }

#ifndef CHECKED_NEEDS_SPEED
        memset(new_items, CHECKED_UNINIT_BYTE, cNew * sizeof(T_ITEM));
#endif

        memcpy(new_items, m_items, m_count * sizeof(T_ITEM));

        PreFree(m_items, m_count);

        T_ITEM* old_items = m_items;
        m_items = new_items;
        T_ALLOC_FREE::checked_free(old_items);

        m_count = cNew;
        return true;
    }

    void Attach(T_ITEM* items, size_t count)
    {
        Free();
        m_items = items;
        m_count = count;
    }

    T_ITEM* Detach()
    {
        m_count = 0;
        T_ITEM* items = m_items;
        m_items = NULL;
        return items;
    }

    bool operator==(const T_ITEM* items) const
    {
        return m_items == items;
    }

    bool operator!=(const T_ITEM* items) const
    {
        return m_items != items;
    }

    void PostAlloc(T_ITEM* items, size_t count)
    {
#ifndef CHECKED_NEEDS_SPEED
        memset(items, CHECKED_UNINIT_BYTE, count * sizeof(T_ITEM));
#endif
    }

    void PreFree(T_ITEM* items, size_t count)
    {
        if (items == NULL)
            return;

        CHECKED_ASSERT(items != CHECKED_UNINIT_POINTER); // uninitialized pointer?
        CHECKED_ASSERT(items != CHECKED_FREED_POINTER); // freed pointer?

#ifndef CHECKED_NEEDS_SPEED
        unsigned char* p = (unsigned char*)items;
        for (size_t i = 0; i < count * sizeof(T_ITEM); ++i)
        {
            if (p[i] != CHECKED_FREED_BYTE)
            {
                p = NULL;
                break;
            }
        }
        CHECKED_ASSERT(p == NULL); // double free?

        memset(items, CHECKED_FREED_BYTE, count * sizeof(T_ITEM));
#endif
    }
};

#ifdef __REACTOS__
template <int t_nPoolType, unsigned long t_nTag>
struct CheckedExPoolAllocFree
{
    void *checked_malloc(size_t count)
    {
        return ::ExAllocatePoolWithTag(t_nPoolType, count, t_nTag);
    }

    void checked_free(void *ptr)
    {
        return ::ExFreePoolWithTag(ptr, t_nTag);
    }
};

template <typename T_ITEM, int t_nPoolType, unsigned long t_nTag>
struct CheckedExPoolVector : CheckedVector<T_ITEM, CheckedExPoolAllocFree<t_nPoolType, t_nTag> >
{
    typedef CheckedVector<T_ITEM, CheckedExPoolAllocFree<t_nPoolType, t_nTag> > T_SUPER;
    typedef CheckedExPoolVector<T_ITEM, t_nPoolType, t_nTag> T_SELF;

    CheckedExPoolVector() : T_SUPER()
    {
    }

    CheckedExPoolVector(size_t count) : T_SUPER(count)
    {
    }

    template <size_t t_count>
    CheckedExPoolVector(const T_ITEM (&array)[t_count]) : T_SUPER(array)
    {
    }

    CheckedExPoolVector(const T_ITEM* items, size_t count) : T_SUPER(items, count)
    {
    }

    CheckedExPoolVector(const T_SELF& array) : T_SUPER(array)
    {
    }
};
#endif /* def __REACTOS__ */
