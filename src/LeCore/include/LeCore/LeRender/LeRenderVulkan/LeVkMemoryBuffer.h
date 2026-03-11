

struct LeVkMemoryBuffer
{

    LeVkMemoryBuffer();
    ~LeVkMemoryBuffer();

    void Update(const void* pData, size_t Size);

    VkDevice device;

    VkDeviceMemory mem;
    VkBuffer buffer;
    VkDeviceSize allocationSize;

};