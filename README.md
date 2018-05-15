**2018.05.11：完成了Vin_Thread类、Vin_ThreadPool类、Vin_Task类、Vin_TaskQueue类以及Vin_Task_PriorityQueue类，以及一些辅助函数。**

# Thread_Pool使用简介
Thread_Pool是模板类，默认使用Vin_TaskQueue作为Task的队列，Task封装了std::function，并添加了优先级，需要使用优先队列时，需要：
```cpp
Vin_ThreadPool<std::shared_ptr<Vin_Task>,Vin_TaskPriorityQueue<std::shared_ptr<Vin_Task> > > tpool;
```
- 这里的std::shared_ptr<Vin_Task>也可以是普通指针，但请注意自行释放内存，Vin_Task可以替换为自己的任务类，需要实现operator <；
- Vin_TaskPriorityQueue以及Vin_Task_Queue默认使用deque作为存储模板

## Tips
1. 尽量使用智能指针进行内存管理；
2. 多线程操作时的mutex及cond操作比较复杂，使用std::unique_lock实现智能的锁管理；

**2018.05.13：这里已经完成了Vin_Codec类、Vin_Base64类、Vin_Singleton类。**

# Vin_Singleton类使用简介
实现了两种单例模式：饿汉、懒汉，且都是线程安全的。使用的话，在你的类里注明一个友元类，并将构造/析构/拷贝控制函数设为私有或delete：
```cpp
class Test{
  friend class Vin_Starver<Vin_Base64>;
  //friend class Vin_Lazer<Vin_Base64>;
private:
  Test();
  ~Test();
  Test(Test&);
  Test& operator= (Test&);
}
```
然后调用
```cpp
Test* t = Vin_Singleto<Test,Vin_Starver>::getInstance();
```
可以获取全局唯一的Test对象。

# Vin_Base64
该类为一个单例类，利用Vin_Singleton获取其实例，可以实现base64编解码。它继承于Vin_Codec类，该类为一切编解码类的基类。

**2018.05.15：这里已经完成了Vin_MemoryPool类。**

# Vin_MemoryPool类使用简介
该类实现了缓存池，且可以线程安全地分配/回收资源，适用于需要经常申请释放内存的应用，以避免高额的内存分配开销和内存碎片化。
```
Vin_MemoryPool mpool(32,1024);
Vin_Slice s=mpool.Allocate(32);
mpool.Deallocate(s);
```


