# CUDA 热身 2:并行前缀和  
## exclusive_scan()函数的cuda化  
![H8LW{B52{9{HVIW068 L((0](https://user-images.githubusercontent.com/97599487/198812347-3a53a5bb-a8d2-4885-a5c0-b2d5cc64cbfe.png)  
### 相关要点
  在不同层间记得要同步一下，然后上半部分的计算可以少算一步(a0-7 + a8-15),其给出的串行实现将这步计算进去了，但因为下一步要赋为0，所以没必要。将a15赋值为0的操作，
我是在核函数的下半部分计算中赋值的，限制了一下让所有线程等待某一线程将a15赋值为0后在继续往下运行(这个部分可能有更优解法，因为我加了个核内同步，性能下降了一点).  
### 结果截图
![_`U H0YR$}SXLI77( _FWEK](https://user-images.githubusercontent.com/97599487/198812638-e27b3944-f9fb-4f78-b451-165b01017e5e.png)  

![0`XYW{1CZ~K 194N_5GBKNT](https://user-images.githubusercontent.com/97599487/198812670-1df4631f-07b3-40af-8aec-1f319e2a414b.png)


## find_peaks()函数的cuda化  
### 相关要点
这个我是用的线性遍历的cuda化，按题目的意思或许是要用上上面辣个函数...但是我没想出来用前缀和求峰值有啥优势hah，所以就做了个阉割版hh.

### 结果截图
![`SIFWU@L}WANJ)0K`0`WZ F](https://user-images.githubusercontent.com/97599487/198812754-b8409b2c-92ea-4a3a-a045-f987e7b6036f.png)  
![Z7(_3{LX3NWN6@46820Y8VU](https://user-images.githubusercontent.com/97599487/198812756-9d71f684-34f9-42d6-a9b3-6e7634deb46c.png)
