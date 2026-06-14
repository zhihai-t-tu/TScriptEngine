## 工程

### 目标


```java
1. 可在C++应用程序中执行代码脚本的脚本引擎。
2. 能够在C++应用程序中，扩展脚本引擎的功能。
3. 变量、函数、类名允许使用中文或中英文结合，可应用于需要中文支持的场景。
4. 核心代码完全使用C++11标准库，集成到C++应用中无需进行代码移植。
5. 核心代码需在遵从LICENSE的条件下，可以自由分发、修改、使用。
```



### 引擎核心代码（C++11）：


```java
tscriptengine.h
tscriptengineimpl.h
tscriptengineimpl.cpp
```



### 开发环境


```java
VS2015 + Qt5.9.8
编码：UTF-8
```



## 语法

### 变量


```java
1. 由let声明的局部变量
let myVal1;
let myVal2 = 1;
let 本地变量Var3="I am var!";

2. 模块级变量
myVal4 = 4;  //未使用let声明

3. 主模块变量
myVal5 = 5; //在主模块中使用，且未使用let声明。
myVal6 = globalVal7; //globalVal7由TScriptEngine::initVarValue设置，或者由TScriptEngine::bindGetVarFunc绑定的函数获得。
```


### 运算符


```java
单目： ++, --, !
双目: +, -, *, /, %, +=, -=, *=, /=, %=, &&, ||, <<, >>, &&=, ||=, >>=, <<=, !=
比较：<, <=, >, >=, ==

其中，++仅支持 val ++;--仅支持 var --;
```


### 函数

#### 1. 由引擎内置的函数，全局函数

```java
输出函数：
debug(str),printf(s,...),console(s)
数学函数：
sin(arc),cos(arc),tan(arc),asin(v),acos(v),atan(v),atan2(l,h),pow(x,y),log(v),log2(v),floor(v),ceil(v),abs(v),min(a,b),max(x,y),sum(x,...),avg(x,...),exp(v),sqrt(v)
转换函数：
atoi(s),atol(s),atof(s),itoa(i),ltoa(l),ftoa(f)
字符串函数：
lower(s),upper(s),strcmp(v1,v2)
判断函数：
isNull(v)
时间函数：
milliseconds(),datetime(),date(),time()
系统函数：
getProjectPath()
内置对象生成函数：
Array();ByteArray();NativeObject(classObject),Map(k1,v1,k2,v2,...)
```


#### 2. 由TScriptEngine::bindUserFunc和TScriptEngine::bindUserDataFunc注册的函数，全局函数


```java
    scriptEngine.bindUserFunc(u8"sleep", [](TScript::TScriptValue & value)->TScript::TScriptValue{
        QThread::sleep(value.toLongLong());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"msleep", [](TScript::TScriptValue & value)->TScript::TScriptValue{
        QThread::msleep(value.toLongLong());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"CreateNativeThread", [](TScript::TScriptValue & threadObject)->TScript::TScriptValue{
        return std::shared_ptr<TScriptNativeObject>(new TScriptNativeThreadObject(threadObject.getNativeObject()));
    });
    scriptEngine.bindUserFunc(u8"CreateNativeMutex", []()->TScript::TScriptValue{
        return std::shared_ptr<TScriptNativeObject>(new TScriptNativeMutexObject());
    });
```


#### 3. 数值函数


```java
字符串函数
toInt(),toLong(),toDouble(),length(),size(),charAt(n),indexOf(substr,...),lastIndexOf(substr,...),trim(),substring(s,...),erase(s,...),empty(),append(s),push_back(s),push_front(s),truncate(s,...),chop(s,...),replace(sub,newsub),split(div)
 
Array函数
length(),size(),indexOf(e),resize(n),reverse(),clear(),remove(),join(array),push_front(e),push_back(e),append(e),pop_front(),pop_back(),empty(),slice(n)
 
ByteArray函数
lengh(),size(),charAt(n),reverse(),clear(),remove(n,...),removeAt(n),push_front(byteArray),push_back(byteArray),insert(n,byteArray),empty(),toString()
 
Map函数
empty(),length(),clear(),contains(k),get(k),put(k,v)
```


#### 4. 由function声明并定义的函数，局部函数


```java
function 中文名函数(param,...) {
	function 局部函数(param,...) {
	}
	return 局部函数(param,...);
}
如果函数没有显示的return value，则最后一条语句执行的结果，作为函数的返回值。
```


### 类


```java
class 中文名类 : 父类名 {

	let  memVar;
	
	//构造函数，非必须
	function 中文名类(param,...) : 父类名(param + "_supper") {
		
	}
	//析构函数，非必须
	function ~中文名类() {
	}
	
	function memFunc(param, ...) {
		return memVar;
	}
    
};
```


### 语句


#### 1. 复制语句


```java
myRes = myVal;
```


#### 2. 循环语句


```java
for(...;...;...) {
}

while(...) {
}

do{
...
}while(...);
```


#### 3. 控制语句


```java
continue;
break;
return;
return value;
```


#### 4. 条件语句


```java
if(...) {
};

if(...) {
} else if(...) {
};

if(...) {
} else if(...) {
} else {
};
```


### 模块引用语句


```java
import "/path1/file";	//相对于getProjectPath() + "/lib"
import "path1/file";	//相对于当前模块
import "../path1/file";	//相对于当前模块
```



## 示例

参考examples下的示例源码。
