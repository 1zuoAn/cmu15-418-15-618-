# 使用 Pthreads 的并行分形生成
##  pthread实现多线程并行，观察t = 不同值的时候，加速比的变化，并探究是否是线性关系?(应实现一份代码适用于所有的线程数)
并不是单纯的线性关系，由于我的运行环境是在8核下进行，所以可以看到，t = 8 与 t = 16 并无明显变化，这是因为如果开16线程，那么部分线程间(同一个进程下的)，仍然是并发的关系，并没有形成并行。但在此之前，加速比的变化是与t值呈线性关系的。

###  t = 2
![](https://img-blog.csdnimg.cn/40be2bf29dd9416c9e51c3142f365cf2.png)  
### t = 8
![](https://img-blog.csdnimg.cn/afa84e0b92d14fc5887e8a8369ad89e1.png)  
### t = 4
![](https://img-blog.csdnimg.cn/edbdaca8b7a44e33b9faeceea0b69da7.png)
### t = 16
![](https://img-blog.csdnimg.cn/c2a3b9562492409090b5f635a067a837.png)

##  ps：代码中并未删除对单个线程的计时，若想得出以上截图，自行删除即可。





