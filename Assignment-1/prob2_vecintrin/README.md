# 使用 SIMD 内部函数向量化代码
## 运行结果截图(均在N = 100000的规模下运行) CLAMPED EXPONENT
### Vector_Width = 2
![7SWER9K42_`BO{G5 QXHSIG](https://user-images.githubusercontent.com/97599487/195807487-0ad3de87-0435-4fbe-afa3-7048410be357.png)
### Vector_Width = 4
![O~AONN3GQ B(4U{97EXWHY0](https://user-images.githubusercontent.com/97599487/195807502-11816627-682b-4285-8ec5-2995b21bb136.png)
###  Vector_Width = 8
![~GA% Q43{UHH6U90Y2Q1{ W](https://user-images.githubusercontent.com/97599487/195807514-0e553f95-d86e-4eb3-9a60-68e362309c1f.png)
### Vector_Width = 16
![$O()K)WE`3CORMQW~@98MU](https://user-images.githubusercontent.com/97599487/195807525-285b8432-2ac6-4747-a5be-26f785d90755.png)
### Vector_Width = 32
![T6%Y~B(OGV OT{ HT_ USP8](https://user-images.githubusercontent.com/97599487/195807543-b61198e5-cd4e-4e3b-8238-3713a25474ce.png)

## SUM函数向量化
### Vector_Width = 2
![5LT7V57QW_B%%54I2KLB%5J](https://user-images.githubusercontent.com/97599487/195807553-c66423af-71e2-46b0-9baf-b66170922d0b.png)
### Vector_Width = 16
![P70GR`QCS98UKLQ9_R6GYA5](https://user-images.githubusercontent.com/97599487/195807565-89d8b848-6f17-4a4e-aa81-8ab49b55e5f7.png)

## 卡了半天的BUG
![1003120963-2557064826-BFAF1AC09AC83F4F53915688538D171B](https://user-images.githubusercontent.com/97599487/195807813-118a6980-8280-4fb5-af0e-811f035d4f47.jpg)
![1003120963-897908768-0FF41D0B62345E403F4F5D03D30A8C5D](https://user-images.githubusercontent.com/97599487/195807842-7e91cbe9-ebd2-4460-91fb-c9e4227daa16.jpg)
**每次循环迭代的开头 掩码要记得归0**
### 教训总结
正常来说 一个变量 如果由另外一个变量在此次循环中 生成 比如a=b，那么他不需要初始化一次，因为可以覆盖，但是掩码运算中有屏蔽操作，八位掩码中，覆盖操作是不完全的，因为有几位被屏蔽掉了，导致新生成的掩码实际上还有上一次循环残留的几位是true or false，如果是false 那没有影响，但如果是true 那么就会导致 你生成的这个新的掩码，他实际上该屏蔽的没有屏蔽到 血与泪的教训 希望你们不要重蹈覆辙 hah 我自然而然的想成覆盖操作了。重点就是 屏蔽是保留原有值 而不是将屏蔽位变成false
