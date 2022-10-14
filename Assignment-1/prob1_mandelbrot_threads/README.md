# 使用 Pthreads 的并行分形生成
##  pthread实现多线程并行，观察t = 不同值的时候，加速比的变化，并探究是否是线性关系?(应实现一份代码适用于所有的线程数)
并不是单纯的线性关系，由于我的运行环境是在8核下进行，所以可以看到，t = 8 与 t = 16 并无明显变化，这是因为如果开16线程，那么部分线程间(同一个进程下的)，仍然是并发的关系，并没有形成并行。但在此之前，加速比的变化是与t值呈线性关系的。

###  t = 2
![Q}%R0LC046@NF4NM%0{8_}V](https://user-images.githubusercontent.com/97599487/195805418-fb0da7fc-3676-404f-8595-f23496159102.png)
### t = 4
![HL(HUW1XATZ~@BOUU@ Z_KQ](https://user-images.githubusercontent.com/97599487/195805545-6339c48f-bb75-4034-a465-8c19c7a17b5e.png)
### t = 8
![0CI)RKDX7CIK@ZEJ$%AN}Y5](https://user-images.githubusercontent.com/97599487/195805666-b1acbb43-2283-4df3-b69a-7bc1db350f27.png)
### t = 16
![8BWIC3)}2UFC$~8K3}_7$PP](https://user-images.githubusercontent.com/97599487/195805694-c62ad6d9-645b-4f1b-927a-8097d9aee370.png)


##  ps：代码中并未删除对单个线程的计时，若想得出以上截图，自行删除即可。





