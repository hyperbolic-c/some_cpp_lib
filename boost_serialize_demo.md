# 前言
在项目实现过程中, 需要实现数据的结构化输出与读入. 比如将一个 **`class`** 或 **`struct`** 包含的数据输出到文件中, 然后还要读取回来. \
上述过程可以用 **序列化 (serialization)** 来实现. 下面记录一下阅读[boost::serialization](https://www.boost.org/doc/libs/1_81_0/libs/serialization/doc/)官方[demo](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo.cpp)的过程. 内容主要是对其[Tutorial](https://www.boost.org/doc/libs/1_81_0/libs/serialization/doc/)的翻译.\
**序列化(serialization)** 是指将一组任意的 **`C++`** 数据结构可逆地解构为一个字节序列.这可以用来在某个程序环境中重建一个等价的解构.具体来说, 我们用 **存档 (archive)** 一词来指代这种字节序列的呈现. 这种序列可以是二进制数据, 文本数据, XML, 或者是由用户创建的文件.
# Tutorial
输出存档类似于输出数据流.可以用运算符 **`<<`** 或者 **`&`** 输出数据到存档中:
```bash
ar << data;
ar & data;
```
输入存档类似于输出存档.可以用运算符 **`>>`** 或者 **`&`** 从存档中读取数据:
```bash
ar >> data;
ar & data;
```
当这些运算符被原始数据类型调用时, 数据简单地保存到存档中或从存档中加载.当被类数据类型调用时, 会调用类的序列化函数. 每个序列化函数都使用上述的运算符来保存或加载类的数据成员. 该过程以递归的方式进行, 直到类中包含的所有数据都被保存或加载.
## A Very Simple Case
这些运算符在序列化函数中保存或者是加载类的数据成员. [demo.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo.cpp)说明了如何使用序列化这个库. 下面我们摘录这个 **`demo.cpp`** 的代码, 用最简单的情况说明这个库的使用方法.
```c++
#include <fstream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/////////////////////////////////////////////////////////////
// gps coordinate
//
// illustrates serialization for a simple type
//
class gps_position
{
private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & degrees;
        ar & minutes;
        ar & seconds;
    }
    int degrees;
    int minutes;
    float seconds;
public:
    gps_position(){};
    gps_position(int d, int m, float s) :
        degrees(d), minutes(m), seconds(s)
    {}
};

int main() {
    // create and open a character archive for output
    std::ofstream ofs("filename");

    // create class instance
    const gps_position g(35, 59, 24.567f);

    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        oa << g;
    	// archive and stream closed when destructors are called
    }

    // ... some time later restore the class instance to its orginal state
    gps_position newg;
    {
        // create and open an archive for input
        std::ifstream ifs("filename");
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> newg;
        // archive and stream closed when destructors are called
    }
    return 0;
}
```
对于每个需要通过序列化来保存的类, 必须有一个函数来保存这个类定义的所有数据成员. 对于每个需要通过序列化加载的类,必须有一个函数按照保存的顺序来加载这些数据成员. 在上面的例子中, 这些函数是由模板成员函数 **`serialize()`** 生成的.
## Non Intrusive Version
上述的实现是侵入性的. 也就是说,要修改那些实例被序列化的类的定义.在某些情况下, 这可能是不太方便的. 另一种等效的, 可行的表述是:
```c++
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class gps_position
{
public:
    int degrees;
    int minutes;
    float seconds;
    gps_position(){};
    gps_position(int d, int m, float s) :
        degrees(d), minutes(m), seconds(s)
    {}
};

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, gps_position & g, const unsigned int version)
{
    ar & g.degrees;
    ar & g.minutes;
    ar & g.seconds;
}

} // namespace serialization
} // namespace boost
```
在这个例子中, 序列化函数不再是 **`class gps_position`** 的成员, 但实现的功能完全相同的.\
非侵入性序列化的主要应用是允许在不修改类定义的情况下为类实现序列化. 但为了实现这个效果, 类必须暴露足够多的信息来重建.这个例子中, 我们假定这个类有公共成员--这不是一个常见的情况. 只有那些暴露足够的信息以保存和重建本身的类才能在无需改变类的定义的情况下实现序列化.
## Serializable Members
一个具有可序列化成员的可序列化类如下所示:
```c++
class bus_stop
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & latitude;
        ar & longitude;
    }
    gps_position latitude;
    gps_position longitude;
protected:
    bus_stop(const gps_position & lat_, const gps_position & long_) :
    latitude(lat_), longitude(long_)
    {}
public:
    bus_stop(){}
    // See item # 14 in Effective C++ by Scott Meyers.
    // re non-virtual destructors in base classes.
    virtual ~bus_stop(){}
};
```
也就是说, **`class`** 类型的成员可以像原始类型的成员一样被序列化. \
需要注意的是, 用一个存档运算符保存一个 **`bus_stop`** 类的实例将调用 **`serialize`** 函数, 该函数将保存经纬度(latitude longitude). 然后, 将通过调用 **`gps_position`** 中定义的 **`serialize`** 来保存具体的定位值(degrees minutes seconds). 通过这种方式, 应用存档运算符, 整个数据结构被保存为其的根项.
## Derived Classes
派生类应该包括其基类的序列化.
```c++
#include <boost/serialization/base_object.hpp>

class bus_stop_corner : public bus_stop
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        ar & boost::serialization::base_object<bus_stop>(*this);
        ar & street1;
        ar & street2;
    }
    std::string street1;
    std::string street2;
    virtual std::string description() const
    {
        return street1 + " and " + street2;
    }
public:
    bus_stop_corner(){}
    bus_stop_corner(const gps_position & lat_, const gps_position & long_,
        const std::string & s1_, const std::string & s2_
    ) :
        bus_stop(lat_, long_), street1(s1_), street2(s2_)
    {}
};
```
请注意基类从派生类中的序列化. 不要直接调用基类的序列化函数. 这样做可能看起来很有效, 但会绕过追踪写入存储的实例的代码, 以消除冗余. 它也会绕过将类的版本信息写入存档. 处于这个原因, 我们最好是将成员的序列化函数变成私有的. \
声明 **``friend boost::serialization::access``** 将授予序列化库对私有变量和函数的访问权限.
## Pointers
假设我们将一条公交车线路定义为一些公交车站的组合. 基于: \
1. 我们可能有几种类型的公交车站(记住 **`bus_stop`** 是一个基类).
2. 一个给定的公交车站可能出现在多条线路中.

用指向 **`bus_stop`** 的指针的数组来表示一条公交车线路.
```c++
class bus_route
{
    friend class boost::serialization::access;
    bus_stop * stops[10];
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        int i;
        for(i = 0; i < 10; ++i)
            ar & stops[i];
    }
public:
    bus_route(){}
};
```
**`stops`** 数组的每一个成员都将被序列化. 但是请记住每个成员都是一个指针--那这到底意味着什么呢? 这种序列化的目标是为了能够在另一个地方和时间重建原始数据结构. 为了用指针来实现这一点, 仅仅保存指针的值是不够的, 必须保存它所指向的对象. 当后来加载时, 必须创建一个新的对象, 并将一个新的指针加载到类成员中.\
如果同一个指针被序列化了不止一次, 那么只有一个实例被添加到存档中. 当读回时, 没有数据被读回. 唯一发生的操作是, 第二个指针被设为等于第一个指针.\
注意到, 在这个例子中, **`stops`** 数组由多态指针组成. 也就是说, 每个数组元素都指向几种可能的公交车站中的一个. 因此, 当指针被保存时, 必须保存某种类型的标识符. 当指针加载回来时, 必须读取类的标识符, 并且必须构建相应类的实例. 最后, 数据可以加载到新创建的正确类型的实例中. 在[demo.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo.cpp)中可以看到, 通过基类指针对派生类的序列化, 可能需要对序列化的派生类进行明确的枚举. 这被称作派生类的 "注册" 或 "导出" .[这里](https://www.boost.org/doc/libs/1_81_0/libs/serialization/doc/serialization.html#derivedpointers)将详细解释这一要求和满足这一要求的方法.\
这些操作都是由序列化库自动完成的. 上述代码是对通过指针访问的对象完成保存和加载操作的全部必要条件.
## Arrays
上面的表述其实没必要那么复杂. 序列化库会检测到被序列化的对象是一个数组, 并发出等同于上述的代码. 所以上面的内容可以简化为:
```c++
class bus_route
{
    friend class boost::serialization::access;
    bus_stop * stops[10];
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & stops;
    }
public:
    bus_route(){}
};
```
## STL Collections
上面的例子使用了一个成员的数组. 更有可能的是, 这样的应用可以用一个STL集合来实现. 序列化库包含了所有STL类的序列化的代码. 因此, 下面的重述也正如期望一般实现了.
```c++
#include <boost/serialization/list.hpp>

class bus_route
{
    friend class boost::serialization::access;
    std::list<bus_stop *> stops;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & stops;
    }
public:
    bus_route(){}
};
```
## Class Versioning
假设我们对 **`bus_route`** 类很满意, 建立了一个使用它的程序, 并发行了产品. 一段时间后, 我们决定对程序进行改进, 于是对 **`bus_route`** 类进行了修改, 加入了该路线的司机的名字, 因此新的版本如下所示:
```c++
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

class bus_route
{
    friend class boost::serialization::access;
    std::list<bus_stop *> stops;
    std::string driver_name;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & driver_name;
        ar & stops;
    }
public:
    bus_route(){}
};
```
很好, 我们都完成了. 除了...使用我们程序的人, 现有有一堆在以前的程序下创建的存档文件, 怎么办? 这些文件如何在我们的新程序版本中使用?\
一般来说, 序列化库为每个序列化的类在存档中存储了一个版本号. 默认情况下, 这个版本号是 0. 当加载存档文件时, 会读取它保存的版本号. 可以利用这一点对上面的代码进行修改:
```c++
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

class bus_route
{
    friend class boost::serialization::access;
    std::list<bus_stop *> stops;
    std::string driver_name;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // only save/load driver_name for newer archives
        if(version > 0)
            ar & driver_name;
        ar & stops;
    }
public:
    bus_route(){}
};

BOOST_CLASS_VERSION(bus_route, 1)
```
通过对每个类应用版本管理, 就不需要试图去维护文件的版本. 也就是说, 一个文件的版本是其所有构成类别的版本的组合. 这个系统允许程序始终与所有以前旧版本的程序创建的存档兼容, 而不需要做出比这个例子更多的工作.
## Splitting serialize into save/load
序列化函数简单、简洁, 并保证类成员以相同的顺序被保存和加载--这是序列化系统的关键. 然而, 在有些情况下, 加载和保存操作并不和这里的例子那么相似. 例如, 这可能发生在一个经过多个版本更迭演变的类中. 上述的类可以重新表述为:
```c++
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

class bus_route
{
    friend class boost::serialization::access;
    std::list<bus_stop *> stops;
    std::string driver_name;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        // note, version is always the latest when saving
        ar  & driver_name;
        ar  & stops;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        if(version > 0)
            ar & driver_name;
        ar  & stops;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
    bus_route(){}
};

BOOST_CLASS_VERSION(bus_route, 1)
```
**`BOOST_SERIALIZATION_SPLIT_MEMBER()`** 宏产生的代码, 根据存档是用于保存还是加载来调用 **`save`** 或 **`load`** 函数.
## Archives
我们讨论的重点是为类添加序列化能力. 要被序列化的数据实际的渲染是在 **`archive`** 类中实现的. 因此, 序列化的数据流是类的序列化和被选存档的产物. 这是一个关键的设计选择, 即这两个组件是独立的. 这允许任何序列化规范可用于任何存档.\
在本教程中, 我们使用了一个特殊的归档类-- **`text_oarchive`** 用于保存而 **`text_iarchive`** 用于加载. 文本存档以文本的形式呈现数据, 可以跨平台移植. 除了文本存档, 该库还包括用于本地二进制数据和XML格式数据的存档类. 所有存档类的接口都是相同的. 一旦为一个类定义了序列化, 该类可以被序列化为任何类型的存档.\
如果当前的存档类不能提供特定应用所需的属性、格式或行为, 可以制作一个新的存档类或从现有的存档类中派生出来. 这将在本手册的后面描述.
## List of Examples
- [demo.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo.cpp)
  这是本教程中完整的例子. 它做了以下事情:
  1. 创建了一个由不同种类的站点, 路线和时间计划表组成的结构
  2. 显示该结构
  3. 将其序列化为一个名为 **`testfile.txt`** 的文件
  4. 从文件中加载回复到另一个结构
  5. 显示回复后的结构
   
  这个程序的[输出](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_output.txt)足以验证这个系统是否满足了最初说的对序列化系统的所有要求. [存档文件](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demofile.txt)的内容也可以显示, 因为序列化文件是ASCII文本.
- [demo_xml.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_xml.cpp)
  这是 **`demo`** 的变体. 除了其他类型, 还支持XML存档. **`BOOST_SERIALIZATION_NVP(name)`** 这个额外包装的宏, 将数据项的名称与相应的xml标签联系起来. 重要的是,  **`name`** 是一个有效的xml标签, 否则无法恢复存档. 更多的信息请参考[Name-Value Pairs](https://www.boost.org/doc/libs/1_81_0/libs/serialization/doc/wrappers.html#nvp). 这里是一个[xml存档](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_save.xml)的例子.
- [demo_xml_save.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_xml_save.cpp) 和 [demo_xml_load.cpp](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_xml_load.cpp)
  还要注意的是, 尽管我们的例子是在同一个程序中保存和加载数据到一个存档中, 但这只是为了方便说明问题. 在一般情况下, 存档可能被创建它的程序加载, 也可能不被加载.

聪明的读者可能会注意到, 这些例子包含一个微妙但重要的缺陷. 它们泄露了内存. 公交车站是在主函数中创建的. 公交车计划时间表可以多次引用这些公交车站. 在主函数结束时, 公交车计划时间表被销毁后, 公交车站也被销毁. 这看起来似乎很好. 但是, 在从存档中加载的过程中创建的结构 **`new_schedule`** 又是怎么回事呢? 它包含了自己独立的公交车站, 这些站点在公交车计划时间表之外没有被引用. 它们不会在程序中任何地方被销毁--内存泄漏.\
有几种方法可以解决这个问题. 一种方法是明确地管理公交车站. 然而, 一个更稳健和透明的方法是使用 **`shared_ptr`** 而不是原始指针. 除了标准库的序列化实现之外, 序列化库还包括 **`boost::shared_ptr`** 的序列化实现. 鉴于此, 改变这些例子中的任何一个内存泄漏应该是很容易的. 这是留给读者的一个练习题. ([解答例子](https://www.boost.org/doc/libs/1_81_0/libs/serialization/example/demo_shared_ptr.cpp))

---
© Copyright Robert Ramey 2002-2004. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
