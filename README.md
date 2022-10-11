# pointName
===========================

## 环境依赖
**java 1.8
mysql 8.0.3
mysql-connector-java-8.0.3.jar
idea**

## 部署步骤及运行过程

**1. 启动mysql server 8.0.3
在终端输入:mysql -u root -p
2. 利用idea打开指定项目——pointName
3. 在idea中配置所需数据库
 [配置数据库](https://blog.csdn.net/qq_61407171/article/details/121705452)
数据库具体配置如下图：password为dax71205**
![](https://img-blog.csdnimg.cn/ad58c825e6d84149bbfa55c5a53d32e7.png)


4. 在idea中运行run类下的main方法
 
5. **终端输出20次E的值**，并在数据库point中产生两张table，表**records**记载450名学生的个人信息以及其出勤的课程与**出勤情况**(用20位的字符串表示，0代表未出勤，1代表出勤)，而表**pointrecords**记载着450名学生的个人信息以及**点名记录**(用20位的字符串表示，0代表未点名到，1代表有效点名即点名到逃课学生，2代表无效点名)
6. 可用mysql在数据库中查看两张表的信息
```
mysql语句：
use point; //选中point数据库
select * from records; //查看records表下的所有信息
select * from pointrecords; //查看pointrecords表下的所有信息
ps:若只想查看学生的出勤记录与点名记录，select course from records/pointrecords;即可
```


## 目录结构描述
**MAIN
│  README.md 
│
└─pointName
&emsp;│  Class.iml
&emsp;│
&emsp;├─.idea
&emsp;│  │  .gitignore
&emsp;│  │  misc.xml
&emsp;│  │  modules.xml
&emsp;│  │  uiDesigner.xml
&emsp;│  │  workspace.xml
&emsp;│  │
&emsp;│  └─codeStyles
&emsp;│          codeStyleConfig.xml
&emsp;│          Project.xml
&emsp;│
&emsp;└─Class
&emsp;&emsp;Class.iml 
&emsp;&emsp;BuildFirstName.java //初始化姓氏
&emsp;&emsp;BuildLastName.java //初始化名字
&emsp;&emsp;BuildName.java //初始化姓名
&emsp;&emsp;Course.java //单节课程对象
&emsp;&emsp;Generate.java //各种信息的初始化
&emsp;&emsp;Lessons.java //课程总表
&emsp;&emsp;Read.java //连接数据库并从数据库上读取信息
&emsp;&emsp;Run.java //运行函数
&emsp;&emsp;Sample.java //点名
&emsp;&emsp;Student.java //单个学生对象
&emsp;&emsp;Students.java //学生总表**




## V1.0.0 版本内容
**详细内容可见[博客](https://blog.csdn.net/dongy_u/article/details/127268799?spm=1001.2014.3001.5502
)**

