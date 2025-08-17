# Cachelab

------

注：`verbose` 功能不完整

# <img src="../images/cachelab1.png" alt="cachelab1" style="zoom:67%;" />

一些想法和经验：

1. 用时间戳来记录最近一次被访问的时间
2. 如果是modify，命中次数直接加1即可，因为load后一定会使需要再store的数据存在于缓存中，store时一定会命中
3. 最后记得 `free` 分配的内存